export enum PlayerConnectionState {
  Offline = 0,
  Online = 1,
}

export const PlayerConnectionStateNames: Record<PlayerConnectionState, string> = {
  [PlayerConnectionState.Offline]: '离线',
  [PlayerConnectionState.Online]: '在线',
}
