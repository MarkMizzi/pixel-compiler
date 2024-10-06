<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
</script>

<template>
  <header
    class="flex flex-wrap md:justify-start md:flex-nowrap sticky top-0 z-50 w-full justify-self-start"
  >
    <nav
      class="relative w-full mx-auto px-4 md:flex md:items-center md:justify-between md:px-6 lg:px-8"
    >
      <div class="flex flex-row">
        <img alt="Vue logo" class="logo" src="@/assets/logo.svg" width="125" height="125" />
      </div>
      <div class="flex flex-row">
        <RouterLink to="/">Home</RouterLink>
        <RouterLink to="/about">About</RouterLink>
      </div>
    </nav>
  </header>

  <RouterView />
</template>
