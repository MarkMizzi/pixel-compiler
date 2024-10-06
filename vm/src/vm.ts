import {
  Color,
  FunctionName,
  PixIRData,
  PixIROpcode,
  PixIRInstruction,
  PixIRDataType,
  checkDataType,
  rgbToHex
} from './instructions'
import { Program } from './assembler'

type Frame = Array<PixIRData | undefined>

interface CallFrame {
  funcName: FunctionName
  pc: number
}

interface PixVMState {
  screenHandle: HTMLCanvasElement
  loggerHandle: HTMLTextAreaElement
  frameStack: Array<Frame>
  workStack: Array<PixIRData>
  // track function calls and return pointers.
  callStack: Array<CallFrame>
  height: number
  width: number
  halted: boolean
}

export class PixelVM {
  private state: PixVMState
  private program: Program

  constructor(screenHandle: HTMLCanvasElement, loggerHandle: HTMLTextAreaElement) {
    this.state = {
      screenHandle,
      loggerHandle,
      frameStack: [[]],
      workStack: [],
      callStack: [{ funcName: '.main', pc: 0 }],
      height: 100,
      width: 100,
      halted: false
    }
    // initialize program to the program that does nothing and halts immediately.
    this.program = new Map([['.main', [{ opcode: PixIROpcode.HALT, operand: undefined }]]])
  }

  /* Pop safely from the work stack. */
  private safePop(): PixIRData {
    let x = this.state.workStack.pop()
    if (x === undefined) throw ReferenceError('Empty stack when operand is needed.')
    return x
  }

  private fillRect(x: number, y: number, w: number, h: number, c: Color) {
    if (x < 0 || y < 0 || x + w >= this.state.width || y + h >= this.state.height)
      throw RangeError(`Out of bounds fill <${x}, ${y}, ${w}, ${h}> requested.`)

    // scale given variables to the actual Javascript canvas
    const canvasX = (x * this.state.screenHandle.width) / this.state.width
    const canvasY = (y * this.state.screenHandle.height) / this.state.height
    const canvasW = (w * this.state.screenHandle.width) / this.state.width
    const canvasH = (h * this.state.screenHandle.height) / this.state.height

    // fill in the rectangle.
    let context = this.state.screenHandle.getContext('2d')
    context!.fillStyle = c
    context!.fillRect(canvasX, canvasY, canvasW, canvasH)
  }

  public load(program: Program) {
    this.program = program
    this.reset()
  }

  public reset() {
    this.state.frameStack = [[]]
    this.state.workStack = []
    this.state.callStack = [{ funcName: '.main', pc: 0 }]
    this.state.halted = false
  }

  public async step() {
    if (this.state.halted) throw Error('Trying to execute step in halted VM.')

    const { funcName, pc } = this.state.callStack[this.state.callStack.length - 1]
    const func = this.program.get(funcName)
    if (func === undefined) throw ReferenceError('Function at the top of the stack was not found.')

    const instr = func[pc]
    switch (instr.opcode) {
      // mathematical operations
      case PixIROpcode.ADD: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) + (y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.SUB: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) - (y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.MUL: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])
        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) * (y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.DIV: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) / (y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.INC: {
        const x = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) + 1
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.DEC: {
        const x = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: (x.val as number) - 1
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.OR:
      case PixIROpcode.MAX: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: Math.max(x.val as number, y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.AND:
      case PixIROpcode.MIN: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: Math.min(x.val as number, y.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // get random number
      case PixIROpcode.IRND: {
        const x = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: Math.random() * (x.val as number)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // logical operations
      case PixIROpcode.NOT: {
        const x = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: 1 - ((x.val as number) > 0 ? 1 : 0)
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.LT: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val < y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.LE: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val <= y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.EQ: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [y.dtype])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val == y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.NEQ: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [y.dtype])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val != y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.GT: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val > y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.GE: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: x.val >= y.val ? 1 : 0
        })

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // stack and control operations
      case PixIROpcode.DUP: {
        const x = this.safePop()

        this.state.workStack.push(x)
        this.state.workStack.push(x)

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.PUSH: {
        this.state.workStack.push(instr.operand as PixIRData)

        // update pc
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.JMP: {
        const x = this.safePop()

        checkDataType(x, [PixIRDataType.PCOFFSET])
        this.state.callStack[this.state.callStack.length - 1].pc = x.val as number
        break
      }

      case PixIROpcode.CJMP: {
        const offset = this.safePop()
        const cond = this.safePop()

        checkDataType(offset, [PixIRDataType.PCOFFSET])
        checkDataType(cond, [PixIRDataType.NUMBER])

        // update pc
        if ((cond.val as number) != 0)
          this.state.callStack[this.state.callStack.length - 1].pc = offset.val as number
        else this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.CJMP2: {
        const offset = this.safePop()
        const cond = this.safePop()

        checkDataType(offset, [PixIRDataType.PCOFFSET])
        checkDataType(cond, [PixIRDataType.NUMBER])

        // update pc
        if ((cond.val as number) == 0)
          this.state.callStack[this.state.callStack.length - 1].pc = offset.val as number
        else this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.CALL: {
        const funcName = this.safePop()
        const argCount = this.safePop()

        checkDataType(funcName, [PixIRDataType.FUNCTION])
        checkDataType(argCount, [PixIRDataType.NUMBER])

        let frame: Frame = []
        for (let i = 0; i < (argCount.val as number); i++) frame.push(this.safePop())

        this.state.frameStack.push(frame)
        this.state.callStack.push({
          funcName: funcName.val as FunctionName,
          pc: 0
        })
        break
      }

      case PixIROpcode.RET: {
        this.state.callStack.pop()
        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.HALT: {
        this.state.halted = true
        break
      }

      // allocate automatic vars, create stack frames.
      case PixIROpcode.ALLOC: {
        const size = this.safePop()

        checkDataType(size, [PixIRDataType.NUMBER])

        for (let i = 0; i < (size.val as number); i++)
          this.state.frameStack[this.state.frameStack.length - 1].push(undefined)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.OFRAME: {
        const size = this.safePop()

        checkDataType(size, [PixIRDataType.NUMBER])

        let frame: Frame = []
        for (let i = 0; i < (size.val as number); i++) frame.push(undefined)

        this.state.frameStack.push(frame)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.CFRAME: {
        this.state.frameStack.pop()

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.ST: {
        const location = this.safePop()
        const frame = this.safePop()
        const val = this.safePop()

        checkDataType(location, [PixIRDataType.NUMBER])
        checkDataType(frame, [PixIRDataType.NUMBER])

        this.state.frameStack[frame.val as number][location.val as number] = val

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // delay operation
      case PixIROpcode.DELAY: {
        // https://builtin.com/software-engineering-perspectives/javascript-sleep
        // sleep in Javascript; delay given in milliseconds
        const sleep = (delay: number) => new Promise((resolve) => setTimeout(resolve, delay))

        const delay = this.safePop()

        checkDataType(delay, [PixIRDataType.NUMBER])

        await sleep(delay.val as number)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // screen related operations
      case PixIROpcode.PIXEL: {
        const x = this.safePop()
        const y = this.safePop()
        const c = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])
        checkDataType(c, [PixIRDataType.COLOR])

        this.fillRect(x.val as number, y.val as number, 1, 1, c.val as Color)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.PIXELR: {
        const x = this.safePop()
        const y = this.safePop()
        const w = this.safePop()
        const h = this.safePop()
        const c = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])
        checkDataType(w, [PixIRDataType.NUMBER])
        checkDataType(h, [PixIRDataType.NUMBER])
        checkDataType(c, [PixIRDataType.COLOR])

        this.fillRect(
          x.val as number,
          y.val as number,
          w.val as number,
          h.val as number,
          c.val as Color
        )

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.CLEAR: {
        const c = this.safePop()

        checkDataType(c, [PixIRDataType.COLOR])

        this.fillRect(0, 0, this.state.width, this.state.height, c.val as Color)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.READ: {
        const x = this.safePop()
        const y = this.safePop()

        checkDataType(x, [PixIRDataType.NUMBER])
        checkDataType(y, [PixIRDataType.NUMBER])

        const canvasX = ((x.val as number) * this.state.screenHandle.width) / this.state.width
        const canvasY = ((y.val as number) * this.state.screenHandle.height) / this.state.height

        const context = this.state.screenHandle.getContext('2d')
        const imageData = context!.getImageData(canvasX, canvasY, 1, 1).data
        this.state.workStack.push({
          dtype: PixIRDataType.COLOR,
          val: rgbToHex(imageData[0], imageData[1], imageData[2])
        })

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.WIDTH: {
        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: this.state.width
        })

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.HEIGHT: {
        this.state.workStack.push({
          dtype: PixIRDataType.NUMBER,
          val: this.state.height
        })

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      // array operations
      case PixIROpcode.ALLOCA: {
        const size = this.safePop()

        checkDataType(size, [PixIRDataType.NUMBER])

        let arr = []
        for (let i = 0; i < (size.val as number); i++) arr.push(undefined)

        this.state.workStack.push({
          dtype: PixIRDataType.ARRAY,
          val: arr
        })

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.STA: {
        let arr = this.safePop()
        const idx = this.safePop()
        const val = this.safePop()

        checkDataType(arr, [PixIRDataType.ARRAY])
        checkDataType(idx, [PixIRDataType.NUMBER])
        ;(arr.val as Array<any>)[idx.val as number] = val

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }

      case PixIROpcode.LDA: {
        let arr = this.safePop()
        const idx = this.safePop()

        checkDataType(arr, [PixIRDataType.ARRAY])
        checkDataType(idx, [PixIRDataType.NUMBER])

        const val = (arr.val as Array<any>)[idx.val as number]
        if (val === undefined) throw ReferenceError('Loaded undefined element from array.')

        this.state.workStack.push(val)

        this.state.callStack[this.state.callStack.length - 1].pc++
        break
      }
    }
  }

  public async run() {
    while (!this.state.halted) {
      await this.step()
    }
  }

  public setWidth(width: number) {
    if (this.state.screenHandle.width < width)
      throw RangeError(
        `Cannot set width of Pixel VM screen larger than width of canvas ${this.state.screenHandle.width}`
      )
    this.state.width = width
  }

  public setHeight(height: number) {
    if (this.state.screenHandle.height < height)
      throw RangeError(
        `Cannot set height of Pixel VM screen larger than height of canvas ${this.state.screenHandle.width}`
      )
    this.state.height = height
  }
}
