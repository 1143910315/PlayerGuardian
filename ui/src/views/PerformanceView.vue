<template>
  <div class="fcl fg">
    <div class="s1"></div>
    <div class="fl">
      <el-text size="large">服务器总体状态：</el-text><el-text size="large" :type="serverStatus">{{ serverText }}</el-text>
    </div>
    <div class="fl">
      <div class="s1"></div>
      <div class="fcl">
        <div class="fl">
          <el-text size="large">TPS：</el-text><el-text size="large" :type="tpsStatus">{{ tps }}</el-text>
        </div>
        <div class="fl">
          <el-text size="large">MSPT：</el-text><el-text size="large" :type="msptStatus">{{ mspt }}ms</el-text>
        </div>
        <div class="fl">
          <el-text size="large">CPU：</el-text><el-text size="large" type="success">20%</el-text>
        </div>
        <div class="fl">
          <el-text size="large">内存：</el-text><el-text size="large" type="success">20%</el-text>
        </div>
      </div>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { useTpsStore } from '@/stores/tps'
import { storeToRefs } from 'pinia'
import { computed } from 'vue'

const tpsStore = useTpsStore()
const { tps, mspt } = storeToRefs(tpsStore)

const serverStatus = computed(() => {
  if (tpsStatus.value === 'danger' || msptStatus.value === 'danger') {
    return 'danger'
  }
  if (tpsStatus.value === 'warning' || msptStatus.value === 'warning') {
    return 'warning'
  }
  return 'success'
})

const serverText = computed(() => {
  if (tpsStatus.value === 'danger' || msptStatus.value === 'danger') {
    return '差'
  }
  if (tpsStatus.value === 'warning' || msptStatus.value === 'warning') {
    return '良'
  }
  return '优'
})

const tpsStatus = computed(() => {
  return tps.value >= 20 ? 'success' : tps.value >= 10 ? 'warning' : 'danger'
})

const msptStatus = computed(() => {
  return mspt.value < 20 ? 'success' : mspt.value < 60 ? 'warning' : 'danger'
})
</script>
