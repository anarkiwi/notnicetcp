# notnicetcp
Make TCP worse


Install and usage
=================

Arrange for FAUCET coprocessor to send traffic to "copro0" host interface.
This eBPF program will hairpin TCP back into FAUCET's pipeline, only worse.

    sudo apt-get install bpfcc-tools python3-bpfcc
    sudo ./notsonice.py
