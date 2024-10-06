<script setup lang="ts">
import CodeMirror from 'codemirror'
import { useTemplateRef } from 'vue'

const props = defineProps<{
  readonly: boolean
  mode: string
}>()

const editorTextAreaRef = useTemplateRef('editor')
CodeMirror.fromTextArea(editorTextAreaRef.value as HTMLTextAreaElement, {
  value: '',
  mode: props.mode
})
</script>

<template>
  <textarea ref="editor" :readonly="readonly"> </textarea>
</template>

<style scoped></style>
