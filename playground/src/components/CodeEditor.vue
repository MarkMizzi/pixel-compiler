<script lang="ts">
import { defineComponent, type Component } from 'vue'
import { Codemirror } from 'vue-codemirror'

// eslint-disable-next-line
export default defineComponent({
  components: {
    Codemirror
  },
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
}) as Component
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
