<template>
  <el-menu default-active="1" class="menu">
    <el-menu-item index="1" class="f no-padding">
      <template #title>
        <RouterLink to="/playerTrajectoryMap" class="fl fg routerLink" @click="targetNavigate">
          <el-icon>
            <location />
          </el-icon>
          <div>玩家轨迹地图</div>
        </RouterLink>
      </template>
    </el-menu-item>
    <el-menu-item index="2">
      <el-icon><icon-menu /></el-icon>
      <template #title>Navigator Two</template>
    </el-menu-item>
    <el-menu-item index="3" disabled>
      <el-icon>
        <document />
      </el-icon>
      <template #title>Navigator Three</template>
    </el-menu-item>
    <el-menu-item index="4">
      <el-icon>
        <setting />
      </el-icon>
      <template #title>Navigator Four</template>
    </el-menu-item>
  </el-menu>
</template>

<style>
.menu {
  border: none;
}
.routerLink{
  padding: 0 var(--el-menu-base-level-padding);
}
</style>

<script lang="ts" setup>
import {
  Document,
  Menu as IconMenu,
  Location,
  Setting,
} from '@element-plus/icons-vue'
const emit = defineEmits(['navigete'])
const targetNavigate=() => {
  emit('navigete')
}
</script>

