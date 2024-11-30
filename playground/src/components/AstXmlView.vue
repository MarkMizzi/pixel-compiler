<script lang="ts">
import CodeEditor from './CodeEditor.vue'
import { defineComponent } from 'vue'
import { xml } from '@codemirror/lang-xml'

export default defineComponent({
  components: {
    CodeEditor
  },
  data() {
    return { content: '' }
  },
  mounted() {
    // add XML extension to the code editor.
    const astXmlCodeEditor = this.$refs.astXmlCodeEditor
    ;(astXmlCodeEditor as typeof CodeEditor).addExtension(xml())
    this.setContent('<!-- Compile a program -->')
  },
  methods: {
    setContent(astXml: string) {
      this.content = astXml
    }
  }
})
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    v-model="content"
    class="border-2 border-slate-700"
    :readonly="true"
  ></CodeEditor>
</template>
