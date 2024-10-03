import { PixIROperand, PixIROpcode, PixIRInstruction, instructions, PixIROperandType } from "./instructions"

type Frame = Array<any>

interface PixVMState {
    screenHandle: HTMLCanvasElement,
    loggerHandle: HTMLTextAreaElement,
    frameStack: Array<Frame>,
    workStack: Array<PixIROperand>
}

function safePop(state: PixVMState): PixIROperand {
    let x = state.workStack.pop();
    if (x === undefined)
        throw Error("Empty stack when operand is needed.");
    return x;
}

function checkOperandType(x: PixIROperand, expected: PixIROperandType) {
    if (x.operandType != expected) {
        throw Error(`Invalid operand type given, expected ${expected}, got ${x.operandType}.`)
    }
}

function executeInstr(state: PixVMState, instr: PixIRInstruction) {
    switch (instr.opcode) {
        // mathematical operations
        case PixIROpcode.ADD: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) + (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.SUB: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) - (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.MUL: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) * (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.DIV: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) / (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.INC: {
            let x = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) + 1
            });
            break;
        }

        case PixIROpcode.DEC: {
            let x = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) - 1
            });
            break;
        }

        case PixIROpcode.MAX: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.max(x.operandValue as number, y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.MIN: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);
            checkOperandType(y, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.min(x.operandValue as number, y.operandValue as number)
            });
            break;
        }

        // get random number
        case PixIROpcode.IRND: {
            let x = safePop(state);

            checkOperandType(x, PixIROperandType.NUMBER);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.random() * (x.operandValue as number)
            });
            break;
        }

        // logical operations
        case PixIROpcode.AND: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.BOOLEAN);
            checkOperandType(y, PixIROperandType.BOOLEAN);

            state.workStack.push({
                operandType: PixIROperandType.BOOLEAN, 
                operandValue: (x.operandValue as boolean) && (y.operandValue as boolean)
            });
            break;
        }

        case PixIROpcode.OR: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperandType(x, PixIROperandType.BOOLEAN);
            checkOperandType(y, PixIROperandType.BOOLEAN);

            state.workStack.push({
                operandType: PixIROperandType.BOOLEAN, 
                operandValue: (x.operandValue as boolean) || (y.operandValue as boolean)
            });
            break;
        }

        case PixIROpcode.NOT: {
            let x = safePop(state);

            checkOperandType(x, PixIROperandType.BOOLEAN);

            state.workStack.push({
                operandType: PixIROperandType.BOOLEAN, 
                operandValue: !(x.operandValue as boolean)
            });
            break;
        }
    }
}