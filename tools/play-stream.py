from re import A
import sys

from serial import Serial
import sounddevice as sd
from sounddevice import CallbackFlags, sleep

argc = len(sys.argv)
if argc < 2:
    print("No serial input specified")
    exit(-1)

indata = []


def callback(outdata, frames: int, time, status: CallbackFlags):
    if status:
        print(status)
    if len(indata) > 2:
        # print(len(outdata), len(indata[0]), frames)
        print(len(indata), len(outdata), len(indata[0]))
        outdata[:] = indata.pop()


def read_bytes(size: int, serial: Serial):
    tmp = b''
    for _ in range(size):
        _bytes = serial.read(2)
        #data = int.from_bytes(_bytes, 'big')
        #print(_bytes, data, data.to_bytes(2, 'little'))
        #tmp += data.to_bytes(2, 'little')
        tmp += _bytes
    return tmp


serial = Serial(sys.argv[1], 115200)


try:
    dur_ms = 100
    duration = 16 * dur_ms
    with sd.RawOutputStream(
        16000,
        duration,
        dtype="int16",
        callback=callback,
        channels=1
    ):
        serial.read_all()
        while True:
            #if len(indata) > 3:
            #    sleep(1)
            #    continue
            indata.append(read_bytes(duration, serial))
except KeyboardInterrupt:
    exit(0)
# except Exception as e:
#    serial.is_open
#    print("Error:", e, file=sys.stderr)
#    exit(-1)
