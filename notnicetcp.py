#!/usr/bin/python3

import argparse
from bcc import BPF

parser = argparse.ArgumentParser(description='notnicetcp')
parser.add_argument('--device', type=str)
parser.add_argument('--attach', action='store_true')
parser.add_argument('--no-attach', dest='attach', action='store_false')
parser.set_defaults(attach=True)
args = parser.parse_args()

b = BPF(src_file='notnicetcp.c')
fn = b.load_func('notnicetcp', BPF.XDP)
if args.attach:
    b.attach_xdp(args.device, fn, 0)
    try:
        b.trace_print()
    except KeyboardInterrupt:
        pass
