<script lang="ts">
import { defineComponent, useTemplateRef } from 'vue'
import { type Program, Assembler, PixelVM } from 'pixel-vm'
import { useToast } from 'vue-toast-notification'

const $toast = useToast()

export interface PixelVMViewData {
  program: Program
  assembler: Assembler
  vm: PixelVM | undefined
}

export default defineComponent({
  data() {
    return {
      program: {},
      assembler: new Assembler(),
      vm: undefined
    } as PixelVMViewData
  },
  mounted() {
    let screenRef = useTemplateRef('pixel-vm-screen')
    let loggerRef = useTemplateRef('pixel-vm-logger')

    this.vm = new PixelVM(
      screenRef.value as HTMLCanvasElement,
      loggerRef.value as HTMLTextAreaElement
    )
  },
  methods: {
    setProgram(asm: string) {
      this.program = this.assembler.assemble(asm)
    },
    run() {
      if (this.vm !== undefined) {
        this.vm.run().catch((error) => {
          $toast.error(`${error}`)
        })
      }
    }
  }
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
