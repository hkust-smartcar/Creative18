const Comm = require('./comm')
const Buffer = require('buffer').Buffer

class Protocol {
  constructor(portName, app) {
    this.comm = new Comm(portName, app, this.handler.bind(this))
    this.app = app
  }

  getEncoders() {
    this.comm.sendPackageImmediate({
      id: this.comm.getPackageId(),
      type: Comm.pkg_type.kRequestEncoders,
      data: Buffer.alloc(0)
    })
  }

  movecar(x, y) {
    const data = Buffer.alloc(4)
    data.writeInt16LE(x, 0)
    data.writeInt16LE(y, 0)
    this.comm.sendPackageImmediate({
      id: this.comm.getPackageId(),
      type: Comm.pkg_type.kRequestMove,
      data
    })
  }

  setMotorById(motor_id, power) {
    const data = Buffer.alloc(3)
    data.writeUInt8(motor_id, 0)
    data.writeInt16LE(power, 1)
    this.comm.sendPackageImmediate({
      id: this.comm.getPackageId(),
      type: 0x05,
      data
    })
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
      console.log(this)
        this.feedCornersHandler(pkg)
        break
    }
  }

  feedCornersHandler(pkg) {
    console.log(pkg)
    const frame_id = pkg.data.readUInt16LE(0)
    const chunk_id = pkg.data.readUInt8(2)
    if(!(frame_id in this.app.frame_corners)){
      this.app.frame_corners[frame_id] = []
      this.app.frame_chunks[frame_id] = {}
    }
    if (!(chunk_id in this.app.frame_chunks[frame_id])){
      this.app.frame_chunks[frame_id][chunk_id] = true
      for(let cursor = 3; cursor < pkg.data.length; cursor += 4){
        const x = pkg.data.readUInt8(cursor)
        const y = pkg.data.readUInt8(cursor + 2)
        this.app.frame_corners[frame_id].push([x,y])
      }
    }
  }
}

module.exports = Protocol