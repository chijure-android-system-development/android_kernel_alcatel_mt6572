 #
 # Copyright © 2015, Varun Chitre "varun.chitre15" <varun.chitre15@gmail.com>
 #
 # Custom build script
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
KERNEL_DIR=$PWD
ZIMAGE=$KERNEL_DIR/kernel/arch/arm/boot/zImage
BUILD_START=$(date +"%s")
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'
# Modify the following variable if you want to build
export CROSS_COMPILE="/home/chijure/arm-cortex-linux-gnueabi-linaro_5.2-2015.11-2/bin/arm-eabi-"

compile_kernel ()
{
echo -e "$blue***********************************************"
echo "          Compiling Phoenix kernel          "
echo -e "***********************************************$nocol"
export KBUILD_BUILD_USER="chijure"
export KBUILD_BUILD_HOST="team-Panther"
cd kernel
export TARGET_PRODUCT=yariss MTK_ROOT_CUSTOM=../mediatek/custom/ MTK_PATH_PLATFORM=../mediatek/platform/mt6572/kernel/ MTK_PATH_SOURCE=../mediatek/kernel/
make -j3
if ! [ -a $ZIMAGE ];
then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol"
exit 1
fi
}

case $1 in
clean)
export TARGET_PRODUCT=yariss MTK_ROOT_CUSTOM=../mediatek/custom/ MTK_PATH_PLATFORM=../mediatek/platform/mt6572/kernel/ MTK_PATH_SOURCE=../mediatek/kernel/
cd kernel
make -j8 clean mrproper
;;
*)
compile_kernel
;;
esac
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"
