import { PixIROperand, PixIROpcode, PixIRInstruction, PixIROperandType, checkOperand } from "./instructions"

type Frame = Array<any>

interface PixVMState {
    screenHandle: HTMLCanvasElement,
    loggerHandle: HTMLTextAreaElement,
    frameStack: Array<Frame>,
    workStack: Array<PixIROperand>,
    callStack: Array<string>,
    pc: number
}

function safePop(state: PixVMState): PixIROperand {
    let x = state.workStack.pop();
    if (x === undefined)
        throw Error("Empty stack when operand is needed.");
    return x;
}


function executeInstr(state: PixVMState, instr: PixIRInstruction) {
    switch (instr.opcode) {
        // mathematical operations
        case PixIROpcode.ADD: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) + (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.SUB: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) - (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.MUL: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);
            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) * (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.DIV: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) / (y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.INC: {
            let x = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) + 1
            });
            break;
        }

        case PixIROpcode.DEC: {
            let x = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: (x.operandValue as number) - 1
            });
            break;
        }

        case PixIROpcode.OR:
        case PixIROpcode.MAX: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.max(x.operandValue as number, y.operandValue as number)
            });
            break;
        }

        case PixIROpcode.AND:
        case PixIROpcode.MIN: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.min(x.operandValue as number, y.operandValue as number)
            });
            break;
        }

        // get random number
        case PixIROpcode.IRND: {
            let x = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: Math.random() * (x.operandValue as number)
            });
            break;
        }

        // logical operations
        case PixIROpcode.NOT: {
            let x = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: 1 - ((x.operandValue as number) > 0 ? 1 : 0)
            });
            break;
        }

        case PixIROpcode.LT: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue < y.operandValue ? 1 : 0
            });
            break;
        }

        case PixIROpcode.LE: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue <= y.operandValue ? 1 : 0
            });
            break;
        }

        case PixIROpcode.EQ: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [y.operandType]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue == y.operandValue ? 1 : 0
            });
            break;
        }

        case PixIROpcode.NEQ: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [y.operandType]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue != y.operandValue ? 1 : 0
            });
            break;
        }

        case PixIROpcode.GT: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue > y.operandValue ? 1 : 0
            });
            break;
        }

        case PixIROpcode.GE: {
            let x = safePop(state);
            let y = safePop(state);

            checkOperand(x, [PixIROperandType.NUMBER]);
            checkOperand(y, [PixIROperandType.NUMBER]);

            state.workStack.push({
                operandType: PixIROperandType.NUMBER, 
                operandValue: x.operandValue >= y.operandValue ? 1 : 0
            });
            break;
        }

        // stack and control operations
        case PixIROpcode.DUP: {
            let x = safePop(state);

            state.workStack.push(x);
            state.workStack.push(x);
            break;
        }

        case PixIROpcode.PUSH: {
            state.workStack.push(instr.operand as PixIROperand);
            break;
        }

        case PixIROpcode.JMP: {
            let x = safePop(state);

            checkOperand(x, [PixIROperandType.PCOFFSET]);
            state.pc = x.operandValue as number;
            break;
        }

        case PixIROpcode.CJMP: {
            let offset = safePop(state);
            let cond = safePop(state);

            checkOperand(offset, [PixIROperandType.PCOFFSET]);
            checkOperand(cond, [PixIROperandType.NUMBER]);

            if (cond.operandValue as number != 0)
                state.pc = offset.operandValue as number;
            break;
        }

        case PixIROpcode.CJMP2: {
            let offset = safePop(state);
            let cond = safePop(state);

            checkOperand(offset, [PixIROperandType.PCOFFSET]);
            checkOperand(cond, [PixIROperandType.NUMBER]);

            if (cond.operandValue as number == 0)
                state.pc = offset.operandValue as number;
            break;
        }
    }
}