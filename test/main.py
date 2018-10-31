#!/usr/bin/env python3

import sys
import os

idprocess = sys.argv[1]
while True:
    buf = sys.stdin.buffer.read(128)
    a = buf[0] + 1
    nbytes = sys.stdout.buffer.write(a.to_bytes(128, 'little'))
    sys.stdout.flush()
