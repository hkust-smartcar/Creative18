from comm import Package

comm = None

def init(_comm):
    global comm
    comm = _comm

def test():
    comm.sendPackageImmediate(Package(1,0,b'0'))

def test2():
    comm.queuePackage(Package(1,0,b'1234'))