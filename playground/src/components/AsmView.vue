<script lang="ts">
import CodeEditor from './CodeEditor.vue'
import { defineComponent } from 'vue'

export default defineComponent({
  components: {
    CodeEditor
  },
  data() {
    return { content: '' }
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
