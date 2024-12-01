<template>
  <main>
    <div class="flex flex-col md:flex-row h-full">
      <div class="w-full">
        <SourceCodeView :compile="compile"></SourceCodeView>
      </div>
      <div class="w-full grid grid-cols-1">
        <div class="flex flex-row h-12 justify-start p-2 gap-x-0">
          <button class="w-32 p-2 bg-slate-900 link-green" @click="selected = 'assembly'">
            assembly
          </button>
          <button class="w-32 p-2 bg-slate-900 link-green" @click="selected = 'astxml'">
            ast xml
          </button>
          <button class="w-32 p-2 bg-slate-900 link-green" @click="selected = 'run'">
            execute
          </button>
        </div>
        <AsmView v-show="selected == 'assembly'" ref="asmView"></AsmView>
        <AstXmlView v-show="selected == 'astxml'" ref="astXmlView"></AstXmlView>
        <PixelVMView v-show="selected == 'run'" ref="pixelVMView"></PixelVMView>
      </div>
    </div>
  </main>
</template>

<script lang="ts">
import AsmView from '@/components/AsmView.vue'
import AstXmlView from '@/components/AstXmlView.vue'
import PixelVMView from '@/components/PixelVMView.vue'
import SourceCodeView from '@/components/SourceCodeView.vue'
import { StatusCodes } from 'http-status-codes'
import { defineComponent } from 'vue'
import $toast from '@/toast'

interface CompilerOutput {
  xmlOutput: string
  asmOutput: string
  compilerStdOut: string
  compilerStdErr: string
}

export default defineComponent({
  components: {
    SourceCodeView,
    AstXmlView,
    AsmView,
    PixelVMView
  },
  data() {
    return { selected: 'assembly' }
  },
  methods: {
    compile(srcCode: string) {
      fetch('/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          srcCode: srcCode
        })
      })
        .then(async (response) => {
          // check error status
          if (response.status == StatusCodes.SERVICE_UNAVAILABLE) {
            // this error comes from NGINX, give our own error message
            throw Error('Rate limit exceeded. Try again later.')
          } else if (response.status != 200) {
            throw Error(`${response.status} ${await response.text()}`)
          } else {
            return response.json()
          }
        })
        .then((data) => {
          const astXmlView = this.$refs.astXmlView as typeof AstXmlView
          const asmView = this.$refs.asmView as typeof AsmView
          const pixelVMView = this.$refs.pixelVMView as typeof PixelVMView

          // parse body
          let output = data as CompilerOutput
          // check for compiler error
          if (output.asmOutput == '') {
            astXmlView.setContent(
              '<!--\n' + output.compilerStdErr + output.compilerStdOut + '\n-->'
            )
            asmView.setContent(
              ('# ' + output.compilerStdErr + output.compilerStdOut).replace('\n', '\n# ')
            )
            // an error occurred, indicate to user and set asm and ast views to combined stderr and stdout streams.
            throw Error('Compilation error occurred. Check your source program.')
          } else {
            // set content of various views accordingly
            astXmlView.setContent(output.xmlOutput)
            asmView.setContent(output.asmOutput)
            pixelVMView.setProgram(output.asmOutput)
          }
        })
        .then(() => {
          $toast.success('Compiled and loaded program.')
        })
        .catch((error) => {
          $toast.error(`${error}`)
        })
    }
  }
})
</script>
