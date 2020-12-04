#!/usr/bin/python3

from bcc import BPF
import time

device = "copro0"
print('initializing')
b = BPF(src_file='notnicetcp.c')
fn = b.load_func('notnicetcp', BPF.XDP)
b.attach_xdp(device, fn, 0)
print('attached')

try:
  b.trace_print()
except KeyboardInterrupt:
  pass
