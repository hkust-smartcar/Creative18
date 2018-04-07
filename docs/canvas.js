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
}

const FromJSON = e => {
  try{
    mode = JSON.parse(idendityJSON.value)
    mode.forEach((row,i)=>row.forEach((m,j)=>ClickBox(i,j,0)))
  } catch (e) {
    console.log(e)
  }
}

const box_size = 50
let mode = []
let canvas = document.getElementById('canvas')
let ctx = canvas.getContext("2d")
let idendityJSON = document.getElementById('idendityJSON')
canvas.width = box_size * 8 + 1
canvas.height = box_size * 8 + 1
canvas.style.width = box_size * 8 + 1
canvas.style.height = box_size * 8 + 1
canvas.oncontextmenu = RClickCanvas
Init()