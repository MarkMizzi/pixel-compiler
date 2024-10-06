import { ref } from 'vue'
import { defineStore } from 'pinia'

export const useAssemblyStore = defineStore('asm', () => {
  const asm = ref('')
  function setAssembly(value: string) {
    asm.value = value
  }

  return { asm, setAssembly }
})
