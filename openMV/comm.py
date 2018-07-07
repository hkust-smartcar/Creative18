from pyb import UART
import ustruct

PKGTYPE = {
            "kACK": 0x00,
            "kRequestSetMotor": 0x01,
            "kRequestEncoder": 0x02,
            "kResponseEncoder": 0x03,
            "kRequestMove": 0x04,
            "kRequestSetMotorById": 0x05,
            "kRequestEncoderById": 0x06,
            "kResponseEncoderById": 0x07,
            "kRequestEncoders": 0x08,
            "kResponseEncoders": 0x09,
            "kFeedGlobalRotation": 0x0A,
            "kFeedGlobalTranslation": 0x0B,
            "kRequestSetServo" : 0x0C,
            "kRequestAutoFeedEncoders" : 0x0D,
            "kFeedCorners": 0xA0,
            "kFeedLocalRotation" : 0xA1
        }

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

    def getPkgId(self):
        t = self.historic_package_sum % 256
        self.historic_package_sum += 1
        return t

    def queuePackage(self, pkg):
        pkg["id"] = (self.historic_package_sum+1) % 256
        self.m_sendqueue.append(pkg)
        self.historic_package_sum += 1
        return pkg["id"]

    def sendPackageImmediate(self, pkg):
        pkgid = pkg["id"]
        pkg_type = pkg["type"]
        data = pkg["data"]

        pkgid %= 256
        length = len(data) + 6
        cs = 0xAA + length + pkg_type + pkgid + 0xFF
        for b in data:
            cs += b
        cs %= 256
        #print("pkg_len", length)
        buf = bytes([0xAA, length, pkg_type, pkgid]) + data + bytes([cs, 0xFF])
        # print('[SI]send immediate', buf, length, pkg_type, pkgid, cs, "[/SI]")
        self.uart.write(buf)

    def period(self):
        for pkg in self.m_sendqueue:
            self.sendPackageImmediate(pkg)

    def listen(self):
        # print("way, ", self.uart.any())
        # return
        while(self.uart.any() > 0):
            ch = self.uart.read(1)
            ch = ustruct.unpack("B", ch)[0] % 256
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
        _, _, pkg_type, pkgid = ustruct.unpack("<BBBB", buf)
        pkg_type %= 256
        pkgid %= 256
        cs, _ = ustruct.unpack_from("<BB", buf, -2)
        cs %= 256
        pkg = {"type":pkg_type, "id":pkgid, "data": buf[4:-2]}
        #print(self.buf, pkg_type, pkgid, cs, pkg["type"], pkg["id"])
        if (pkg["type"] == PKGTYPE["kACK"]):
            n_queue = []
            for k, _pkg in enumerate(self.m_sendqueue):
                if _pkg["id"] != pkgid:
                    n_queue.append(_pkg)
                else:
                    n_queue += self.m_sendqueue[k+1:]
                    break
            self.m_sendqueue = n_queue
        elif (pkg["type"] != PKGTYPE["kACK"]):
            self.sendPackageImmediate({
                "id": pkgid,
                "type": PKGTYPE["kACK"],
                "data": b''
            })

        #print("called package handler", pkg["data"])
        # self.handler(pkg)
