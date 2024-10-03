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

type Color = string

type PixIROperand = Color | number | boolean

export interface PixIRInstruction {
    opcode: PixIROpcode,
    operand: PixIROperand | undefined
}

function readOperand(opStr: string): PixIROperand {
    // try checking if operand is a boolean
    if (opStr == "true")
        return true;
    if (opStr == "false")
        return false;

    // try checking if operand is a number
    let numValue = parseFloat(opStr);
    if (!isNaN(numValue))
        return numValue;

    // try checking if operand is a Colour
    let isColor = opStr.length == 7;
    if (isColor) {
        isColor &&= opStr[0] == "#";
        // check that rest of string after # is a hex literal.
        isColor &&= !isNaN(parseInt(opStr.substring(1), 16))
    }
    if (!isColor)
        throw Error(`Invalid operand ${opStr} found.`);
    return opStr;
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