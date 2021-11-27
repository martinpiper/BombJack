import sys


def get_data(file):
    if not hasattr(file, 'thisLine'):
        file.thisLine = 0

    while True:
        line = file.readline()
        file.thisLine = file.thisLine + 1
        line = line.strip()
        if not line:
            return None

        idx = line.find('d$')
        if idx == -1:
            continue

        line = line[idx+2:]
        bus = int(line, 16)

        # Ignore empty writes
        if bus == 0:
            continue

        # Ignore APU EBS writes
        if bus & 0x200:
            continue

        return line

    return line


def main():
    print("Begin reconciliation")
    file1 = open(sys.argv[1], 'r')
    file2 = open(sys.argv[2], 'r')
    while file1.readable() and file2.readable():
        line1 = get_data(file1)
        line2 = get_data(file2)

        if not line1 or not line2:
            break

        if line1 != line2:
            if line1[:6] != line2[:6]:
                print("address ", file1.thisLine, " ", line1, ' : ', file2.thisLine, " ", line2)
            else:
                print("data    ", file1.thisLine, " ", line1, ' : ', file2.thisLine, " ", line2)

    print('Read lines ', file1.thisLine, " : ", file2.thisLine)


if __name__ == "__main__":
    main()
