import { ref } from 'vue'
import { defineStore } from 'pinia'
import type { Position } from '@/types/position'
import type { PlayerConnectionState } from '@/types/state'

interface Player {
  name: string
  position: Position
  state: PlayerConnectionState
}
export const useEntityStore = defineStore('entity', () => {
  const player = ref<Player[]>([])
  return { player }
})
