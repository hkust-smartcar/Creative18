const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer
const Protocol = require('./protocol')
const download = require('./download')

pkgid = 0

var app = new Vue({
  el: '#app',
  data: {
    console:console,
    protocol: null,
    str: 'hello world',
    ports: [],
    parser: null,
    canvas: null,
    ctx: null,
    motor: [0, 0, 0],
    encoders: [0, 0, 0],
    encoders_hist: [],
    encoder_interval_crawler: null,
    craw_interval: 250,
    logs: '',
    frame_corners:{},
    frame_chunks:{},
    active: null
  },
  methods: {
    log: str => app.logs += str + '\n',

    connect: portName => {
      app.protocol = new Protocol(portName, app)
    },

    movecar: event => {
      console.log(event)
      let [x, y] = [event.offsetX - 250, 250 - event.offsetY]
      app.ctx.fillStyle = "#FFFFFF"
      app.ctx.fillRect(0, 0, 100, 100)
      app.ctx.fillStyle = "#000000"
      app.ctx.fillText(`( ${x}, ${y} )`, 0, 40)
      console.log(x, y)
      app.protocol.movecar(x, y)
    },

    setMotorById: motor_id => {
      if (app.protocol === null) {
        alert('please connect first')
        return false
      }
      app.motor[motor_id]=Number(app.motor[motor_id])
      app.log('setmotor '+ motor_id +' '+ app.motor[motor_id])
      console.log('setmotor', motor_id, app.motor[motor_id])
      app.protocol.setMotorById(motor_id, app.motor[motor_id])
    },

    getEncoders: () => app.protocol && app.protocol.getEncoders,

    startEncodersIntervalGet: () => {
      app.encoder_interval_crawler = setInterval(app.getEncoders, app.craw_interval)
    },

    stopEncodersIntervalGet: () => {
      clearInterval(app.encoder_interval_crawler)
    },

    drawGraph: () => {
      let c = document.getElementById('graph').getContext("2d")
      c.fillStyle = '#ffffff'
      c.fillRect(0, 0, 500, 500)
      app.encoders_hist.forEach((snapshot, k) => {
        snapshot.forEach((pos, i) => {
          c.fillStyle = ['#ff0000', '#00ff00', '#0000ff'][i]
          c.fillRect(500 - k * 5, pos + 250, 5, 5)
        })
      })
      while (app.encoders_hist.length > 100) {
        app.encoders_hist.shift()
      }
    },

    clearFrameCorners: ()=>{
      app.frame_corners = {}
      app.frame_chunks = {}
    },

    exportFrameCorners: ()=>{
      download('frame_corners.txt',JSON.stringify(app.frame_corners))
    }
  }
})

window.onload = () => {
  SerialPort.list((errors, ports) => {
    app.ports = ports
  })

  app.canvas = document.getElementById('canvas')
  app.ctx = app.canvas.getContext("2d")
  app.ctx.fillStyle = "#000000"
  app.ctx.fillRect(0, 250, 500, 1)
  app.ctx.fillRect(250, 0, 1, 500)
}