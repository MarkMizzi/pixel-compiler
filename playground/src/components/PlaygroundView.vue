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
        <div v-show="selected == 'assembly'" class="flex flex-row">
          <div class="w-full">
            <AsmView ref="asmView"></AsmView>
          </div>
          <div class="flex flex-col p-2 gap-x-0">
            <button class="h-8 w-8 p-2 link-green tooltip" @click="assemble()">
              <div class="tooltip-text">Assemble</div>
              <link
                rel="stylesheet"
                href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=build"
              />
              <span class="material-symbols-outlined"> build </span>
            </button>
          </div>
        </div>
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
    compile(srcCode: string, compilerOpts?: string) {
      fetch('/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          srcCode,
          compilerOpts
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
    },
    // assemble code contained in the Assembly view at the behest of the user.
    // DO NOT use this function for compiled Pixel asm code
    assemble() {
      const astXmlView = this.$refs.astXmlView as typeof AstXmlView
      const asmView = this.$refs.asmView as typeof AsmView
      const pixelVMView = this.$refs.pixelVMView as typeof PixelVMView

      try {
        // load the assembly code in the assembly view into the VM
        const content = asmView.getContent()
        pixelVMView.setProgram(content)
        $toast.success('Assembled and loaded program.')

        // since Pixel asm code loaded in VM is now written and assembled by user, AST does not match it.
        // AST is for Pixel source code not for Pixel ASM.
        astXmlView.setContent('<!-- Compile a program -->')
      } catch (error) {
        $toast.error(`${error}`)
      }
    }
  }
})
</script>
