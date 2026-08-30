import sys
import csv
import math

if __name__ == '__main__':
    csvfile = open(sys.argv[1], 'r', encoding='utf-8')

    csvreader = csv.reader(csvfile)
    headerRow = next(csvreader)

    indexPartID = headerRow.index("Part ID")
    indexPackage = headerRow.index("Package")
    indexLayer = headerRow.index("Layer")
    indexRotation = headerRow.index("Rotation")
    indexX = headerRow.index("X")
    indexY = headerRow.index("Y")

    packageNames = []
    packageToRot = {}
    packageToX = {}
    packageToY = {}
    packageToCapPackage = {}
    packageToCapRotation = {}
    packageToCapOffsetX = {}
    packageToCapOffsetY = {}

    for row in csvreader:

        # Look for distinct IC packages, not caps, and note their rotation and position
        if (row[indexPackage].find("DIL") != -1) or (row[indexPackage].find("-CAP") != -1):
            packageName = row[indexPackage] + "_" + row[indexRotation]

            assert packageName not in packageNames
            packageNames.append(packageName)
            packageToRot[packageName] = row[indexRotation]
            packageToX[packageName] = float(row[indexX])
            packageToY[packageName] = float(row[indexY])
            continue

        # For safety, look for caps
        if (row[indexPackage].find("CAP20") != -1) or (row[indexPackage].find("CAPC1005X55N") != -1):
            bestDistance = -1
            bestPackageName = ""
            x = float(row[indexX])
            y = float(row[indexY])
            # Find the closest IC package
            for name in packageNames:
                distance = math.dist([x, y], [packageToX[name], packageToY[name]])
                if bestDistance < 0 or distance < bestDistance:
                    bestDistance = distance
                    bestPackageName = name
#            print("Found best for: " + bestPackageName + " : " + str(row))

            assert bestPackageName not in packageToCapPackage
            packageToCapPackage[bestPackageName] = row[indexPackage]
            packageToCapRotation[bestPackageName] = row[indexRotation]
            packageToCapOffsetX[bestPackageName] = x - packageToX[bestPackageName]
            packageToCapOffsetY[bestPackageName] = y - packageToY[bestPackageName]

    assert len(packageNames) == len(packageToCapPackage)

    # Now we have the database of package and rotation to decoupling cap, rotation, offset, process the target file
    csvfile = open(sys.argv[2], 'r', encoding='utf-8')

    csvreader = csv.reader(csvfile)
    headerRow = next(csvreader)

    indexPartID = headerRow.index("Part ID")
    indexPackage = headerRow.index("Package")
    indexLayer = headerRow.index("Layer")
    indexRotation = headerRow.index("Rotation")
    indexX = headerRow.index("X")
    indexY = headerRow.index("Y")

    processed = [headerRow]

    capIndex = [0, 0]
    capIndexToPrefix = ["DC-", "SDC-"]
    for row in csvreader:

        processed.append(row)

        # Look for distinct IC packages, not caps, and note their rotation and position
        if (row[indexPackage].find("DIL") != -1 and row[indexPackage].find("CAP20") != -1) or (
                row[indexPackage].find("-CAP") != -1):
            normalisedRot = int(row[indexRotation])
            while normalisedRot < 0:
                normalisedRot += 360
            while normalisedRot >= 360:
                normalisedRot -= 360
            packageName = row[indexPackage] + "_" + str(normalisedRot)

            capType = 0
            if row[indexPackage].find("-CAP") != -1:
                capType = 1

            assert packageName in packageNames

            newRow = row.copy()
            capIndex[capType] += 1
            newRow[indexPartID] = capIndexToPrefix[capType] + str(capIndex[capType])
            newRow[indexPackage] = packageToCapPackage[packageName]
            newRow[indexLayer] = row[indexLayer]
            newRow[indexRotation] = packageToCapRotation[packageName]
            newRow[indexX] = str(round(float(row[indexX]) + packageToCapOffsetX[packageName], 3))
            newRow[indexY] = str(round(float(row[indexY]) + packageToCapOffsetY[packageName], 3))

            processed.append(newRow)
            continue

    csvfile = open(sys.argv[2] + "_caps.csv", 'w', encoding='utf-8')

    csvwriter = csv.writer(csvfile, lineterminator="\r")
    csvwriter.writerows(processed)

    print("Caps:", capIndexToPrefix, capIndex)
