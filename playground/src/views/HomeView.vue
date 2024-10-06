<script lang="ts">
import AsmView from '@/components/AsmView.vue'
import AstXmlView from '@/components/AstXmlView.vue'
import SourceCodeView from '@/components/SourceCodeView.vue'
import { defineComponent, useTemplateRef } from 'vue'

export default defineComponent({
  methods: {
    setAstXml(astXml: string) {
      const astXmlView = useTemplateRef('astXmlView')
      ;(astXmlView.value as typeof AstXmlView).setAstXml(astXml)
    },
    setAssembly(assembly: string) {
      const asmView = useTemplateRef('asmView')
      ;(asmView.value as typeof AsmView).setAssembly(assembly)
    }
  }
})
</script>

<template>
  <main>
    <div class="flex flex-col lg:flex-row">
      <SourceCodeView :set-assembly="setAssembly" , :set-ast-xml="setAstXml"></SourceCodeView>
      <AsmView ref="asmView"></AsmView>
      <div class="hidden">
        <AstXmlView ref="astXmlView"></AstXmlView>
      </div>
    </div>
  </main>
</template>
