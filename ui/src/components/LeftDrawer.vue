<template>
  <el-button type="primary" style="margin-left: 16px" @click="drawer = true">
    open
  </el-button>
  <el-button type="primary" style="margin-left: 16px" @click="drawer2 = true">
    with footer
  </el-button>

  <el-drawer v-model="drawer" title="I am the title" :direction="directionData" :before-close="handleClose">
    <slot></slot>
  </el-drawer>
  <el-drawer v-model="drawer2" :direction="directionData">
    <template #header>
      <h4>set title by slot</h4>
    </template>
    <template #default>
      <div>
        <el-radio v-model="radio1" value="Option 1" size="large">
          Option 1
        </el-radio>
        <el-radio v-model="radio1" value="Option 2" size="large">
          Option 2
        </el-radio>
      </div>
    </template>
  </el-drawer>
</template>

<script lang="ts" setup>
import { ref } from 'vue'
import type { DrawerProps } from 'element-plus'

const { direction = 'ltr' } = defineProps<{ direction?: "ltr" | "rtl" | "ttb" | "btt" }>()

const drawer = ref(false)
const drawer2 = ref(false)
const directionData = ref<DrawerProps['direction']>(direction)
const radio1 = ref('Option 1')
const handleClose = (done: () => void) => {
  done()
}
function cancelClick() {
  drawer2.value = false
}
</script>
