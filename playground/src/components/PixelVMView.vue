<script setup lang="ts">
import { onMounted, useTemplateRef, defineExpose } from 'vue'
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

onMounted(() => {
  vm = new PixelVM(screenRef.value as HTMLCanvasElement, loggerRef.value as HTMLTextAreaElement)
})

function setProgram(asm: string) {
  vm?.load(assembler.assemble(asm))
}

function stop() {
  vm?.stop()
}

function run() {
  vm?.run().catch((error) => {
    $toast.error(`${error}`)
  })
}

defineExpose({
  setProgram
})
</script>

<template>
  <div class="grid grid-cols-1">
    <div class="flex flex-row justify-start p-2 gap-x-2">
      <div class="flex flex-col gap-y-2 md:h-[80vh]">
        <canvas
          ref="pixel-vm-screen"
          class="relative w-full overflow-auto border-2 content-center border-slate-900"
          width="800"
          height="800"
        >
        </canvas>
        <textarea
          ref="pixel-vm-logger"
          placeholder="Logs go here"
          class="w-full h-96 overflow-y-scroll border-0 bg-slate-900 text-slate-100"
          readonly
        >
        </textarea>
      </div>
      <div class="flex flex-col gap-x-0">
        <button class="h-8 w-8 p-2 link-green" @click="run()">
          <span class="material-icons"> play_arrow </span>
        </button>
        <button class="h-8 w-8 p-2 link-green">
          <span class="material-icons" @click="stop()"> stop </span>
        </button>
      </div>
    </div>
  </div>
</template>
