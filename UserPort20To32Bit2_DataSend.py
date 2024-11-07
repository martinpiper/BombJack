import random
import sys

import serial
import time

# serialPort = serial.Serial(port="COM5", baudrate=115200, parity=serial.PARITY_ODD, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
serialPort = serial.Serial(port="COM5") # This seems to be just as fast as explicitly setting the speed
#serialPort = serial.Serial(port="COM5", baudrate=115200)
#serialPort = serial.Serial(port="COM5", baudrate=4000000)


# Using 4MHz
# 2 * 1024 * 1024  RAM bytes sent
# 125947  bytes per second
# Using different clock speeds, with the DIP switch clock divider does change the speed. Indicating we are FTDI limited now. Previous code was Python serial limited.

totalBytesSent = 0
startTime = time.time()

print(serialPort)

toSendBytes = bytearray()


def sendRawByte(value):
    toSendBytes.append(value)


lastLatches = [0, 0]


def enablePCOnWrite():
    lastLatches[1] = lastLatches[1] & 0xef


def disablePCOnWrite():
    lastLatches[1] = lastLatches[1] | 0x10


def sendFTDILatchData(latch, data):
    data = data & 0x7f
    lastLatches[latch] = data
    sendRawByte((latch << 7) | data)


def setLatch(latch):
    disablePCOnWrite()
    toSend = lastLatches[1] & 0xf0
    toSend = toSend | (latch & 0x0f)
    sendFTDILatchData(1, toSend)


def sendDataByte(value):
    global totalBytesSent, startTime
    totalBytesSent = totalBytesSent + 1
    enablePCOnWrite()
    value = int(value)
    sendFTDILatchData(0, value & 0x7f)
    sendFTDILatchData(1, ((value >> 1) & 0x40) | (lastLatches[1] & 0xbf))


print("Preparing data...")

# Reset the FTDI interface latches to clear
sendFTDILatchData(0, 0)
sendFTDILatchData(1, 0)
# Prepare the interface for FTDI usage
sendFTDILatchData(1, 0x30)  # Hi _PC Hi Use


# Reset the interface
setLatch(7)
sendDataByte(0x00)
sendDataByte(0x80)

# Write RAM
setLatch(6)

if len(sys.argv) < 2:
    i = 0
    char = random.randint(0, 255)
    # Some rows of 1024 characters
    while i < (2 * 1024 * 1024):
        # while i < (256 * 1024):
        # while i < (20 * 1024):
        #    sendDataByte(0x00)       # Screen @
        #    sendDataByte(0x00)       # Screen Black
        sendDataByte(char)
        #    sendDataByte(i / 16)  # Colour
        sendDataByte(i / 4)  # Colour
        i = i + 1
        if ((i % 10240) == 0):
            char = char + 1

    # Then some ending bytes
    i = 0
    while i < 32:
        sendDataByte(0xff)  # Screen @
        sendDataByte(0xff)  # Screen @
        i = i + 1
else:
    file = open(sys.argv[1], 'rb')
    fileData = file.read()
    i = 0
    while i < len(fileData):
        sendDataByte(fileData[i])
        i = i + 1


# Reset the interface again
setLatch(7)
sendDataByte(0x00)
sendDataByte(0x80)

# Stop using FTDI mode
sendFTDILatchData(1, 0x10)  # Hi _PC

print("Final buffer send...")
pos = 0
chunkSize = 256000
while pos < len(toSendBytes):
    serialPort.write(toSendBytes[pos:pos+chunkSize])
    pos = pos + chunkSize
    print( int(100 * pos / len(toSendBytes)), " percent")

deltaTime = time.time() - startTime
if deltaTime > 0:
    print(int(totalBytesSent / deltaTime), " bytes per second")
