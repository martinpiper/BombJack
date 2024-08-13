import serial

#serialPort = serial.Serial(port="COM5", baudrate=115200, parity=serial.PARITY_NONE, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
serialPort = serial.Serial(port="COM5")


def sendRawByte(value):
    toSendWithEnable = bytearray([value | 0x40])
    toSendNoEnable = bytearray([value])
    # Setup data for the FTDI latches.
    serialPort.write(toSendWithEnable)
    # The no enable version is important, because it switches off the selector with the data still held, which causes a positive edge on the FTDI latches to load the held data.
    serialPort.write(toSendNoEnable)


def sendFTDILatchNybble(latch, nybble):
    nybble = nybble & 0x0f;
    sendRawByte((latch << 4) | nybble)


def setLatch(latch):
    sendFTDILatchNybble(2, latch)


def setDataByte(value):
    sendFTDILatchNybble(0, value)
    sendFTDILatchNybble(1, value >> 4)


def sendCompleteData():
    sendFTDILatchNybble(3, 0x01)    # Hi _PC
    sendFTDILatchNybble(3, 0x03)    # Hi _PC Hi Use
    sendFTDILatchNybble(3, 0x02)    # Lo _PC Hi Use
    sendFTDILatchNybble(3, 0x03)    # Hi _PC Hi Use
    sendFTDILatchNybble(3, 0x01)    # Hi _PC


# Reset the FTDI interface latches to clear
sendFTDILatchNybble(0, 0)
sendFTDILatchNybble(1, 0)
sendFTDILatchNybble(2, 0)
sendFTDILatchNybble(3, 0)


# Reset the interface
setLatch(7)
setDataByte(0x00)
sendCompleteData()
setDataByte(0x80)
sendCompleteData()


# Write RAM
setLatch(6)
i = 0
# 10 rows of 1024 characters
while i < (10 * 1024):
    setDataByte(0x00)       # Screen @
    sendCompleteData()
    setDataByte(0x01)       # Colour 1 white
    sendCompleteData()
    i = i + 1


# Reset the interface again
setLatch(7)
setDataByte(0x00)
sendCompleteData()
setDataByte(0x80)
sendCompleteData()

