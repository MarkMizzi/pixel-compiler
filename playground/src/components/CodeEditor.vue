<script setup lang="ts">
import CodeMirror from 'codemirror'
import { useTemplateRef, ref, type Ref } from 'vue'

const { content = ref(''), ...props } = defineProps<{
  readonly: boolean | 'nocursor'
  mode: string
  content?: Ref<string>
}>()

const editorTextAreaRef = useTemplateRef('editor')
CodeMirror.fromTextArea(editorTextAreaRef.value as HTMLTextAreaElement, {
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
  <textarea ref="editor" :value="content"></textarea>
</template>

<style scoped></style>
