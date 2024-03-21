import sys
import subprocess

if len(sys.argv) < 3:
    print("Input and/or output filename missing")
    exit(-1)

with open(sys.argv[1]) as file:
    with open(sys.argv[2], "wb") as out:
        for line in file:
            if line.startswith("***"): # Skip lines starting with ***
                continue
            if (line == '\n'): # Skip empty lines
                continue
            words = line.strip().split(" ") # Split line to hex words e.g. 0xffff
            # print(int(words[0], 16).to_bytes(2, 'little'))
            # {out.write(int(_hex, 16).to_bytes(2, 'little')) for _hex in words}
            for _hex in words:
                out.write(int(_hex, 16).to_bytes(2, 'little'))

def ffmpeg(filename: str):
    """
    Use ffmpeg to add wav file header to `filename` and save it to
    `filename`.wav
    """
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
