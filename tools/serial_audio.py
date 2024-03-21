import sys
import subprocess

if len(sys.argv) < 3:
    exit(-1)

with open(sys.argv[1]) as file:
    with open(sys.argv[2], "wb") as out:
        for line in file:
            if line.startswith("***"):
                continue
            if (line == '\n'):
                continue
            words = line.strip().split(" ")
            bytesList = list(map(lambda word: bytes(word, 'utf8'), words))
            # print(int(words[0], 16).to_bytes(2, 'little'))
            # print(*bytesList, sep='')
                # {out.write(int(_hex, 16).to_bytes(2, 'little')) for _hex in words}
            for _hex in words:
                out.write(int(_hex, 16).to_bytes(2, 'little'))

def ffmpeg(filename):
    command = [
        "ffmpeg",
        "-hide_banner",
        "-f",
        "s16le",
        "-ar",
        "16k",
        "-ac",
        "1",
        "-i",
        filename,
        filename + ".wav"
    ]
    subprocess.run(command)

if len(sys.argv) < 4 or sys.argv[3] != '-no-ffmpeg':
    ffmpeg(sys.argv[2])
