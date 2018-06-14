const SerialPort = require('serialport')

var app = new Vue({
  el: '#app',
  data: {
    str: 'hello world',
    ports: [],
    port: null,
    parser: null
  },
  methods: {
    connect: portName => {
      this.port = new SerialPort(portName, { baudRate: 115200 })
      this.port.on('error', function (err) {
        console.log('Error: ', err.message);
      })
      this.port.on('data', console.log)
      console.log(`connected to ${portName}`)
    },
    write: buffer => {
      this.port.write(buffer, console.log)
    }
  }
})

SerialPort.list((errors, ports) => {
  app.ports = ports
})