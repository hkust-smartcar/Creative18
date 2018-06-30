from comm import PKGTYPE
import ustruct

comm = None


def init(_comm):
    global comm
    comm = _comm


def test():
    comm.sendPackageImmediate({"type": 1, "id": 0, "data": b''})


def test2():
    comm.queuePackage({"type": 1, "id": 0, "data": b'134'})


"""
corners: [(x,y),(x,y),...]
"""


def feedCorners(frame_id, corners):
    chunk_id = 0
    byte = b''
    for k, p in enumerate(corners):
        byte += ustruct.pack("<hh", p[0], p[1])
        if k % 23 == 22:
            comm.sendPackageImmediate({
                "type": PKGTYPE["kFeedCorners"],
                "id": comm.getPkgId(),
                "data": ustruct.pack("<h", frame_id, chunk_id/23)+byte
            })
            byte = b''
    comm.sendPackageImmediate({
        "type": PKGTYPE["kFeedCorners"],
        "id": comm.getPkgId(),
        "data": ustruct.pack("<h", frame_id, chunk_id/23)+byte
    })
