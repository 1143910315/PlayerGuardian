<template>
  <el-drawer v-model="isShow" title="菜单" :direction="directionData" :before-close="handleClose">
    <slot></slot>
  </el-drawer>
</template>

<script lang="ts" setup>
import { ref } from 'vue'
import type { DrawerProps } from 'element-plus'

const { direction = 'ltr' } = defineProps<{ direction?: 'ltr' | 'rtl' | 'ttb' | 'btt' }>()

const isShow = ref(false)
const directionData = ref<DrawerProps['direction']>(direction)
const handleClose = (done: () => void) => {
  done()
}
const show = () => (isShow.value = true)
const hide = () => (isShow.value = false)
defineExpose({
  show,
  hide,
})
</script>
