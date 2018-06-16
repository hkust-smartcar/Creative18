const Comm = require('./comm')
const Buffer = require('buffer').Buffer

class Protocol {
  constructor(portName, app){
    this.comm = new Comm(portName, app)
  }
}