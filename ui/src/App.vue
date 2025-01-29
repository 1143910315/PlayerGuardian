<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import NavigateMenu from './components/NavigateMenu.vue'
</script>

<template>
  <el-config-provider :locale="zhCn">
    <header>

      <div class="wrapper">
        <NavigateMenu />

        <nav>
          <RouterLink to="/">Home</RouterLink>
          <RouterLink to="/interaction">交互</RouterLink>
          <RouterLink to="/about">About</RouterLink>
        </nav>
      </div>
    </header>

    <RouterView />
  </el-config-provider>
</template>

<style scoped>
</style>
