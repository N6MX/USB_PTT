#!/bin/bash

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root"
	exit
else
	avrdude -c usbtiny -p attiny85 -u -U efuse:w:0xff:m
	avrdude -c usbtiny -p attiny85 -u -U hfuse:w:0xdf:m
	avrdude -c usbtiny -p attiny85 -u -U lfuse:w:0xe1:m
fi
