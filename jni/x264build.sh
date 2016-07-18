#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <libs_output_dir>"
    exit
fi

if ! [ -d $1 ]; then
	echo "folder '$1' does not exist"
	exit
fi

if ! [ $(env | grep "ANDROID_NDK") ]; then
	echo "Please set ANDROID_NDK env variable to your NDK location"
	echo "eg: export ANDROID_NDK=~/Android/Sdk/ndk-bundle"
	exit
fi

if ! [ -d $ANDROID_NDK ]; then
	echo "Wrong NDK location: folder does not exist"
	exit
fi

output_folder=$1

#remove last '/' from output_folder, if present
last_char_idx=$((${#output_folder}-1))
last_char=${output_folder:$last_char_idx:1}
if [ $last_char == '/' ]; then
	output_folder=${output_folder:0:$last_char_idx} 
	echo $output_folder
fi

LIBNAME="x264"
LIB_FILENAME="lib$LIBNAME.a"
platform=9
toolchain_version=4.9

while ! [ -d $ANDROID_NDK/platforms/android-$platform ]; do
	echo "Platform $platform not installed. Installed platforms:"
	ls $ANDROID_NDK/platforms
	echo -n "Choose platform > "
	read platform
done

ABIs=(
	arm
	mips
	x86 
	#x86_64
)
toolchains=(
	arm-linux-androideabi-$toolchain_version
	mipsel-linux-android-$toolchain_version
	x86-$toolchain_version
	x86_64-$toolchain_version
)
prefixes=(
	arm-linux-androideabi-
	mipsel-linux-android-
	i686-linux-android-
	x86_64-linux-android-
)
hosts=(
	arm-linux
	mips-linux
	i686-linux
	x86_64-linux
)
NDK_folders=(
	armeabi
	mips
	x86
	x86_64
)

total=${#ABIs[*]}
for (( i=0; i<$total; i++ ))
do
	abi=${ABIs[$i]}
	tc=${toolchains[$i]}
    pr=${prefixes[$i]}
    host=${hosts[$i]}
    folder="$output_folder/${NDK_folders[$i]}"
    
	echo "configuring for $abi arch"
	
	com="./configure \
		--host=$host \
		--enable-static \
		--disable-asm \
		--enable-pic \
		--cross-prefix=$ANDROID_NDK/toolchains/$tc/prebuilt/linux-x86_64/bin/$pr \
		--sysroot=$ANDROID_NDK/platforms/android-$platform/arch-$abi/" 
	#echo $com
	
    ./configure \
		--host=$host \
		--enable-static \
		--disable-asm \
		--enable-pic \
		--cross-prefix=$ANDROID_NDK/toolchains/$tc/prebuilt/linux-x86_64/bin/$pr \
		--sysroot=$ANDROID_NDK/platforms/android-$platform/arch-$abi/ \
	#>/dev/null && echo "configured. Compiling..."
	
	make clean #>/dev/null #do not print onto stdout
	make #>/dev/null #do not print onto stdout
	echo "$LIBNAME compiled successfully -> created "$LIB_FILENAME
	
	if ! [ -d $folder ]; then
		mkdir $folder && echo "Created dir $folder"
	fi
	cp $LIB_FILENAME $folder && echo "Copied $LIB_FILENAME to $folder"
	
done

echo "cleaning..."
make clean #>/dev/null #do not print onto stdout
echo "clean success"
