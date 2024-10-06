import { defineStore } from 'pinia'

export const useAssemblyStore = defineStore('assembly', {
  state: () => ({
    assembly: ''
  }),
  getters: {
    assembly: (state) => state.assembly
  },
  actions: {
    setAssembly(assembly: string) {
      this.assembly = assembly
    }
  }
})

export const useAstXmlStore = defineStore('astXml', {
  state: () => ({
    astXml: ''
  }),
  getters: {
    astXml: (state) => state.astXml
  },
  actions: {
    setAstXml(astXml: string) {
      this.astXml = astXml
    }
  }
})
