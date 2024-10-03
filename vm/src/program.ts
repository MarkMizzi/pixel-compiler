import { PixIRInstruction, PixIROpcode, PixIROperand, PixIROperandType, readInstr } from "./instructions";

type Program = Map<string, Array<PixIRInstruction>>

function validateProgram(program: Program) {
    // check that each function referenced in push instruction is within the program.
    for (let [_, instrs] of program) {
        for (let instr of instrs) {
            if (instr.opcode == PixIROpcode.PUSH) {
                let operand = instr.operand as PixIROperand;
                if (operand.operandType == PixIROperandType.FUNCTION && !((operand.operandValue as string) in program)) {
                    throw Error(`Function ${operand!.operandValue} not found.`);
                }
            }
        }
    }

    if (!(".main" in program))
        throw Error("Program does not have a .main function.");
}

export function assembleProgram(src: string): Program {
    let program = new Map();
    let currFuncName = undefined;

    while (src.length != 0) {
        let line: string
        [line, src] = src.split("\n", 1);
        // remove extra whitespace
        line = line.trim();
        
        // skip empty or comment lines
        if (line.length == 0 || line[0] == "#")
            continue;

        // check if this line starts a new function
        if (line[0] == ".") {
            currFuncName = line;
            program.set(currFuncName, []);
        }

        if (currFuncName === undefined)
            throw Error("Invalid program; instructions encountered outside of a function.");
        
        // line must be a new instruction; read it and add it to current function.
        program.get(currFuncName).push(readInstr(line));
    }

    validateProgram(program);
    return program;
}