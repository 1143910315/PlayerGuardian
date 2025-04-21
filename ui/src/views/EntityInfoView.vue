<template>
  <el-collapse v-model="activeNames" @change="handleChange">
    <el-collapse-item title="玩家" name="1">
      <el-table :data="tableData" style="width: 100%">
        <el-table-column prop="name" label="玩家名" width="180" />
        <el-table-column prop="state" label="在线状态" width="180" />
        <el-table-column prop="position" label="所在坐标" width="180" />
        <el-table-column prop="dimension" label="所在维度" />
      </el-table>
    </el-collapse-item>
    <el-collapse-item title="Feedback" name="2">
      <div>
        Operation feedback: enable the users to clearly perceive their operations by style updates
        and interactive effects;
      </div>
      <div>
        Visual feedback: reflect current state by updating or rearranging elements of the page.
      </div>
    </el-collapse-item>
    <el-collapse-item title="Efficiency" name="3">
      <div>Simplify the process: keep operating process simple and intuitive;</div>
      <div>
        Definite and clear: enunciate your intentions clearly so that the users can quickly
        understand and make decisions;
      </div>
      <div>
        Easy to identify: the interface should be straightforward, which helps the users to identify
        and frees them from memorizing and recalling.
      </div>
    </el-collapse-item>
    <el-collapse-item title="Controllability" name="4">
      <div>
        Decision making: giving advices about operations is acceptable, but do not make decisions
        for the users;
      </div>
      <div>
        Controlled consequences: users should be granted the freedom to operate, including
        canceling, aborting or terminating current operation.
      </div>
    </el-collapse-item>
  </el-collapse>
</template>

<script lang="ts" setup>
import { ref, computed } from 'vue'
import { useEntityStore } from '@/stores/entity'
import { PlayerConnectionStateNames } from '@/types/state'
import { DimensionNames, Dimension } from '@/types/position'
import type { Position } from '@/types/position'
import type { CollapseModelValue } from 'element-plus'

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

const activeNames = ref(['1'])
const handleChange = (val: CollapseModelValue) => {
  console.log(val)
}
</script>
