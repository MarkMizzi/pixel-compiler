import { Color, FunctionName, PixIROperand, PixIROpcode, PixIRInstruction, PixIROperandType, checkOperand, rgbToHex } from "./instructions"
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

    private fillRect(x: number, y: number, w: number, h: number, c: Color) {
        if (x < 0 || 
            y < 0 || 
            x + w >= this.state.width || 
            y + h >= this.state.height)
            throw Error(`Out of bounds fill <${x}, ${y}, ${w}, ${h}> requested.`);

        // scale given variables to the actual Javascript canvas
        const canvasX = x * this.state.screenHandle.width / this.state.width;
        const canvasY = y * this.state.screenHandle.height / this.state.height;
        const canvasW = w * this.state.screenHandle.width / this.state.width;
        const canvasH = h * this.state.screenHandle.height / this.state.height;

        // fill in the rectangle.
        let context = this.state.screenHandle.getContext("2d");
        context!.fillStyle = c;
        context!.fillRect(canvasX, canvasY, canvasW, canvasH);
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

    public async step() {
        if (this.state.halted)
            throw Error("Trying to execute step in halted VM.");

        const {funcName, pc} = this.state.callStack[this.state.callStack.length - 1];
        const func = this.program.get(funcName);
        if (func === undefined)
            throw Error("Function at the top of the stack was not found.");

        const instr = func[pc];
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

            case PixIROpcode.ST: {
                let location = this.safePop();
                let frame = this.safePop();
                let val = this.safePop();

                checkOperand(location, [PixIROperandType.NUMBER]);
                checkOperand(frame, [PixIROperandType.NUMBER]);
    
                this.state.frameStack[frame.operandValue as number][location.operandValue as number] =
                    val;

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            // delay operation
            case PixIROpcode.DELAY: {
                // https://builtin.com/software-engineering-perspectives/javascript-sleep
                // sleep in Javascript; delay given in milliseconds
                const sleep = (delay: number) => new Promise((resolve) => setTimeout(resolve, delay));
                
                let delay = this.safePop();

                checkOperand(delay, [PixIROperandType.NUMBER]);

                await sleep(delay.operandValue as number);
                
                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            // screen related operations
            case PixIROpcode.PIXEL: {
                let x = this.safePop();
                let y = this.safePop();
                let c = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
                checkOperand(c, [PixIROperandType.COLOR]);

                this.fillRect(
                    x.operandValue as number, 
                    y.operandValue as number, 
                    1, 
                    1, 
                    c.operandValue as Color
                );

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.PIXELR: {
                let x = this.safePop();
                let y = this.safePop();
                let w = this.safePop();
                let h = this.safePop();
                let c = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);
                checkOperand(w, [PixIROperandType.NUMBER]);
                checkOperand(h, [PixIROperandType.NUMBER]);
                checkOperand(c, [PixIROperandType.COLOR]);

                this.fillRect(
                    x.operandValue as number,
                    y.operandValue as number,
                    w.operandValue as number,
                    h.operandValue as number,
                    c.operandValue as Color
                );

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.CLEAR: {
                let c = this.safePop();

                checkOperand(c, [PixIROperandType.COLOR]);

                this.fillRect(
                    0, 
                    0, 
                    this.state.width, 
                    this.state.height, 
                    c.operandValue as Color
                );

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.READ: {
                let x = this.safePop();
                let y = this.safePop();
    
                checkOperand(x, [PixIROperandType.NUMBER]);
                checkOperand(y, [PixIROperandType.NUMBER]);

                const canvasX = x.operandValue as number * this.state.screenHandle.width / this.state.width;
                const canvasY = y.operandValue as number * this.state.screenHandle.height / this.state.height;

                const context = this.state.screenHandle.getContext("2d");
                const imageData = context!.getImageData(canvasX, canvasY, 1, 1).data;
                this.state.workStack.push({
                    operandType: PixIROperandType.COLOR,
                    operandValue: rgbToHex(imageData[0], imageData[1], imageData[2])
                })

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.WIDTH: {
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: this.state.width
                });

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.HEIGHT: {
                this.state.workStack.push({
                    operandType: PixIROperandType.NUMBER, 
                    operandValue: this.state.height
                });

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            // array operations
            case PixIROpcode.ALLOCA: {
                let size = this.safePop();
    
                checkOperand(size, [PixIROperandType.NUMBER]);

                let arr = [];
                for (let i = 0; i < (size.operandValue as number); i++)
                    arr.push(undefined);

                this.state.workStack.push({
                    operandType: PixIROperandType.ARRAY,
                    operandValue: arr,
                })

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.STA: {
                let arr = this.safePop();
                const idx = this.safePop();
                const val = this.safePop();
    
                checkOperand(arr, [PixIROperandType.ARRAY]);
                checkOperand(idx, [PixIROperandType.NUMBER]);

                (arr.operandValue as Array<any>)[idx.operandValue as number] =
                    val;

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }

            case PixIROpcode.LDA: {
                let arr = this.safePop();
                const idx = this.safePop();
    
                checkOperand(arr, [PixIROperandType.ARRAY]);
                checkOperand(idx, [PixIROperandType.NUMBER]);

                const val = (arr.operandValue as Array<any>)[idx.operandValue as number];
                if (val === undefined)
                    throw Error("Loaded undefined element from array.");

                this.state.workStack.push(val);

                this.state.callStack[this.state.callStack.length - 1].pc++;
                break;
            }
        }
    }

    public async run() {
        while (!this.state.halted) {
            await this.step();
        }
    }

    public setWidth(width: number) { this.state.width = width; }

    public setHeight(height: number) { this.state.height = height; }
}


function executeInstr(state: PixVMState, instr: PixIRInstruction) {
    
}