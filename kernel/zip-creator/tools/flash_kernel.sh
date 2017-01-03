#!/sbin/sh
 #
 # Copyright © 2014, Varun Chitre "varun.chitre15" <varun.chitre15@gmail.com> 
 #
 # Live ramdisk patching script
 #
 # This software is licensed under the terms of the GNU General Public
 # License version 2, as published by the Free Software Foundation, and
 # may be copied, distributed, and modified under those terms.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # Please maintain this if you use this script or any part of it
 #
cd /tmp/
./busybox dd if=/dev/bootimg of=./boot.img bs=6291456 count=1
./unpackbootimg -i /tmp/boot.img
./mkbootimg --kernel /tmp/zImage --ramdisk /tmp/boot.img-ramdisk.gz --pagesize 2048  --base 0x10000000 --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --output /tmp/newboot.img
./busybox dd if=/tmp/newboot.img of=/dev/bootimg bs=6291456 count=1
