/* Type representing a VM Opcode */
export enum PixIROpcode {
  // mathematical operations
  ADD = 'add',
  SUB = 'sub',
  MUL = 'mul',
  DIV = 'div',
  INC = 'inc',
  DEC = 'dec',
  MAX = 'max',
  MIN = 'min',
  // get random number
  IRND = 'irnd',
  // logical operations
  AND = 'and',
  OR = 'or',
  NOT = 'not',
  LT = 'lt',
  LE = 'le',
  EQ = 'eq',
  NEQ = 'neq',
  GT = 'gt',
  GE = 'ge',
  // stack and control operations
  DUP = 'dup',
  PUSH = 'push',
  JMP = 'jmp',
  CJMP = 'cjmp',
  CJMP2 = 'cjmp2',
  CALL = 'call',
  RET = 'ret',
  HALT = 'halt',
  // allocate automatic vars, create stack frames.
  ALLOC = 'alloc',
  OFRAME = 'oframe',
  CFRAME = 'cframe',
  ST = 'st',
  // delay operation
  DELAY = 'delay',
  // screen related operations
  PIXEL = 'pixel',
  PIXELR = 'pixelr',
  CLEAR = 'clear',
  READ = 'read',
  WIDTH = 'width',
  HEIGHT = 'height',
  // log output
  PRINT = 'print',
  // array operations
  ALLOCA = 'alloca',
  STA = 'sta',
  LDA = 'lda'
}

/* Data type enum used to tag VM data so that it can be type-checked at runtime. */
export enum PixIRDataType {
  COLOR = 'color',
  NUMBER = 'number',
  LABEL = 'label',
  // relative offset from PC.
  PCOFFSET = 'pcoffset',
  // function address
  FUNCTION = 'function',
  ARRAY = 'array'
}

/* A Color is a CSS RGB hex string, e.g. #ff57 */
export type Color = string
/* A Label is a location in VM memory, consisting of an [offset, frame] pair.
 * Data pointed to by the label will be in the offset^th position of frame.
 */
type Label = [number, number]
/* Data type for function labels. */
export type FunctionName = string

/* Represents data type used by the VM (e.g. in work stack and frames). */
export interface PixIRData {
  dtype: PixIRDataType
  val: Color | number | boolean | Label | FunctionName | Array<PixIRData | undefined>
}

/* Represents a VM instruction (opcode + any operand data) */
export interface PixIRInstruction {
  opcode: PixIROpcode
  operand: PixIRData | undefined
}

/* Utility function to convert rgb numbers into Color hex string. */
export function rgbToHex(r: number, g: number, b: number): Color {
  if (r > 255 || g > 255 || b > 255) throw Error(`Invalid color component rgb(${r}, ${g}, ${b}).`)
  return '#' + ((r << 16) | (g << 8) | b).toString(16)
}

function isAlphaNum(c: string): boolean {
  let charCode = c.charCodeAt(0)
  return (
    (charCode > 47 && charCode < 58) ||
    (charCode > 96 && charCode < 123) ||
    (charCode > 64 && charCode < 91)
  )
}

export function validateFuncName(funcName: FunctionName) {
  // validate function name
  if (funcName.length <= 1 || funcName.at(0) != '.')
    throw Error(`Invalid function name ${funcName} found.`)
  for (let i = 1; i < funcName.length; i++) {
    if (!isAlphaNum(funcName.at(i) as string))
      throw Error(`Invalid function name ${funcName} found.`)
  }
}

export function checkOperand(x: PixIRData, expectedTypes: Array<PixIRDataType>) {
  if (!(x.dtype in expectedTypes)) {
    throw Error(`Invalid operand type given, expected one of ${expectedTypes}, got ${x.dtype}.`)
  }
}

function readOperand(opStr: string): PixIRData {
  // try checking if operand is a number
  let numValue = parseFloat(opStr)
  if (!isNaN(numValue)) return { dtype: PixIRDataType.NUMBER, val: numValue }

  // try checking if operand is a function name
  if (opStr[0] == '.') {
    // validate function name
    validateFuncName(opStr[0])
    return { dtype: PixIRDataType.FUNCTION, val: opStr }
  }

  // try checking if operand is a label
  if (opStr[0] == '[' && opStr[opStr.length - 1] == ']') {
    let label = opStr.substring(1, opStr.length - 1)
    let splitLabel = label.split(':', 1)
    let offset = parseInt(splitLabel[0])
    let frame = 0
    if (splitLabel.length > 1) frame = parseInt(splitLabel[1])
    if (isNaN(offset) || isNaN(frame)) throw Error(`Invalid label ${opStr} found.`)
    return { dtype: PixIRDataType.LABEL, val: [offset, frame] }
  }

  // try checking if operand is a PC offset
  if (opStr.substring(0, 3) == '#PC') {
    let offset = parseInt(opStr.substring(3))
    if (isNaN(offset)) throw Error(`Invalid PC offset ${opStr} found.`)
    return { dtype: PixIRDataType.PCOFFSET, val: offset }
  }

  // try checking if operand is a Colour
  let isColor = opStr.length == 7
  if (isColor) {
    isColor &&= opStr[0] == '#'
    // check that rest of string after # is a hex literal.
    isColor &&= !isNaN(parseInt(opStr.substring(1), 16))
  }
  if (!isColor) throw Error(`Invalid operand ${opStr} found.`)
  return { dtype: PixIRDataType.COLOR, val: opStr }
}

export function readInstr(line: string): PixIRInstruction {
  let splitInstr = line.split(' ', 1)
  if (!(splitInstr[0] in PixIROpcode)) throw Error(`${splitInstr[0]} is not a valid instruction.`)
  let opcode = splitInstr[0] as PixIROpcode

  // get opcode operand if the opcode is a push instruction
  let operand = undefined
  if (opcode == PixIROpcode.PUSH) {
    if (splitInstr.length == 1) throw Error('Operand for push instruction was not specified.')
    operand = readOperand(splitInstr[1])
  }

  return { opcode, operand }
}
