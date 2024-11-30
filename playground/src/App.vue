<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
</script>

<template>
  <header
    class="flex flex-wrap md:justify-start md:flex-nowrap sticky top-0 z-50 w-full justify-self-start"
  >
    <nav
      class="absolute z-20 top-0 left-0 w-screen bg-slate-900 px-4 py-2 flex flex-row justify-end gap-x-4"
    >
      <RouterLink class="link-green" to="/">home</RouterLink>
      <RouterLink class="link-green" to="/about">about</RouterLink>
    </nav>
  </header>

  <RouterView class="mt-16" />
</template>
