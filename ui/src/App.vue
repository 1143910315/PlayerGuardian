<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import NavigateMenu from './components/NavigateMenu.vue'
import LeftDrawer from './components/LeftDrawer.vue'
import { useTemplateRef } from 'vue'

const leftDrawer = useTemplateRef('leftDrawer')
const showMenu = () => {
  leftDrawer.value?.show()
}
</script>

<template>
  <el-config-provider :locale="zhCn">
    <header>

      <div class="wrapper">
        <LeftDrawer ref="leftDrawer">
          <NavigateMenu />
        </LeftDrawer>
        <div class="fl">
          <button aria-label="菜单" @click="showMenu">
            <i class="segoe-icon" aria-hidden="true"></i>
          </button>
        </div>
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
.segoe-icon {
  font-family: 'Segoe MDL2 Assets';
  font-size: 31px;
  user-select: none;
  font-style: normal;
}
</style>
