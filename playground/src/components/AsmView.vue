<script setup lang="ts">
import CodeEditor from './CodeEditor.vue'
</script>

<template>
  <CodeEditor
    ref="astXmlCodeEditor"
    class="border-2 border-slate-700"
    readonly="nocursor"
    mode="null"
  ></CodeEditor>
</template>

<style scoped></style>
