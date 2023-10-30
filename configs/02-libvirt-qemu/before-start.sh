#!/bin/bash
# ensure that libvirtd is running

echo 2 > /sys/devices/pci0000:00/0000:00:02.0/sriov_numvfs
virsh net-start default
virsh nodedev-detach pci_0000_00_02_1
virsh nodedev-detach pci_0000_00_02_2
modprobe -v kvmfr static_size_mb=128
chmod 0660 /dev/kvmfr0

