import { ref } from 'vue'
import { defineStore } from 'pinia'

export const useAstXmlStore = defineStore('astXml', () => {
  const astXml = ref('')
  function setAstXml(value: string) {
    astXml.value = value
  }

  return { astXml, setAstXml }
})
