#!/bin/sh

dDep=n
iDep=n
current_dir=`pwd`

echo "Download Dependencies? (y/n)"
read dDep

if [ $dDep == "n" ]; then
    echo "Build Dependencies? (y/n)"
    read iDep
fi

if [ $dDep == "y" ]; then
    setup/download_dependencies ../
fi

if [ $dDep == "y" ] || [ $iDep == "y" ]; then
    setup/mac/build_dependencies_mac
fi

cd $current_dir

mkdir Syncer.app                    > dummy
mkdir Syncer.app/Contents           > dummy
mkdir Syncer.app/Contents/MacOS     > dummy
mkdir Syncer.app/Contents/Resources > dummy

mkdir icon.iconset
sips -z 16 16     res/logo.png --out icon.iconset/icon_16x16.png        > dummy
sips -z 32 32     res/logo.png --out icon.iconset/icon_16x16@2x.png     > dummy
sips -z 32 32     res/logo.png --out icon.iconset/icon_32x32.png        > dummy
sips -z 64 64     res/logo.png --out icon.iconset/icon_32x32@2x.png     > dummy
sips -z 128 128   res/logo.png --out icon.iconset/icon_128x128.png      > dummy
sips -z 256 256   res/logo.png --out icon.iconset/icon_128x128@2x.png   > dummy
sips -z 256 256   res/logo.png --out icon.iconset/icon_256x256.png      > dummy
cp res/logo.png icon.iconset/icon_256x256@2x.png
iconutil -c icns icon.iconset
rm -R icon.iconset
rm dummy

cp setup/mac/Info.plist Syncer.app/Contents
mv icon.icns Syncer.app/Contents/Resources

echo Building...
setup/mac/build.sh ../
