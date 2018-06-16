const pkg_type = {
  kACK: 0x00,
  kRequestSetMotor: 0x01,
  kRequestEncoder: 0x02,
  kResponseEncoder: 0x03,
  kRequestMove: 0x04,
  kRequestSetMotorById: 0x05
}

m_sendqueue = []
historic_package_sum = 0
