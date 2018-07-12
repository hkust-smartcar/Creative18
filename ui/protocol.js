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
        this.responseEncoderByIdHandler(pkg)
        break
      case Comm.pkg_type.kResponseEncoders:
        this.responseEncodersHandler(pkg)
        break
      case Comm.pkg_type.kFeedCorners:
        this.feedCornersHandler(pkg)
        break
      case Comm.pkg_type.kFeedGlobalRotation:
        this.feedGlobalRotationHandler(pkg)
        break
      case Comm.pkg_type.kFeedLocalRotation:
        this.feedLocalRotationHandler(pkg)
        break
      case Comm.pkg_type.kFeedGlobalTranslation:
        this.feedGlobalTranslationHandler(pkg)
        break
      case Comm.pkg_type.kFeedLocalTranslation:
        this.feedLocalTranslationHandler(pkg)
        break
    }
  }

  responseEncoderByIdHandler(pkg){
    const id = pkg.data.readUInt8(0)
    const count = pkg.data.readInt32LE(1)
    console.log(id, count)
    this.app.encoders[id] = count
  }

  responseEncodersHandler(pkg){
    const encoders = [
      pkg.data.readInt32LE(0),
      pkg.data.readInt32LE(4),
      pkg.data.readInt32LE(8)
    ]
    this.app.encoders_hist.push(encoders.map((e, k) => e - app.encoders[k]))
    this.app.encoders = encoders
    this.app.drawGraph(app)
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
        const x = pkg.data.readInt16LE(cursor)
        const y = pkg.data.readInt16LE(cursor + 2)
        this.app.frame_corners[frame_id].push([x,y])
      }
    }
    this.app.drawCorners(this.app.frame_corners[frame_id], true)
    this.updateFrame(frame_id)
  }

  feedGlobalRotationHandler(pkg){
    this.app.globalRotation = pkg.data.readFloatLE(0)
    this.app.globalRotationLapse = pkg.data.readUInt16LE(4)
    const frame_id = pkg.data.readUInt16LE(6)
    this.app.globalRotations[frame_id] = this.app.globalRotation
    this.app.globalRotationReceivedTime = Date.now()
    this.updateFrame(frame_id)
  }

  feedLocalRotationHandler(pkg){
    this.app.localRotation = pkg.data.readFloatLE(0)
    this.app.localRotationLapse = pkg.data.readUInt16LE(4)
    const frame_id = pkg.data.readUInt16LE(6)
    this.app.localRotations[frame_id] = this.app.localRotation
    this.app.localRotationReceivedTime = Date.now()
    this.updateFrame(frame_id)
  }

  feedGlobalTranslationHandler(pkg){
    this.app.globalTranslationX = pkg.data.readInt32LE(0)
    this.app.globalTranslationY = pkg.data.readInt32LE(4)
    this.app.globalTranslationLapse = pkg.data.readUInt16LE(8)
    const frame_id = pkg.data.readUInt16LE(10)
    this.app.globalTranslations[frame_id] = [this.app.globalTranslationX,this.app.globalTranslationY]
    this.app.globalTranslationReceivedTime = Date.now()
    this.updateFrame(frame_id)
  }

  feedLocalTranslationHandler(pkg){
    this.app.localTranslationX = pkg.data.readInt32LE(0)
    this.app.localTranslationY = pkg.data.readInt32LE(4)
    this.app.localTranslationLapse = pkg.data.readUInt16LE(8)
    const frame_id = pkg.data.readUInt16LE(10)
    this.app.localTranslations[frame_id] = [this.app.localTranslationX,this.app.localTranslationY]
    this.app.localTranslationReceivedTime = Date.now()
    this.updateFrame(frame_id)
  }

  updateFrame(frame_id){
    if(this.app.last_frame_id < frame_id){
      const now = Date.now()
      const lapse = now - this.app.last_frame_receive_time
      this.app.last_frame_id = frame_id
      this.app.last_frame_receive_time = now
      this.app.fps = Math.floor(1000/lapse)
    }
  }
}

module.exports = Protocol