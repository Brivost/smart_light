#!/usr/bin/env python3
"""Convert a text file of PCM samples to a WAV file."""

import wave
import struct
import sys


def txt_to_wav(txt_path):
    wav_path = txt_path.rsplit(".", 1)[0] + ".wav"

    with open(txt_path, "r") as f:
        samples = [int(line.strip()) for line in f if line.strip()]

    with wave.open(wav_path, "w") as wav:
        wav.setnchannels(1)  # Mono
        wav.setsampwidth(2)  # 16-bit
        wav.setframerate(16000)  # 16 kHz
        wav.writeframes(struct.pack(f"<{len(samples)}h", *samples))

    print(f"Wrote {len(samples)} samples to {wav_path}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <input.txt>")
        sys.exit(1)
    txt_to_wav(sys.argv[1])
