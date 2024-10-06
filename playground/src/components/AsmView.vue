<script lang="ts">
import CodeEditor from './CodeEditor.vue'
import { defineComponent, useTemplateRef } from 'vue'

export default defineComponent({
  components: {
    CodeEditor
  },
  methods: {
    setAssembly(assembly: string) {
      const assemblyEditor = useTemplateRef('assemblyEditor')
      ;(assemblyEditor.value as typeof CodeEditor).setCode(assembly)
    }
  }
})
</script>

<template>
  <CodeEditor
    ref="assemblyEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="null"
  ></CodeEditor>
</template>

<style scoped></style>
