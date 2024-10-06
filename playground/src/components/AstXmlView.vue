<script setup lang="ts">
import CodeEditor from './CodeEditor.vue'
import { useAstXmlStore } from '@/stores/ast'

const astXmlStore = useAstXmlStore()
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="xml"
    :content="astXmlStore.astXml"
  ></CodeEditor>
</template>

<style scoped></style>
