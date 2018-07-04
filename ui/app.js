const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer
const Protocol = require('./protocol')
const download = require('./download')

var app = new Vue({
  el: '#app',
  data: {
    console:console,
    protocol: null,
    str: 'hello world',
    ports: [],
    parser: null,
    canvas: null,
    cornerCanvas: null,
    cornerCtx: null,
    ctx: null,
    motor: [0, 0, 0],
    encoders: [0, 0, 0],
    encoders_hist: [],
    encoder_interval_crawler: null,
    craw_interval: 250,
    logs: '',
    frame_corners:{},
    frame_chunks:{},
    active: null,
    globalRotations:{},
    globalRotation: 0,
    globalRotationLapse: 0,
    globalRotationReceivedTime: 0,
    globalTranslations:{},
    globalTranslationX: 0,
    globalTranslationY: 0,
    globalTranslationLapse: 0,
    globalTranslationReceivedTime: 0,
    last_frame_id:0,
    last_frame_receive_time:0,
    fps:'N/A'
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
      c.fillStyle = 'rgba(255, 255, 255, 1)'
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
    },

    clearFrameRotations: ()=>{
      app.globalRotations = {}
    },

    exportFrameRotations: ()=>{
      download('frame_rotations.txt',JSON.stringify(app.globalRotations))
    },

    clearFrameTranslations: ()=>{
      app.globalTranslations = {}
    },

    exportFrameTranslations: ()=>{
      download('frame_translations.txt',JSON.stringify(app.globalTranslations))
    },

    clearFrameDetails: ()=>{
      app.globalTranslations = {}
      app.globalRotations = {}
      app.frame_corners = {}
      app.frame_chunks = {}
    },

    exportFrameDetails: ()=>{
      details = {}
      download('frame_details.txt',JSON.stringify({
        corners:app.corners,
        rotations:app.globalRotations,
        translations:app.globalTranslations
      }))
    },

    drawLine: (ctx,x1,y1,x2,y2,color = '#000000')=>{
      ctx.strokeStyle = color
      ctx.beginPath()
      ctx.moveTo(x1,y1)
      ctx.lineTo(x2,y2)
      ctx.stroke()
    },

    drawCorners: (corners, link)=>{
      app.cornerCtx.fillStyle = 'rgba(255, 255, 255, 1)'
      app.cornerCtx.fillRect(0, 0, 500, 500)
      app.cornerCtx.fillStyle = '#ff0000'
      corners.forEach(([x,y],k)=>{
        if(link){
          const [x0,y0] = (k%4 == 0 ? corners[k+3] : corners[k-1])
          app.drawLine(app.cornerCtx, x0,y0,x,y)
        }
        app.cornerCtx.fillRect(x, y, 5, 5)
      })
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
  app.cornerCanvas = document.getElementById('corners_canvas')
  app.cornerCtx = app.cornerCanvas.getContext('2d')
}