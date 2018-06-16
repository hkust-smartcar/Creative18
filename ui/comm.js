const SerialPort = require('serialport')
const Buffer = require('buffer').Buffer

class Comm {
  constructor(portName, app) {
    this.app = app
    this.port = new SerialPort(portName, { baudRate: 115200 })
    this.port.on('error', function (err) {
      app.log(`'Error: ', ${err.message}`)
      console.log('Error: ', err.message);
    })
    this.port.on('data', data => {
      data.forEach(element => {
        this.parse(element)
      });
    })
    console.log(`connected to ${portName}`)
    app.log(`connected to ${portName}`)

    this.m_sendqueue = []
    this.historic_package_sum = 0

    this.pkgid = 0
    this.pkg_start = false // Check whether a package start has received
    this.len = -1 // Check package length
    this.buffer = [] // Buffer for incoming package
  }

  sendPackageImmediate({id, type, data}) {
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
    this.port.write(buf)
  }

  parse(ch) {
    if (!this.pkg_start && ch == 0xAA) {
      //handle head
      this.pkg_start = true
      this.len = 0
      this.buffer.push(ch)
    } else if (this.pkg_start && this.len == 0) {
      //handle length
      this.len = ch
      this.buffer.push(ch)
    } else if (this.pkg_start && this.buffer.length < this.len - 1) {
      //handle body
      this.buffer.push(ch)
    } else if (this.pkg_start && this.buffer.length == this.len - 1 && ch == 0xFF) {
      //handle tail
      this.buffer.push(ch)
      this.packageHandler(this.buffer)
      this.pkg_start = false;
      this.buffer = [];
    } else {
      //dump
      console.log('dump',this.buffer)
      this.pkg_start = false // remove the package from handling
      this.buffer = []
    }
  }

  packageHandler(buffer) {
    console.log(buffer)
  }
}

Comm.pkg_type = {
  kACK: 0x00,
  kRequestSetMotor: 0x01,
  kRequestEncoder: 0x02,
  kResponseEncoder: 0x03,
  kRequestMove: 0x04,
  kRequestSetMotorById: 0x05
}

module.exports = Comm
