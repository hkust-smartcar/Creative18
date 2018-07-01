const Comm = require('./comm')
const Buffer = require('buffer').Buffer

class Protocol {
  constructor(portName, app) {
    this.comm = new Comm(portName, app, this.handler)
    this.app = app
  }

  getEncoders() {
    this.comm.sendPackageImmediate({
      id: pkgid++,
      type: Comm.pkg_type.kRequestEncoders,
      data: Buffer.alloc(0)
    })
    pkgid %= 256
  }

  movecar(x, y) {
    const data = Buffer.alloc(4)
    data.writeInt16LE(x, 0)
    data.writeInt16LE(y, 0)
    this.comm.sendPackageImmediate({
      id: plgid++,
      type: Comm.pkg_type.kRequestMove,
      data
    })
    pkgid %= 256
  }

  setMotorById(motor_id, power) {
    const data = Buffer.alloc(3)
    data.writeUInt8(motor_id, 0)
    data.writeInt16LE(power, 1)
    this.comm.sendPackageImmediate({
      id: pkgid++,
      type: 0x05,
      data
    })
    pkgid %= 256
  }

  handler(pkg) {
    const app = this.app
    switch (pkg.type) {
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
        app.encoders_hist.push(encoders.map((e, k) => e - app.encoders[k]))
        app.encoders = encoders
        console.log(app.encoders)
        app.drawGraph(app)
        break
      case Comm.pkg_type.kFeedCorners:
        feedCornersHandler(app, pkg)
        break
    }
  }

  feedCornersHandler(pkg) {
    console.log(pkg)
  }
}

module.exports = Protocol