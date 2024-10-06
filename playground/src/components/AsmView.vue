<script lang="ts">
import CodeEditor from './CodeEditor.vue'
import { defineComponent } from 'vue'

export default defineComponent({
  components: {
    CodeEditor
  },
  methods: {
    setAssembly(assembly: string) {
      const assemblyCodeEditor = this.$refs.assemblyEditor
      ;(assemblyCodeEditor as typeof CodeEditor).setCode(assembly)
    }
  }
})
</script>

<template>
  <CodeEditor
    ref="assemblyEditor"
    class="border-2 border-slate-700"
    :readonly="true"
    mode="null"
  ></CodeEditor>
</template>

<style scoped></style>
