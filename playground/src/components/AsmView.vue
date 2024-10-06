<script setup lang="ts">
import CodeEditor from './CodeEditor.vue'
import { useAssemblyStore } from '@/stores/asm'

const asmStore = useAssemblyStore()
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="null"
    :content="asmStore.asm"
  ></CodeEditor>
</template>

<style scoped></style>
