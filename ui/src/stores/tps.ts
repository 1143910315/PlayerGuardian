import { ref } from 'vue'
import { defineStore } from 'pinia'

export const useTpsStore = defineStore('tps', () => {
  const tps = ref(0)
  const mspt = ref(0)

  return { tps, mspt }
})
