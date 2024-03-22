import sys
from typing import TextIO

from serial import Serial

argc = len(sys.argv)
if argc < 2:
    exit(-1)

file_name = sys.argv[2] if argc >= 3 else 'serial_out'

port = Serial(sys.argv[1], 115200)

BUFFER_START = "START OF BUFFER DUMP"
BUFFER_END = "END OF BUFFER DUMP"


def get_decoded_line(port: Serial):
    """
    Read and decode a line as utf-8 from `port`
    """
    line = port.readline()
    return str(line, 'utf8').strip()


def write_file(file: TextIO, port: Serial):
    """
    Read data from `port` until `BUFFER_END` is reached.

    Return `None` on successful write and current line
    if the line contains `BUFFER_START`
    """
    line = ""
    while BUFFER_END not in line:
        line = get_decoded_line(port)
        if BUFFER_START in line:
            return line
        print(line)
        file.write(line + "\n")
    return None


def start_write(line: str, port: Serial):
    """
    Start file write with `line` and restart on partial file
    """
    while line is not None:
        with open(file_name, "w") as file:
            print(line)
            file.write(line + "\n")
            line = write_file(file, port)


_quit = False
while not _quit:
    line = get_decoded_line(port)
    if BUFFER_START in line:
        start_write(line, port)
        _quit = True
