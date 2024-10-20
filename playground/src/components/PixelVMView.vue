<script setup lang="ts">
import { onMounted, useTemplateRef, defineExpose } from 'vue'
import { type Program, Assembler, PixelVM } from 'pixel-vm'
import { useToast } from 'vue-toast-notification'

const $toast = useToast()

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
      <button class="w-32 p-2 rounded bg-teal-700 text-white" @click="run()">Run</button>
    </div>
    <div>
      <canvas ref="pixel-vm-screen" width="800" height="800"> </canvas>
      <textarea ref="pixel-vm-logger" readonly> </textarea>
    </div>
  </div>
</template>
