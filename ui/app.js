const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer
const Comm = require('./comm')

pkgid = 0

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
    encoder: [0,0,0],
    logs:''
  },
  methods: {
    log: str => app.logs+=str+'\n',

    connect: portName => {
      app.comm = new Comm(portName, app, app.handler)
    },

    movecar: event => {
      let [x, y] = [event.clientX - 250, event.clientY - 250]
      app.ctx.fillStyle = "#FFFFFF"
      app.ctx.fillRect(0,0,100,100)
      app.ctx.fillStyle = "#000000"
      app.ctx.fillText(`( ${x}, ${y} )`,0,40)
      console.log(x,y)
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
    },

    handler: pkg => {
      switch(pkg.type){
        case Comm.pkg_type.kResponseEncoderById:
          const id = pkg.data.readUInt8(0)
          const count = pkg.data.readInt32LE(1)
          console.log(id, count)
          app.encoder[id] = count
          break
        case Comm.pkg_type.kResponseEncoders:
          app.encoder = [
            pkg.data.readInt32LE(0),
            pkg.data.readInt32LE(4),
            pkg.data.readInt32LE(8)
          ]
          console.log(app.encoder)
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