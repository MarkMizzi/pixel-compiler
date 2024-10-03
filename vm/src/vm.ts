import { FunctionName, PixIROperand, PixIROpcode, PixIRInstruction, PixIROperandType, checkOperand } from "./instructions"
import { Program } from "./program"

type Frame = Array<PixIROperand | undefined>

interface CallFrame {
    funcName: FunctionName,
    pc: number
}

interface PixVMState {
    screenHandle: HTMLCanvasElement,
    loggerHandle: HTMLTextAreaElement,
    frameStack: Array<Frame>,
    workStack: Array<PixIROperand>,
    // track function calls and return pointers.
    callStack: Array<CallFrame>,
    height: number,
    width: number,
    halted: boolean,
}

class PixelVM {
    private state: PixVMState;
    private program: Program;
    
    constructor(screenHandle: HTMLCanvasElement, loggerHandle: HTMLTextAreaElement) {
        this.state = {
            screenHandle,
            loggerHandle,
            frameStack: [[]],
            workStack: [],
            callStack: [{funcName: ".main", pc: 0}],
            height: 100,
            width: 100,
            halted: false
        }
        // initialize program to the program that does nothing and halts immediately.
        this.program = new Map([[".main", [{opcode: PixIROpcode.HALT, operand: undefined}]]])
    }

    /* Pop safely from the work stack. */
    private safePop(): PixIROperand {
        let x = this.state.workStack.pop();
        if (x === undefined)
            throw Error("Empty stack when operand is needed.");
        return x;
    }

    public load(program: Program) {
        this.program = program;
        this.reset();
    }

    public reset() {
        this.state.frameStack = [[]];
        this.state.workStack = [];
        this.state.callStack = [{funcName: ".main", pc: 0}];
        this.state.halted = false;
    }

    public step() {
        if (this.state.halted)
            throw Error("Trying to execute step in halted VM.");

        let {funcName, pc} = this.state.callStack[this.state.callStack.length - 1];
        let func = this.program.get(funcName);
        if (func === undefined)
            throw Error("Function at the top of the stack was not found.");

        let instr = func[pc];
        switch (instr.opcode) {
            // mathematical operations
            case PixIROpcode.ADD: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) + (y.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.SUB: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) - (y.operandValue as number)
                });

                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.MUL: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) * (y.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.DIV: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) / (y.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.INC: {
                let x = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) + 1
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.DEC: {
                let x = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: (x.operandValue as number) - 1
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.OR:
            case PixIROpcode.MAX: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: Math.max(x.operandValue as number, y.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.AND:
            case PixIROpcode.MIN: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: Math.min(x.operandValue as number, y.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            // get random number
            case PixIROpcode.IRND: {
                let x = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: Math.random() * (x.operandValue as number)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            // logical operations
            case PixIROpcode.NOT: {
                let x = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: 1 - ((x.operandValue as number) > 0 ? 1 : 0)
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.LT: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue < y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.LE: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue <= y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.EQ: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [y.operandType]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue == y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.NEQ: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [y.operandType]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue != y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.GT: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue > y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.GE: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
    
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: x.operandValue >= y.operandValue ? 1 : 0
                });
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            // stack and control operations
            case PixIROpcode.DUP: {
                let x = this.safePop();
    
                this.state.workStack.push(x);
                this.state.workStack.push(x);
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.PUSH: {
                this.state.workStack.push(instr.operand as PixIROperand);
                
                // update pc
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.JMP: {
                let x = this.safePop();
    
                checkOperand(x, [PixIROperandType.PCOFFSET]);
                this.state.callStack[this.state.callStack.length - 1].pc = 
                    x.operandValue as number;
                break;
            }
    
            case PixIROpcode.CJMP: {
                let offset = this.safePop();
                let cond = this.safePop();
    
                checkOperand(offset, [PixIROperandType.PCOFFSET]);
                checkOperand(cond, [PixIROperandType.NUMBER]);
    
                // update pc
                if (cond.operandValue as number != 0)
                    this.state.callStack[this.state.callStack.length - 1].pc = 
                        offset.operandValue as number;
                else
                    this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
    
            case PixIROpcode.CJMP2: {
                let offset = this.safePop();
                let cond = this.safePop();
    
                checkOperand(offset, [PixIROperandType.PCOFFSET]);
                checkOperand(cond, [PixIROperandType.NUMBER]);

                // update pc
                if (cond.operandValue as number == 0)
                    this.state.callStack[this.state.callStack.length - 1].pc = 
                        offset.operandValue as number;
                else
                    this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.CALL: {
                let funcName = this.safePop();
                let argCount = this.safePop();
                
                checkOperand(funcName, [PixIROperandType.FUNCTION]);
                checkOperand(argCount, [PixIROperandType.NUMBER]);

                let frame: Frame = []
                for (let i = 0; i < (argCount.operandValue as number); i++)
                    frame.push(this.safePop());
                
                this.state.frameStack.push(frame);
                this.state.callStack.push({
                    funcName: funcName.operandValue as FunctionName, 
                    pc: 0
                });
                break;
            }

            case PixIROpcode.RET: {
                this.state.callStack.pop();
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.HALT: {
                this.state.halted = true;
                break;
            }

            // allocate automatic vars, create stack frames.
            case PixIROpcode.ALLOC: {
                let size = this.safePop();

                checkOperand(size, [PixIROperandType.NUMBER]);

                for (let i = 0; i < (size.operandValue as number); i++)
                    this.state.frameStack[this.state.frameStack.length - 1].push(undefined);
                
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.OFRAME: {
                let size = this.safePop();

                checkOperand(size, [PixIROperandType.NUMBER]);

                let frame: Frame = [];
                for (let i = 0; i < (size.operandValue as number); i++)
                    frame.push(undefined);

                this.state.frameStack.push(frame);
                
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.CFRAME: {
                this.state.frameStack.pop();
                
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
        }
    }

    public run() {
        while (!this.state.halted) {
            this.step();
        }
    }

    public setWidth(width: number) { this.state.width = width; }

    public setHeight(height: number) { this.state.height = height; }
}


function executeInstr(state: PixVMState, instr: PixIRInstruction) {
    
}