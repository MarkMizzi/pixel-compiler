<script setup lang="ts">
import { computed } from 'vue'
import { useAssemblyStore } from '@/stores/compilerOutput'
import CodeEditor from './CodeEditor.vue'

const asmStore = useAssemblyStore()
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="null"
    :content="computed(() => asmStore.assembly)"
  ></CodeEditor>
</template>

<style scoped></style>
