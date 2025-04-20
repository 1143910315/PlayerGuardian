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
    <div
      class="hidden receiveTpsData receiveEntityData"
      @tps-data="tpsData"
      @entity-data="entityData"
    ></div>
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
import { useTpsStore } from '@/stores/tps'
import { useEntityStore } from '@/stores/entity'
import { PlayerConnectionState } from '@/types/state'

const leftDrawer = useTemplateRef('leftDrawer')
const showMenu = () => {
  leftDrawer.value?.show()
}
const hideMenu = () => {
  leftDrawer.value?.hide()
}

interface TpsEventDetail {
  TPS: number
  MSPT: number
}

interface EntityEventDetail {
  age: number
  category: number
  dimensionId: number
  isLeashed: boolean
  isTame: boolean
  isTrusting: boolean
  itemTypeName: string
  nameTag: string
  runtimeID: number
  typeName: string
  x: number
  y: number
  z: number
}

const tpsData = (event: CustomEvent<TpsEventDetail>) => {
  const store = useTpsStore()
  store.tps = event.detail.TPS
  store.mspt = event.detail.MSPT
}

const entityData = (event: CustomEvent<EntityEventDetail[]>) => {
  const store = useEntityStore()
  const currentPlayers = new Map<string, EntityEventDetail>()

  // 预处理当前事件中的玩家，建立快速查找表
  event.detail
    .filter((detail) => detail.typeName === 'minecraft:player')
    .forEach((playerDetail) => {
      currentPlayers.set(playerDetail.nameTag, playerDetail)
    })

  // 第一轮遍历：更新或新增玩家数据
  currentPlayers.forEach((detail, nameTag) => {
    const existingIndex = store.player.findIndex((p) => p.name === nameTag)

    if (existingIndex > -1) {
      // 更新现有玩家数据
      const target = store.player[existingIndex]
      target.position = {
        x: detail.x,
        y: detail.y,
        z: detail.z,
        dimensionID: detail.dimensionId,
      }
    } else {
      // 添加新玩家
      store.player.push({
        name: nameTag,
        state: PlayerConnectionState.Online,
        position: {
          x: detail.x,
          y: detail.y,
          z: detail.z,
          dimensionID: detail.dimensionId,
        },
      })
    }
  })

  // 第二轮遍历：更新玩家在线状态
  store.player.forEach((player) => {
    if (!currentPlayers.has(player.name)) {
      player.state = PlayerConnectionState.Offline
    } else {
      player.state = PlayerConnectionState.Online
    }
  })
}
</script>
