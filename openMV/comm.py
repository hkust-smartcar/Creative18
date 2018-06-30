from pyb import UART
import ustruct


class Package:
    def __init__(self, pkg_type, id, data):
        self.type = pkg_type
        self.id = id
        self.data = data


class Comm:
    def __init__(self, _handler):
        self.uart = UART(3, 115200, timeout_char=1000)
        self.m_sendqueue = []
        self.historic_package_sum = 0

        self.pkgid = 0
        self.pkg_start = False  # Check whether a package start has received
        self.length = -1  # Check package length
        self.buf = []  # Buffer for incoming package
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
        print('[SI]send immediate',buf,length, pkg_type, pkgid, "[/SI]")
        self.uart.write(buf)

    def period(self):
        for pkg in self.m_sendqueue:
            self.sendPackageImmediate(pkg)

    def listen(self):
        # print("way, ", self.uart.any())
        # return
        while(self.uart.any() > 0):
            ch = self.uart.read(1)
            ch = ustruct.unpack("b", ch)[0] % 256
            # print("ch",ch, "buffer", self.buf)
            if ((not self.pkg_start) and ch == 0xAA):
                # handle head
                # print("handle head")
                self.pkg_start = True
                self.length = 0
                self.buf.append(ch)
            elif (self.pkg_start and self.length == 0):
                # handle length
                # print("handle len")
                self.length = ch
                self.buf.append(ch)
            elif (self.pkg_start and len(self.buf) < self.length - 1):
                # handle body
                # print("handle body")
                self.buf.append(ch)
            elif (self.pkg_start and len(self.buf) == self.length - 1 and ch == 0xFF):
                # handle tail
                # print("handle tail")
                self.buf.append(ch)
                self.buildBufferPackage()
                self.pkg_start = False
                self.buf = []
                #print("package recieved")
            else:
                # dump
                # print("dump")
                self.pkg_start = False  # remove the package from handling
                self.buf = []

    def buildBufferPackage(self):
        buf = bytes(self.buf)
        _, _, pkg_type, pkgid = ustruct.unpack("<bbbb", buf)
        pkg_type %= 256
        pkgid %= 256
        cs, _ = ustruct.unpack_from("<bb", buf, -2)
        cs %= 256
        pkg = Package(pkg_type, pkgid, buf[4:-2])
        print(self.buf, pkg_type, pkgid, cs, pkg.type, pkg.id)
        if (pkg.type == self.pkg_type["kACK"]):
            n_queue = []
            for k, _pkg in enumerate(self.m_sendqueue):
                if _pkg.id != pkgid:
                    n_queue.append(_pkg)
                else:
                    n_queue += self.m_sendqueue[k+1:]
                    break
            self.m_sendqueue = n_queue
        elif (pkg.type != self.pkg_type["kACK"]):
            self.sendPackageImmediate(
                Package(pkg.id, self.pkg_type["kACK"], b'')
            )

        print("called package handler")
        # self.handler(pkg)
