from pyb import UART
import ustruct


class Package:
    def __init__(self, id, pkg_type, data):
        self.id = id
        self.type = pkg_type
        self.data = data


class Comm:
    def __init__(self, _handler):
        self.uart = UART(3, 115200, timeout_char=1000)
        self.m_sendqueue = []
        self.historic_package_sum = 0

        self.pkgid = 0
        self.pkg_start = False  # Check whether a package start has received
        self.len = -1  # Check package length
        self.buffer = []  # Buffer for incoming package
        self.handler = _handler
        self.pkg_type = {
            "kACK": 0x00,
            "kRequestSetMotor": 0x01,
            "kRequestEncoder": 0x02,
            "kResponseEncoder": 0x03,
            "kRequestMove": 0x04,
            "kRequestSetMotorById": 0x05,
            "kRequestEncoderById": 0x06,
            "kResponseEncoderById": 0x07,
            "kRequestEncoders": 0x08,
            "kResponseEncoders": 0x09
        }

    def queuePackage(self, pkg):
        pkg.id = (self.historic_package_sum+1) % 256
        self.m_sendqueue.append(pkg)
        self.historic_package_sum += 1
        return pkg.id

    def sendPackageImmediate(self, pkg):
        pkgid = pkg.id
        pkg_type = pkg.type
        data = pkg.data

        pkgid %= 256
        length = len(data) + 6
        cs = 0xAA + length + pkg_type + pkgid + 0xFF
        for b in data:
            cs += b
        cs %= 256
        buf = bytes([0xAA, length, pkg_type, pkgid]) + data + bytes([cs, 0xFF])
        print('send immediate',buf)
        self.uart.write(buf)

    def period(self):
        for pkg in self.m_sendqueue:
            self.sendPackageImmediate(pkg)

    def listen(self):
        # print("way, ", self.uart.any())
        # return
        while(self.uart.any() > 0):
            ch = self.uart.read(1)
            ch = ustruct.unpack("b",ch)[0] % 256
            print("ch",ch, "buffer", self.buffer)
            if ((not self.pkg_start) and ch == 0xAA):
                # handle head
                print("handle head")
                self.pkg_start = True
                self.len = 0
                self.buffer.append(ch)
            elif (self.pkg_start and self.len == 0):
                # handle length
                print("handle len")
                self.len = ch
                self.buffer.append(ch)
            elif (self.pkg_start and len(self.buffer) < self.len - 1):
                # handle body
                print("handle body")
                self.buffer.append(ch)
            elif (self.pkg_start and len(self.buffer) == self.len - 1 and ch == 0xFF):
                # handle tail
                print("handle tail")
                self.buffer.append(ch)
                self.buildBufferPackage(self.buffer)
                self.pkg_start = False
                self.buffer = []
                print("package recieved")
            else:
                # dump
                print("dump")
                self.pkg_start = False  # remove the package from handling
                self.buffer = []

    def buildBufferPackage(self, buf):
        # _, pkg_type, pkgid = ustruct.unpack("<bbb", buf)
        # cs, _ = ustruct.unpack_from("<bb", -2, buf)
        # pkg = Package(pkg_type, pkgid, buf[4:-2])
        # if (pkg.type == self.pkg_type["kACK"]):
        #     n_queue = []
        #     for k, pkg in enumerate(self.m_sendqueue):
        #         if pkg.id != pkgid:
        #             n_queue.append(pkg)
        #         else:
        #             n_queue += self.m_sendqueue[k+1:]
        #             break
        #     self.m_sendqueue = n_queue
        # elif (pkg.type != self.pkg_type["kACK"]):
        #     self.sendPackageImmediate(
        #         Package(pkg.id, self.pkg_type["kACK"], b''))

        print("called package handler")
        # self.handler(pkg)
