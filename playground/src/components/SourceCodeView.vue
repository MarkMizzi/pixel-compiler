<script setup lang="ts">
import CodeEditor from './CodeEditor.vue'

function compile() {}
</script>

<template>
  <div class="flex flex-col">
    <div class="flex flex-row justify-start">
      <button class="w-32 p-2 rounded bg-teal-700 text-white" onclick="compile()">Compile</button>
    </div>
    <CodeEditor class="border-2 border-slate-700" :readonly="false" mode="null"></CodeEditor>
  </div>
</template>

<style scoped></style>
