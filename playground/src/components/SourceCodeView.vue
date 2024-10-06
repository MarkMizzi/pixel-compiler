<script setup lang="ts">
import { StatusCodes } from 'http-status-codes'
import CodeEditor from './CodeEditor.vue'
import { ref, type Component, type Ref } from 'vue'
import { useToast } from 'vue-toast-notification'

const $toast = useToast()

const props = defineProps<{
  setAstXml: (s: string) => void
  setAssembly: (s: string) => void
}>()

// once the component is mounted this will automatically be set to
// point to child component with ref="srcCodeEditor"
const srcCodeEditor: Ref<Component | null> = ref(null)

interface CompilerOutput {
  xmlOutput: string
  asmOutput: string
  compilerStdOut: string
  compilerStdErr: string
}

const compile = () => {
  fetch('/compile', {
    body: JSON.stringify({
      srcCode: (srcCodeEditor.value as typeof CodeEditor).getContent()
    })
  })
    .then(async (response) => {
      // check error status
      if (response.status == StatusCodes.SERVICE_UNAVAILABLE) {
        // this error comes from NGINX, give our own error message
        $toast.error('Rate limit exceeded. Try again later.')
      } else if (response.status != 200) {
        $toast.error(`${response.status} Error: ${response.body}`)
      } else {
        // parse body
        let output = (await response.json()) as CompilerOutput
        // check for compiler error
        if (output.asmOutput == '') {
          // an error occurred, indicate to user and set asm and ast views to combined stderr and stdout streams.
          $toast.error('Compilation error occurred. Check your source program.')
          props.setAssembly(output.compilerStdErr + output.compilerStdOut)
          props.setAstXml(output.compilerStdErr + output.compilerStdOut)
        } else {
          // set store variables accordingly
          props.setAssembly(output.asmOutput)
          props.setAstXml(output.xmlOutput)
        }
      }
    })
    .catch((error) => {
      $toast.error(`Error occurred while attempting connection to compilation server: ${error}`)
    })
}
</script>

<template>
  <div class="flex flex-col">
    <div class="flex flex-row justify-start">
      <button class="w-32 p-2 rounded bg-teal-700 text-white" @click="compile()">Compile</button>
    </div>
    <CodeEditor
      ref="srcCodeEditor"
      class="border-2 border-slate-700"
      :readonly="false"
      mode="null"
    ></CodeEditor>
  </div>
</template>

<style scoped></style>
