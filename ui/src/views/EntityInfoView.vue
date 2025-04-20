<template>
  <el-table :data="tableData" style="width: 100%">
    <el-table-column prop="name" label="玩家名" width="180" />
    <el-table-column prop="state" label="在线状态" width="180" />
    <el-table-column prop="position" label="所在坐标" width="180" />
    <el-table-column prop="dimension" label="所在维度" />
  </el-table>
</template>

<script lang="ts" setup>
import { computed } from 'vue'
import { useEntityStore } from '@/stores/entity'
import { PlayerConnectionStateNames } from '@/types/state'
import { DimensionNames, Dimension } from '@/types/position'
import type { Position } from '@/types/position'

const entityStore = useEntityStore()

const tableData = computed(() =>
  entityStore.player.map((player) => ({
    name: player.name,
    state: PlayerConnectionStateNames[player.state],
    position: formatPosition(player.position),
    dimension: getDimensionName(player.position.dimensionID),
  })),
)

// 坐标格式化函数
const formatPosition = (pos: Position) => {
  return `(${pos.x.toFixed()}, ${pos.y.toFixed()}, ${pos.z.toFixed()})`
}

// 维度名称获取函数
const getDimensionName = (id: number) => {
  return DimensionNames[id as Dimension] ?? `未知维度 ${id}`
}
</script>
