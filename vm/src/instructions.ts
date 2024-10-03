export enum PixIROpcode {
    // mathematical operations
    ADD = "add",
    SUB = "sub",
    MUL = "mul",
    DIV = "div",
    INC = "inc",
    DEC = "dec",
    MAX = "max",
    MIN = "min",
    // get random number
    IRND = "irnd",
    // logical operations
    AND = "and",
    OR = "or",
    NOT = "not",
    LT = "lt",
    LE = "le",
    EQ = "eq",
    NEQ = "neq",
    GT = "gt",
    GE = "ge",
    // stack and control operations
    DUP = "dup",
    PUSH = "push",
    JMP = "jmp",
    CJMP = "cjmp",
    CJMP2 = "cjmp2",
    CALL = "call",
    RET = "ret",
    ST = "st",
    HALT = "halt",
    // allocate automatic vars, create stack frames.
    ALLOC = "alloc",
    OFRAME = "oframe",
    CFRAME = "cframe",
    // delay operation
    DELAY = "delay",
    // screen related operations
    PIXEL = "pixel",
    PIXELR = "pixelr",
    CLEAR = "clear",
    READ = "read",
    WIDTH = "width",
    HEIGHT = "height",
    // log output
    PRINT = "print",
    // array operations
    ALLOCA = "alloca",
    STA = "sta",
    LDA = "lda",
}

export enum PixIROperandType {
    COLOR = "color",
    NUMBER = "number",
    BOOLEAN = "boolean",
    LABEL = "label",
    PCOFFSET = "pcoffset",
    FUNCTION = "function",
}

type Color = string
type Label = [number, number]

export interface PixIROperand {
    operandType: PixIROperandType
    operandValue: Color | number | boolean | Label
}

export interface PixIRInstruction {
    opcode: PixIROpcode,
    operand: PixIROperand | undefined
}

function isAlphaNum(c: string): boolean {
    let charCode = c.charCodeAt(0);
    return (charCode > 47 && charCode < 58) ||
            (charCode > 96 && charCode < 123) ||
            (charCode > 64 && charCode < 91);
}

function readOperand(opStr: string): PixIROperand {
    // try checking if operand is a boolean
    if (opStr == "true")
        return {operandType: PixIROperandType.BOOLEAN, operandValue: true};
    if (opStr == "false")
        return {operandType: PixIROperandType.BOOLEAN, operandValue: false};

    // try checking if operand is a number
    let numValue = parseFloat(opStr);
    if (!isNaN(numValue))
        return {operandType: PixIROperandType.NUMBER, operandValue: numValue};

    // try checking if operand is a function name
    if (opStr[0] == ".") {
        // validate function name
        if (opStr.length == 1)
            throw Error(`Invalid function name ${opStr} given.`);
        for (let i = 1; i < opStr.length; i++) {
            if (!isAlphaNum(opStr.at(i) as string))
                throw Error(`Invalid function name ${opStr} given.`);
        }
        return {operandType: PixIROperandType.FUNCTION, operandValue: opStr};
    }

    // try checking if operand is a label
    if (opStr[0] == "[" && opStr[opStr.length - 1] == "]") {
        let label = opStr.substring(1, opStr.length - 1);
        let splitLabel = label.split(":", 1);
        let offset = parseInt(splitLabel[0]);
        let frame = 0;
        if (splitLabel.length > 1)
            frame = parseInt(splitLabel[1]);
        if (isNaN(offset) || isNaN(frame))
            throw Error(`Invalid label ${opStr} found.`);
        return {operandType: PixIROperandType.LABEL, operandValue: [offset, frame]};
    }

    // try checking if operand is a PC offset
    if (opStr.substring(0, 3) == "#PC") {
        let offset = parseInt(opStr.substring(3));
        if (isNaN(offset))
            throw Error(`Invalid PC offset ${opStr} found.`);
        return {operandType: PixIROperandType.PCOFFSET, operandValue: offset};
    }

    // try checking if operand is a Colour
    let isColor = opStr.length == 7;
    if (isColor) {
        isColor &&= opStr[0] == "#";
        // check that rest of string after # is a hex literal.
        isColor &&= !isNaN(parseInt(opStr.substring(1), 16))
    }
    if (!isColor)
        throw Error(`Invalid operand ${opStr} found.`);
    return {operandType: PixIROperandType.COLOR, operandValue: opStr};
}

export function* instructions(src: string): Generator<PixIRInstruction> {
    while (src.length != 0) {
        let splitSrc = src.split("\n", 1);
        // update src to remainder
        src = splitSrc[1];
        
        // get opcode within the next instruction
        let instr = splitSrc[0];
        let splitInstr = src.split(" ", 1);
        if (!(splitInstr[0] in PixIROpcode))
            throw Error(`${splitInstr[0]} is not a valid instruction.`);
        let opcode = splitInstr[0] as PixIROpcode;
        
        // get opcode operand if the opcode is a push instruction
        let operand = undefined;
        if (opcode == PixIROpcode.PUSH) {
            if (splitInstr.length == 1)
                throw Error("Operand for push instruction was not specified.");
            operand = readOperand(splitInstr[1]);
        }

        yield {opcode, operand}
    }
}