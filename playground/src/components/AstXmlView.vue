<script lang="ts">
import CodeEditor from './CodeEditor.vue'
import { defineComponent, useTemplateRef } from 'vue'
import { xml } from '@codemirror/lang-xml'

export default defineComponent({
  mounted() {
    // add XML extension to the code editor.
    const astXmlCodeEditor = useTemplateRef('astXmlCodeEditor')
    ;(astXmlCodeEditor.value as typeof CodeEditor).addExtension(xml)
  },
  methods: {
    setAstXml(astXml: string) {
      const astXmlCodeEditor = useTemplateRef('astXmlCodeEditor')
      ;(astXmlCodeEditor.value as typeof CodeEditor).setCode(astXml)
    }
  }
})
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="xml"
  ></CodeEditor>
</template>

<style scoped></style>
