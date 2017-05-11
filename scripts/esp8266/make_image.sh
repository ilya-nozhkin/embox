#!/bin/bash
sudo python ~/.local/lib/python2.7/site-packages/esptool.py elf2image -e 2 --flash_size 4m --flash_mode dio -e 1 -o "build/base/bin/embox-" "build/base/bin/embox"
