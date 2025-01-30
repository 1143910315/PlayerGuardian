import { createRouter, createMemoryHistory } from 'vue-router'
import 'vue-router'

declare module 'vue-router' {
  interface RouteMeta {
    pageName: string
  }
}

const router = createRouter({
  history: createMemoryHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      meta:{
        pageName: '主页'
      },
      component: () => import('../views/HomeView.vue'),
    },
    {
      path: '/playerTrajectoryMap',
      name: 'playerTrajectoryMap',
      meta:{
        pageName: '玩家轨迹地图'
      },
      component: () => import('../views/PlayerTrajectoryMapView.vue'),
    },
    {
      path: '/interaction',
      name: 'interaction',
      meta:{
        pageName: ''
      },
      component: () => import('../views/InteractionView.vue'),
    },
    {
      path: '/about',
      name: 'about',
      meta:{
        pageName: ''
      },
      component: () => import('../views/AboutView.vue'),
    },
  ],
})

export default router
