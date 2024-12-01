<script setup lang="ts">
import { onMounted, useTemplateRef, defineExpose, ref } from 'vue'
import { type Program, Assembler, PixelVM } from 'pixel-vm'
import $toast from '@/toast'

export interface PixelVMViewData {
  program: Program
  assembler: Assembler
  vm: PixelVM | undefined
}

let assembler = new Assembler()
let vm: PixelVM | undefined = undefined

const screenRef = useTemplateRef('pixel-vm-screen')
const loggerRef = useTemplateRef('pixel-vm-logger')

const isRunning = ref(false)
const isPaused = ref(false)

const currScreenWidth = ref(0)
const currScreenHeight = ref(0)

const newScreenWidthInput = ref('')
const newScreenHeightInput = ref('')

onMounted(() => {
  vm = new PixelVM(screenRef.value as HTMLCanvasElement, loggerRef.value as HTMLTextAreaElement)
  currScreenWidth.value = vm!.getWidth()
  currScreenHeight.value = vm!.getHeight()
})

function setNewWidth() {
  const newScreenWidth = parseInt(newScreenWidthInput.value, 10)
  if (isNaN(newScreenWidth)) {
    $toast.error(`${newScreenWidthInput.value} is invalid width: Must enter number.`)
    return
  }

  if (newScreenWidth <= 0) {
    $toast.error('New screen width must be > 0.')
    return
  }

  if (vm) {
    try {
      vm.setWidth(newScreenWidth)
    } catch (error) {
      $toast.error(`${error}`)
    }
    currScreenWidth.value = vm.getWidth()
  }
}

function setNewHeight() {
  const newScreenHeight = parseInt(newScreenHeightInput.value, 10)
  if (isNaN(newScreenHeight)) {
    $toast.error(`${newScreenHeightInput.value} is invalid height: Must enter number.`)
    return
  }

  if (newScreenHeight <= 0) {
    $toast.error('New screen height must be > 0.')
    return
  }

  if (vm) {
    try {
      vm.setHeight(newScreenHeight)
    } catch (error) {
      $toast.error(`${error}`)
    }
    currScreenHeight.value = vm.getHeight()
  }
}

function setProgram(asm: string) {
  vm?.load(assembler.assemble(asm))
}

function stop() {
  vm?.stop()
}

function pause() {
  isPaused.value = true
  vm?.pause()
}

function step() {
  vm?.safeStep().catch((error) => {
    $toast.error(`${error}`)
  })
}

function runOrContinue() {
  if (isRunning.value) {
    $toast.error('VM is already running!')
  }
  isRunning.value = true

  if (isPaused.value) {
    isPaused.value = false
    vm
      ?.continue()
      .catch((error) => {
        $toast.error(`${error}`)
      })
      .finally(() => {
        isRunning.value = false
      })
  } else {
    vm
      ?.run()
      .catch((error) => {
        $toast.error(`${error}`)
      })
      .finally(() => {
        isRunning.value = false
      })
  }
}

defineExpose({
  setProgram
})
</script>

<template>
  <div class="grid grid-cols-1">
    <div class="flex flex-row justify-start p-2 gap-x-2">
      <div class="flex flex-col gap-y-2 md:h-[80vh]">
        <div class="flex flex-col md:flex-row justify-start md:h-12 gap-y-2 md:gap-y-0 md:gap-x-6">
          <p class="text-sm my-auto">
            <span class="italic">width x height: </span>
            {{ currScreenWidth }} x {{ currScreenHeight }}
          </p>
          <form class="flex flex-row justify-start gap-x-2">
            <div class="flex flex-col content-start">
              <label class="text-sm italic">set width:</label>
              <input
                v-model="newScreenWidthInput"
                type="number"
                class="bg-slate-900 text-slate-100 text-sm py-0 my-0 w-24"
              />
            </div>
            <button class="link-green" type="button" @click="setNewWidth()">update</button>
          </form>
          <form class="flex flex-row justify-start gap-x-2">
            <div class="flex flex-col content-start">
              <label class="text-sm italic">set height:</label>
              <input
                v-model="newScreenHeightInput"
                type="number"
                class="bg-slate-900 text-slate-100 text-sm py-0 my-0 w-24"
              />
            </div>
            <button class="link-green" type="button" @click="setNewHeight()">update</button>
          </form>
        </div>
        <canvas
          ref="pixel-vm-screen"
          class="relative w-full overflow-auto border-2 content-center border-slate-900"
          width="800"
          height="800"
        >
        </canvas>
        <textarea
          ref="pixel-vm-logger"
          placeholder="Logs will be printed here..."
          class="w-full h-96 overflow-y-scroll border-0 bg-slate-900 text-slate-100 resize-none"
          readonly
        ></textarea>
      </div>
      <div class="flex flex-col gap-x-0">
        <button v-if="!isRunning" class="h-8 w-8 p-2 link-green" @click="runOrContinue()">
          <span class="material-icons"> play_arrow </span>
        </button>
        <button v-if="!isRunning" class="h-8 w-8 p-2 link-green" @click="step()">
          <span class="material-symbols-outlined"> step </span>
        </button>
        <button v-if="isRunning" class="h-8 w-8 p-2 link-green" @click="pause()">
          <span class="material-icons"> pause </span>
        </button>
        <button v-if="isRunning" class="h-8 w-8 p-2 link-green">
          <span class="material-icons" @click="stop()"> stop </span>
        </button>
      </div>
    </div>
  </div>
</template>
