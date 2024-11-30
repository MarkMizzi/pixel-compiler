<script lang="ts">
import { pixelAsm } from '@/pixel-extensions'
import CodeEditor from './CodeEditor.vue'
import { defineComponent } from 'vue'

export default defineComponent({
  components: {
    CodeEditor
  },
  data() {
    return { content: '' }
  },
  mounted() {
    // add pixel ASM extension to the code editor.
    const assemblyEditor = this.$refs.assemblyEditor
    ;(assemblyEditor as typeof CodeEditor).addExtension(pixelAsm())
    this.setContent('# Compile a program')
  },
  methods: {
    setContent(assembly: string) {
      this.content = assembly
    }
  }
})
</script>

<template>
  <CodeEditor
    ref="assemblyEditor"
    v-model="content"
    class="border-2 border-slate-700"
    :readonly="true"
    mode="null"
  ></CodeEditor>
</template>
