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
    <div class="flex flex-col md:flex-row">
      <SourceCodeView :set-assembly="setAssembly" :set-ast-xml="setAstXml"></SourceCodeView>
      <AsmView ref="asmView"></AsmView>
      <div class="hidden">
        <AstXmlView ref="astXmlView"></AstXmlView>
      </div>
    </div>
  </main>
</template>
