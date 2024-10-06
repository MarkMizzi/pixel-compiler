<script lang="ts">
import AsmView from '@/components/AsmView.vue'
import AstXmlView from '@/components/AstXmlView.vue'
import SourceCodeView from '@/components/SourceCodeView.vue'
import { defineComponent } from 'vue'

export default defineComponent({
  components: {
    SourceCodeView,
    AstXmlView,
    AsmView
  },
  data() {
    return { selected: 'assembly' }
  },
  methods: {
    setAstXml(astXml: string) {
      const astXmlView = this.$refs.astXmlView
      ;(astXmlView as typeof AstXmlView).setAstXml(astXml)
    },
    setAssembly(assembly: string) {
      const asmView = this.$refs.asmView
      ;(asmView as typeof AsmView).setAssembly(assembly)
    }
  }
})
</script>

<template>
  <main>
    <div class="flex flex-flow-row">
      <div class="w-full">
        <SourceCodeView :set-assembly="setAssembly" :set-ast-xml="setAstXml"></SourceCodeView>
      </div>
      <div class="w-full grid grid-cols-1">
        <div class="flex flex-row justify-start p-2 gap-x-4">
          <button class="w-32 p-2 rounded bg-teal-700 text-white" @click="selected = 'assembly'">
            View Assembly
          </button>
          <button class="w-32 p-2 rounded bg-teal-700 text-white" @click="selected = 'astxml'">
            View AST XML
          </button>
        </div>
        <AsmView v-show="selected == 'assembly'" ref="asmView"></AsmView>
        <AstXmlView v-show="selected == 'astxml'" ref="astXmlView"></AstXmlView>
      </div>
    </div>
  </main>
</template>
