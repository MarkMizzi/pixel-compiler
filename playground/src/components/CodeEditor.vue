<script lang="ts">
import { defineComponent, type DefineComponent } from 'vue'
import { Codemirror } from 'vue-codemirror'

// eslint-disable-next-line
export default defineComponent({
  components: {
    Codemirror
  },
  props: {
    readonly: { type: Boolean, required: true }
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
    getCode(): string {
      return this.code
    },
    addExtension(extension: any) {
      this.extensions.push(extension)
    }
  }
}) as DefineComponent<
  { readonly: boolean },
  {},
  { code: string; extensions: Array<any> },
  {},
  { addExtension: (code: string) => void; setCode: (code: string) => void }
>
</script>

<template>
  <codemirror
    ref="codemirror"
    v-model="code"
    placeholder=""
    :style="{ height: '400px' }"
    :indent-with-tab="true"
    :tab-size="2"
    :extensions="extensions"
    :disabled="readonly"
  />
</template>
