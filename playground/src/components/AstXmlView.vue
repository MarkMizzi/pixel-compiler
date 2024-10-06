<script setup lang="ts">
import { computed } from 'vue'
import { useAstXmlStore } from '@/stores/compilerOutput'
import CodeEditor from './CodeEditor.vue'

const astXmlStore = useAstXmlStore()
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="xml"
    :content="computed(() => astXmlStore.astXml)"
  ></CodeEditor>
</template>

<style scoped></style>
