<script setup lang="ts">
import CodeMirror from 'codemirror'
import { useTemplateRef } from 'vue'

const props = defineProps<{
  readonly: boolean | 'nocursor'
  mode: string
}>()

const editorTextAreaRef = useTemplateRef('editor')
CodeMirror.fromTextArea(editorTextAreaRef.value as HTMLTextAreaElement, {
  value: '',
  mode: props.mode,
  readOnly: props.readonly
})

const getContent = (): string => {
  let textArea = editorTextAreaRef.value as HTMLTextAreaElement
  return textArea.value
}
// make the getContent method public
defineExpose({ getContent })
</script>

<template>
  <textarea ref="editor"></textarea>
</template>

<style scoped></style>
