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
    logs:''
  },
  methods: {
    log: str => app.logs+=str+'\n',
    connect: portName => {
      app.comm = new Comm(portName, app)
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