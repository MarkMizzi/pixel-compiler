<script lang="ts">
import { defineComponent } from 'vue'

export default defineComponent({
  props: {
    readonly: { type: [String, Boolean], required: true }
  },
  data() {
    return {
      code: '',
      extensions: []
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
  />
</template>

<style scoped></style>
