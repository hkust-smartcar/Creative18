const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer
const Comm = require('./comm')

pkgid = 0

const drawGraph = (app)=>{
  let c = document.getElementById('graph').getContext("2d")
  c.fillStyle = '#ffffff'
  c.fillRect(0,0,500,500)
  app.encoders_hist.forEach((snapshot,k)=>{
    snapshot.forEach((pos,i)=>{
      c.fillStyle = ['#ff0000','#00ff00','#0000ff'][i]
      c.fillRect(500-k*5,pos+250,5,5)
    })
  })
  while(app.encoders_hist.length>100){
    app.encoders_hist.shift()
  }
}

var app = new Vue({
  el: '#app',
  data: {
    comm: null,
    str: 'hello world',
    ports: [],
    parser: null,
    canvas: null,
    ctx: null,
    motor: [0,0,0],
    encoders: [0,0,0],
    encoders_hist: [],
    encoder_interval_crawler: null,
    craw_interval: 250,
    logs:''
  },
  methods: {
    log: str => app.logs+=str+'\n',

    connect: portName => {
      app.comm = new Comm(portName, app, app.handler)
    },

    movecar: event => {
      console.log(event)
      let [x, y] = [event.offsetX - 250,  250 - event.offsetY]
      app.ctx.fillStyle = "#FFFFFF"
      app.ctx.fillRect(0,0,100,100)
      app.ctx.fillStyle = "#000000"
      app.ctx.fillText(`( ${x}, ${y} )`,0,40)
      console.log(x,y)
      const data = Buffer.alloc(4)
      data.writeInt16LE(x,0)
      data.writeInt16LE(y,0)
      app.comm.sendPackageImmediate({
        id:plgid++,
        type:Comm.pkg_type.kRequestMove,
        data
      })
      pkgid%=256
    },
    
    setMotorById: motor_id=>{
      if(app.comm === null){
        alert('please connect first')
        return false
      }
      console.log('setmotor',motor_id,app.motor[motor_id])
      const data = Buffer.alloc(3)
      data.writeUInt8(motor_id,0)
      data.writeInt16LE(app.motor[motor_id],1)
      app.comm.sendPackageImmediate({
        id:pkgid++,
        type:0x05,
        data
      })
      pkgid%=256
    },

    getEncoders: () => {
      app.comm.sendPackageImmediate({
        id: pkgid++,
        type: Comm.pkg_type.kRequestEncoders,
        data: Buffer.alloc(0)
      })
      pkgid%=256
    },

    startEncodersIntervalGet: ()=>{
      app.encoder_interval_crawler = setInterval(app.getEncoders,app.craw_interval)
    },

    stopEncodersIntervalGet: ()=>{
      clearInterval(app.encoder_interval_crawler)
    },

    handler: pkg => {
      switch(pkg.type){
        case Comm.pkg_type.kResponseEncoderById:
          const id = pkg.data.readUInt8(0)
          const count = pkg.data.readInt32LE(1)
          console.log(id, count)
          app.encoders[id] = count
          break
        case Comm.pkg_type.kResponseEncoders:
          const encoders = [
            pkg.data.readInt32LE(0),
            pkg.data.readInt32LE(4),
            pkg.data.readInt32LE(8)
          ]
          app.encoders_hist.push(encoders.map((e,k)=>e-app.encoders[k]))
          app.encoders = encoders
          console.log(app.encoders)
          drawGraph(app)
          break

      }
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