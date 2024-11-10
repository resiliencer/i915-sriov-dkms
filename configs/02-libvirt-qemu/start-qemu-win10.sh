 #!/bin/bash

cp -v /usr/share/edk2/x64/OVMF_VARS.4m.fd ./winvm_OVMF_VARS.4m.fd
EFI=" -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd "
EFI="${EFI} -drive if=pflash,format=raw,file=./winvm_OVMF_VARS.4m.fd "
CPU=" -m 4096 -enable-kvm  -smp dies=1,cores=4,threads=1,sockets=1 "
CPU="${CPU} -machine pc-q35-8.1,usb=off,vmport=off,dump-guest-core=off,kernel-irqchip=on,hpet=off "
# it is necessary to set host-phys-bits-limit=37 (38 or 39) to prevent ovmf crash during VM boot
CPU="${CPU} -cpu host,hv_vendor_id=GenuineIntel,+invtsc,+kvm_pv_unhalt,+kvm_pv_eoi,host-phys-bits-limit=37 "
VID=" -device vfio-pci,host=0000:00:02.1,romfile=./IntelGopDriver.efi "
# at first you need some videoadapter like QXL or VGA
# VID="${VID} -device virtio-vga,blob=true -display gtk,gl=on " 
# step 2: migrate to remote desktop client (connect from linux host to this vm)
# step 3: migrate to looking glass client (run looking-glass-client on linux host)
NET=" -netdev bridge,id=lan0,br=virbr0 -device virtio-net-pci,netdev=lan0 "   
MEM=" -object memory-backend-memfd,id=mem1,size=4096M -machine memory-backend=mem1 "
# use spice only if you need audio or input through it
SPC=" -device virtio-serial-pci,id=virtio-serial0 -chardev spicevmc,id=charchannel0,name=vdagent " 
SPC=" ${SPC} -spice port=5900,addr=127.0.0.1,disable-ticketing=on,seamless-migration=on "
MISC=" -nodefaults -monitor stdio "
# fastest shared memory DMA, ensure that kvmfr kernel module is loaded
FR=" -device ivshmem-plain,memdev=ivshmem -object memory-backend-file,id=ivshmem,share=on,mem-path=/dev/kvmfr0,size=128M "
# put here usb mouse/keyboard vendorid and productid or use another input
INPUT=" -device qemu-xhci -device usb-host,vendorid=0xXXXX,productid=0xXXXX "  
BOOT=" -boot menu=off,order=c -drive file=./winvm10.qcow2,index=0 -drive file=./virtio-cd.iso,index=1,media=cdrom "

qemu-system-x86_64 $CPU $EFI $VID $NET $MEM $SPC $MISC $FR $INPUT $BOOT 


