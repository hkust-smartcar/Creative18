comm = None

def init(_comm):
    global comm
    comm = _comm

def test():
    comm.sendPackageImmediate({"type":1, "id":0, "data":b''})

def test2():
    comm.queuePackage({"type":1, "id":0, "data":b'134'})