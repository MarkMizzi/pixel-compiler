<script lang="ts">
import { defineComponent } from 'vue'

export default defineComponent({
  props: {
    readonly: { type: [String, Boolean], required: true }
  },
  data() {
    return {
      code: '',
      extensions: new Array<any>()
    }
  },
  methods: {
    setCode(code: string) {
      this.code = code
    },
    addExtension(extension: any) {
      this.extensions.push(extension)
    }
  }
})
</script>

<template>
  <codemirror
    v-model="code"
    placeholder=""
    :style="{ height: '400px' }"
    :indent-with-tab="true"
    :tab-size="2"
    :extensions="extensions"
    :readonly="readonly"
  />
</template>

<style scoped></style>
