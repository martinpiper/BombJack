import serial
import time

# serialPort = serial.Serial(port="COM5", baudrate=115200, parity=serial.PARITY_ODD, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
# serialPort = serial.Serial(port="COM5") # This seems to be just as fast as explicitly setting the speed
serialPort = serial.Serial(port="COM5", baudrate=512000)

# Using 1MHz
# 256000  RAM bytes sent
# 18195  bytes per second
# Using 4MHz does not alter the transfer time, it is FTDI limited then, the DMA is four times faster though :)

totalBytesSent = 0
startTime = time.time()

print(serialPort)


def sendRawByte(value):
    global totalBytesSent, startTime
    toSendWithEnable = bytearray([value])
    # Setup data for the FTDI latches.
    serialPort.write(toSendWithEnable)
    totalBytesSent = totalBytesSent + 1
    if ((totalBytesSent % 10240) == 0):
        deltaTime = time.time() - startTime
        if (deltaTime > 0):
            print(int(totalBytesSent / deltaTime), " bytes per second")


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
    enablePCOnWrite()
    value = int(value)
    sendFTDILatchData(0, value & 0x7f)
    sendFTDILatchData(1, ((value >> 1) & 0x40) | (lastLatches[1] & 0xbf))


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
i = 0
char = 0
# Some rows of 1024 characters
while i < (256 * 1024):
#    sendDataByte(0x00)       # Screen @
    sendDataByte(char)
    sendDataByte(i / 16)  # Colour 1 white
    i = i + 1
    if ((i % 10240) == 0):
        print(i, " RAM bytes sent")
        char = char + 1

# Then some ending bytes
i = 0
while i < 32:
    sendDataByte(0xff)  # Screen @
    sendDataByte(0xff)  # Screen @
    i = i + 1

# Reset the interface again
setLatch(7)
sendDataByte(0x00)
sendDataByte(0x80)

# Stop using FTDI mode
sendFTDILatchData(1, 0x10)  # Hi _PC
