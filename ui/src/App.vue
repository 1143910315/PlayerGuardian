<template>
  <el-config-provider :locale="zhCn">
    <header>

      <div class="wrapper">
        <LeftDrawer ref="leftDrawer">
          <NavigateMenu @navigete="hideMenu" />
        </LeftDrawer>
        <div class="fl">
          <button aria-label="菜单" @click="showMenu" class="f">
            <i class="segoe-icon" aria-hidden="true"></i>
            <div class="right-padding"></div>
            <el-text size="large">{{ $router.currentRoute.value.meta.pageName }}</el-text>
          </button>
        </div>
      </div>
    </header>

    <RouterView />
  </el-config-provider>
</template>

<style scoped>
.segoe-icon {
  font-family: 'Segoe MDL2 Assets';
  --el-text-font-size: var(--el-font-size-medium);
  font-size: var(--el-text-font-size);
  align-self: center;
  user-select: none;
  font-style: normal;
}
</style>

<script setup lang="ts">
import { RouterView } from 'vue-router'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import NavigateMenu from './components/NavigateMenu.vue'
import LeftDrawer from './components/LeftDrawer.vue'
import { useTemplateRef } from 'vue'

const leftDrawer = useTemplateRef('leftDrawer')
const showMenu = () => {
  leftDrawer.value?.show()
}
const hideMenu = () => {
  leftDrawer.value?.hide()
}
</script>
