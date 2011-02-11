#!/bin/sh

if [ ! -e /dev/nvram ] ; then
	mknod /dev/nvram c 10 144
fi

modprobe nvram

echo -n "Writing NVRAM/CMOS... "
# Settings:
# COM1 = port 0x3F8, IRQ 4
# COM2 = port 0x2F8, IRQ 3
# COM3 = port 0x3E8, IRQ 5
# COM4 = port 0x2E8, IRQ 10
cat << EOF | xxd -r > /dev/nvram
0000000: 0000 00fc f0f0 0f80 02c0 ff2f 2fbf ffff  ...........//...
0000010: 0000 0000 0000 0000 6897 1000 0067 973f  ........h....g.?
0000020: 0a93 c0ff 2080 e0f0 f0ff ffff ff0f bed1  .... ...........
0000030: e211 00ff c4dc fff9 f07a e31b bde0 ffff  .........z......
0000040: fffc 1dc0 7844 0200 2fc3 d091 ffff ffff  ....xD../.......
0000050: c3fd b3fc 00fc dfe2 1e2f 0000 0000 0000  ........./......
0000060: 0000 2f00 0000 0000 0000 00ff ff1d 2500  ../...........%.
0000070: 0084                                     ..
EOF

echo "done"

cat << EOF > /etc/serial.conf
/dev/ttyS0 port 0x03f8 irq 4 
/dev/ttyS1 port 0x02f8 irq 3
/dev/ttyS2 port 0x03e8 irq 5
/dev/ttyS3 port 0x02e8 irq 10
EOF

/etc/init.d/etc-setserial start

echo "You may need to reboot for changes to take effect."
