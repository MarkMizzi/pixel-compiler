import {
  type PixIRInstruction,
  PixIROpcode,
  type PixIRData,
  PixIRDataType,
  readInstr,
  validateFunctionName
} from './instructions'

export type Program = Map<string, Array<PixIRInstruction>>

export class Assembler {
  /* Validate a program generated by the Assembler instance.
   *
   * Currenly the following checks are implemented:
   * - Check that each function referenced in a push instruction exists in the program.
   * - Check that the program contains a .main function (this is the entry point).
   */
  private validate(program: Program) {
    // check that each function referenced in push instruction is within the program.
    for (const [_, instrs] of program) {
      for (const instr of instrs) {
        if (instr.opcode == PixIROpcode.PUSH) {
          const operand = instr.operand as PixIRData
          if (operand.dtype == PixIRDataType.FUNCTION && !program.has(operand.val as string)) {
            throw SyntaxError(`Function ${operand!.val} not found.`)
          }
        }
      }
    }

    if (!program.has('.main')) throw SyntaxError('Program does not have a .main function.')
  }

  /* Assemble an input source string into an actual Program object that
   * can be executed by the machine.
   */
  public assemble(src: string): Program {
    let program = new Map()
    let currFuncName = undefined

    const srcLines = src.split('\n')

    for (let line of srcLines) {
      // remove extra whitespace
      line = line.trim()

      // skip empty or comment lines
      if (line.length == 0 || line[0] == '#') continue

      // check if this line starts a new function
      if (line[0] == '.') {
        currFuncName = line
        validateFunctionName(currFuncName)
        program.set(currFuncName, [])
        continue
      }

      if (currFuncName === undefined)
        throw SyntaxError('Invalid program; instructions encountered outside of a function.')

      // line must be a new instruction; read it and add it to current function.
      program.get(currFuncName).push(readInstr(line))
    }

    this.validate(program)
    return program
  }
}
