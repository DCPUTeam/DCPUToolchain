#!/bin/sh
#
# Toolchain IDE start script for Mac OSX.

TOOLCHAIN_BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/DCPU-16 Toolchain//'`"
TOOLCHAIN_RESOURCES="$TOOLCHAIN_BUNDLE/Contents/Resources"
TOOLCHAIN_TEMP="/tmp/dcputoolchain/$UID"
TOOLCHAIN_ETC="$TOOLCHAIN_TEMP/etc"

echo "running $0"
echo "TOOLCHAIN_BUNDLE: $TOOLCHAIN_BUNDLE"

# Start X11 ...
ps -wx -ocommand | grep -e '[X]11.app' > /dev/null
if [ "$?" != "0" -a ! -f ~/.xinitrc ]; then
    echo "rm -f ~/.xinitrc" > ~/.xinitrc
    sed 's/xterm/# xterm/' /usr/X11R6/lib/X11/xinit/xinitrc >> ~/.xinitrc
fi

mkdir -p $TOOLCHAIN_TEMP
cat << __END_OF_GETDISPLAY_SCRIPT__ > "$TOOLCHAIN_TEMP/getdisplay.sh"
#!/bin/sh
mkdir -p "$TOOLCHAIN_TEMP"

if [ "\$DISPLAY"x == "x" ]; then
    echo :0 > "$TOOLCHAIN_TEMP/display"
else
    echo \$DISPLAY > "$TOOLCHAIN_TEMP/display"
fi
__END_OF_GETDISPLAY_SCRIPT__
chmod +x "$TOOLCHAIN_TEMP/getdisplay.sh"
rm -f $TOOLCHAIN_TEMP/display
open-x11 $TOOLCHAIN_TEMP/getdisplay.sh || \
open -a XDarwin $TOOLCHAIN_TEMP/getdisplay.sh || \
echo ":0" > $TOOLCHAIN_TEMP/display

while [ "$?" == "0" -a ! -f $TOOLCHAIN_TEMP/display ];
do
  #echo "Waiting for display $TOOLCHAIN_TEMP/display"
  sleep 1;
done
export "DISPLAY=`cat $TOOLCHAIN_TEMP/display`"

ps -wx -ocommand | grep -e '[X]11' > /dev/null || exit 11

# Setup temporary runtime files
rm -rf "$TOOLCHAIN_TEMP"

export "DYLD_LIBRARY_PATH=$TOOLCHAIN_RESOURCES/lib"
export "FONTCONFIG_PATH=$TOOLCHAIN_RESOURCES/etc/fonts"
export "PATH=$TOOLCHAIN_RESOURCES/bin:$PATH"

# Also set the toolchain environment variables.
export "TOOLCHAIN_MODULES=$TOOLCHAIN_RESOURCES/share/dcpu/modules"
export "TOOLCHAIN_KERNELS=$TOOLCHAIN_RESOURCES/share/dcpu/kernels"
export "TOOLCHAIN_STDLIBS=$TOOLCHAIN_RESOURCES/share/dcpu/stdlib"

exec "$TOOLCHAIN_RESOURCES/bin/dtide"
