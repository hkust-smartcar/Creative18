const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer
const parser = require('./parser')

pkgid = 0

Vue.component('hi',{
  template: `<p>hi</p>`
})

var app = new Vue({
  el: '#app',
  data: {
    str: 'hello world',
    ports: [],
    port: null,
    parser: null,
    canvas: null,
    ctx: null,
    motor: [0,0,0],
  },
  methods: {
    connect: portName => {
      app.port = new SerialPort(portName, { baudRate: 115200 })
      app.port.on('error', function (err) {
        console.log('Error: ', err.message);
      })
      app.port.on('data', parser)
      console.log(`connected to ${portName}`)
    },
    write: buffer => {
      app.port.write(buffer, console.log)
    },
    movecar: event => {
      let [x, y] = [event.clientX - 250, event.clientY - 250]
      app.ctx.fillStyle = "#FFFFFF"
      app.ctx.fillRect(0,0,100,100)
      app.ctx.fillStyle = "#000000"
      app.ctx.fillText(`( ${x}, ${y} )`,0,40)
      console.log(x,y)
    },
    sendPackageImmediate: ({id, type, data}) => {
      const len = data.length + 6
      const buf = Buffer.alloc(len)
      Buffer.from([0xAA,len,type,id]).copy(buf,0)
      data.copy(buf,4)
      buf.writeUInt8(0,len-2)
      buf.writeUInt8(0xFF,len-1)
      console.log(buf, buf.readUInt8())
      const checksum = buf.reduce((oldv,curv)=>(oldv+curv)%256,0)
      buf.writeUInt8(checksum,len-2)
      console.log(checksum, buf)
      app.port.write(buf)
    },
    setMotorById: motor_id=>{
      console.log('setmotor',motor_id,app.motor[motor_id])
      const data = Buffer.alloc(3)
      data.writeUInt8(motor_id,0)
      data.writeInt16LE(app.motor[motor_id],1)
      app.sendPackageImmediate({
        id:pkgid++,
        type:0x05,
        data
      })
      pkgid%=256
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