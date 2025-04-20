export enum Dimension {
  OVERWORLD = 0,
  NETHER = 1,
  END = 2,
}

export const DimensionNames: Record<Dimension, string> = {
  [Dimension.OVERWORLD]: '主世界',
  [Dimension.NETHER]: '下界',
  [Dimension.END]: '末地',
}

export interface Position {
  x: number
  y: number
  z: number
  dimensionID: number
}
