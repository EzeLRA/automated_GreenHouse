#!/usr/bin/python3

import serial
import math
import time
import logging
import sys
from PIL import Image
import os
import argparse
import signal

def readImage(port_dev='COM5',width=320,height=240,regId=0xD0,regVal=0xD0):
	ser = serial.Serial(port_dev, 8000000)
	ser.flushInput()
	ser.write(bytearray([regId,regVal]))
	
	if regId == 0xD0 and regVal == 0xD0:
		dataY = ser.read(size=width*height)
		dataCbCr = ser.read(size=width*height)
		index = 0
		dataCb = list()
		dataCr = list()
		Alt = True

		for chroma in dataCbCr:
			if Alt:
				dataCr.extend([chroma,chroma])
				Alt = False
			else:
				dataCb.extend([chroma,chroma])
				Alt = True

		bitmap = list()

		for y in range(height):
			for x in range(width):
				Y = dataY[index]
				Cb = dataCb[index]
				Cr = dataCr[index]

				R = int(max(0, min(255,Y + 1.40200 * (Cr - 0x80))))
				G = int(max(0, min(255,Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80))))
				B = int(max(0, min(255,Y + 1.77200 * (Cb - 0x80))))
				bitmap.append((R,G,B))
				index += 1

		ser.close()
		return bitmap
	elif regId != 0xD0 and regVal == 0xD0:
		value = ser.read(1)
		return hex(ord(value))



parser = argparse.ArgumentParser()
parser.add_argument('-p','--port',metavar='port',default='COM5',nargs=1,type=str,help='serial port')
parser.add_argument('-f','--filename',metavar='filename',default="out.png",nargs=1,type=str,help='file name')
args = parser.parse_args()

fileDir = os.path.dirname(os.path.abspath(__file__))
display_width = 320
display_height = 240


port = args.port
index = 0
bitmap = readImage(port,display_width,display_height)
image = Image.new(mode="RGB", size = (display_width,display_height))

for y in range(display_height):
		for x in range(display_width):
			image.putpixel((x, y), bitmap[index])
			index += 1
image.save(fileDir+"/"+args.filename)

print("DONE")