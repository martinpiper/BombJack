import serial
import time

#serialPort = serial.Serial(port="COM5", baudrate=115200, parity=serial.PARITY_ODD, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
#serialPort = serial.Serial(port="COM5") # This seems to be just as fast as explicitly setting the speed
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
    toSendWithEnable = bytearray([value | 0x40, value])
    # Setup data for the FTDI latches.
    # The no enable version is important, because it switches off the selector with the data still held, which causes a positive edge on the FTDI latches to load the held data.
    serialPort.write(toSendWithEnable)
    totalBytesSent = totalBytesSent + 1
    if ((totalBytesSent % 10240) == 0):
        deltaTime = time.time() - startTime
        if (deltaTime > 0):
            print(int(totalBytesSent / deltaTime), " bytes per second")


def sendFTDILatchNybble(latch, nybble):
    nybble = nybble & 0x0f
    sendRawByte((latch << 4) | nybble)


def setLatch(latch):
    sendFTDILatchNybble(2, latch)


def setDataByte(value):
    value = int(value)
    sendFTDILatchNybble(0, value)
    sendFTDILatchNybble(1, value >> 4)


def sendCompleteData():
    sendFTDILatchNybble(3, 0x02)  # Lo _PC Hi Use
    sendFTDILatchNybble(3, 0x02)  # Lo _PC Hi Use
    sendFTDILatchNybble(3, 0x03)  # Hi _PC Hi Use
    sendFTDILatchNybble(3, 0x03)  # Hi _PC Hi Use


# Reset the FTDI interface latches to clear
sendFTDILatchNybble(0, 0)
sendFTDILatchNybble(1, 0)
sendFTDILatchNybble(2, 0)
sendFTDILatchNybble(3, 0)
# Prepare the interface for FTDI usage
sendFTDILatchNybble(3, 0x03)  # Hi _PC Hi Use
sendFTDILatchNybble(3, 0x03)  # Hi _PC Hi Use

# Reset the interface
setLatch(7)
setDataByte(0x00)
sendCompleteData()
setDataByte(0x80)
sendCompleteData()


# Write RAM
setLatch(6)
i = 0
char = 0
# Some rows of 1024 characters
while i < (256 * 1024):
#    setDataByte(0x00)       # Screen @
    setDataByte(char)
    sendCompleteData()
    setDataByte(i / 16)       # Colour 1 white
    sendCompleteData()
    i = i + 1
    if ((i % 10240) == 0):
        print(i, " RAM bytes sent")
        char = char + 1



# Then some ending bytes
setDataByte(0xff)       # Screen @
i = 0
while i < 32:
    sendCompleteData()
    sendCompleteData()
    i = i + 1


# Reset the interface again
setLatch(7)
setDataByte(0x00)
sendCompleteData()
setDataByte(0x80)
sendCompleteData()

# Stop using FTDI mode
sendFTDILatchNybble(3, 0x01)  # Hi _PC
sendFTDILatchNybble(3, 0x01)  # Hi _PC
