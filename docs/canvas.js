const Init = () => {
  for (let i = 0; i < 9; i++) {
    ctx.fillStyle = "#000000"
    ctx.fillRect(0, i * box_size, 8 * box_size, 1)
    ctx.fillRect(i * box_size, 0, 1, 8 * box_size)
    if (i !== 8) mode[i] = [0, 0, 0, 0, 0, 0, 0, 0]
  }
}

const ClickCanvas = (e, d = 1) => {
  x = e.clientX - canvas.offsetLeft
  y = e.clientY - canvas.offsetTop
  x /= box_size
  y /= box_size
  ClickBox(Math.floor(x), Math.floor(y), d)
}

const RClickCanvas = e => {
  ClickCanvas(e, -1)
  return false;
}

const ClickBox = (i, j, d) => {
  mode[i][j] = (mode[i][j] + d + 3) % 3
  switch (mode[i][j]) {
    case 0:
      ctx.fillStyle = "#FFFFFF"
      break
    case 1:
      ctx.fillStyle = "#0000FF"
      break
    case 2:
      ctx.fillStyle = "#000000"
  }
  ctx.fillRect(i * box_size + 1, j * box_size + 1, box_size - 1, box_size - 1)
  idendityJSON.value = JSON.stringify(mode)
  const {points,matrix} = GenMatrix()
  adjMatrix.value = JSON.stringify(matrix)
  adjMatrix2.value = StringifyMatrix(matrix)
  pointList.value = JSON.stringify(points)
}

const FromJSON = e => {
  try{
    mode = JSON.parse(idendityJSON.value)
    mode.forEach((row,i)=>row.forEach((m,j)=>ClickBox(i,j,0)))
  } catch (e) {
    console.log(e)
  }
}

const FilterPoints = (type = 3) => {
  let points = []
  if(type === -1){
    mode.forEach((row,i)=>row.forEach((m,j)=>(m!==0?points.push([i,j]):null)))
  } else if (type <3 ) {
    mode.forEach((row,i)=>row.forEach((m,j)=>(m!==type?points.push([i,j]):null)))
  } else {
    points = [[],[],[]]
    mode.forEach((row,i)=>row.forEach((m,j)=>(points[m].push([i,j]))))
  }
  return points
}

const GenMatrix = () => {
  let points = FilterPoints()
  points = points[1].concat(points[2])
  // let matrix = points.map(a=>points.map(b=>Dist(a,b)).push(EdgeDist(a))).push(points.map(a=>EdgeDist(a)).push(0))
  let matrix = points.map(a=>points.map(b=>Dist(a,b)))
  matrix.forEach((row,i)=>row.push(EdgeDist(points[i])))
  matrix.push([...points.map(EdgeDist),0])
  return {points,matrix}
}

const Dist = ([x1,y1],[x2,y2]) => Math.sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
const EdgeDist = ([x,y]) => Math.min(x+1,y+1,8-x,8-y)

const StringifyMatrix = m => {
  return m.length+'\n'+m.map(r=>r.join(' ')).join('\n')
}

const box_size = 50
let mode = []
let canvas = document.getElementById('canvas')
let ctx = canvas.getContext("2d")
let idendityJSON = document.getElementById('idendityJSON')
let pointList = document.getElementById('pointList')
let adjMatrix = document.getElementById('adjMatrix')
let adjMatrix2 = document.getElementById('adjMatrix2')
canvas.width = box_size * 8 + 1
canvas.height = box_size * 8 + 1
canvas.style.width = box_size * 8 + 1
canvas.style.height = box_size * 8 + 1
canvas.oncontextmenu = RClickCanvas
Init()