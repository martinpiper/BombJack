import sys
import csv
# pip install xlsxwriter
import xlsxwriter

if __name__ == '__main__':
    csvfile = open(sys.argv[1], encoding='utf-8')
    csvreader = csv.reader(csvfile)
    headerRow = next(csvreader)

    indexQuantity = headerRow.index("Quantity")
    indexPlaced = headerRow.index("Placed")
    indexStockCode = headerRow.index("Stock Code")
    indexValue = headerRow.index("Value")
    indexPCBPackage = headerRow.index("PCB Package")

    indexSupplier = headerRow.index("Supplier")
    headerRow.pop(indexSupplier)
    indexGroupLayer = headerRow.index("Group Layer")
    headerRow.pop(indexGroupLayer)
    processed = [headerRow]

    countCAP = 0
    countCAP20 = 0

    for row in csvreader:
        if row[indexPlaced] != "Not placed":
            package = row[indexPCBPackage]
            if package.find("-CAP") > 0:
                countCAP += int(row[indexQuantity])
            if package.find("CAP20") > 0:
                countCAP20 += int(row[indexQuantity])
            code = row[indexStockCode]
            pos = code.rfind('/')
            if pos > 0:
                code = code[pos + 1:]
                pos = code.find('?')
                if pos > 0:
                    code = code[:pos]
                    row[indexValue] = code

            row.pop(indexSupplier)
            row.pop(indexGroupLayer)
            processed.append(row)

    if countCAP20 > 0:
        rowCAP = ["Capacitors", str(countCAP20), "One 100nF (0.1uF) capacitor for DIP ICs marked with extra DC near "
                                                 "pin 1", "K104K15X7RF53H5",
                  "https://www.mouser.sg/ProductDetail/Vishay-BC-Components/K104K15X7RF53H5?qs=sYfpZ29HcUSlgH5bSUHkYw"
                  "%3D%3D", "Decoupling capacitors for each DC component", "All with suffix CAP20", "Top Copper"]
        processed.append(rowCAP)

    if countCAP > 0:
        rowCAP = ["Capacitors", str(countCAP),
                  "One 100nF (0.1uF) capacitor for SMT ICs marked with extra SDC near pin 1", "CL05B104KO5NNNC",
                  "https://www.mouser.sg/ProductDetail/Samsung-Electro-Mechanics/CL05B104KO5NNNC?qs=hqM3L16%252BxlfT2SKOuAUq6Q%3D%3D",
                  "Decoupling capacitors for each SDC component", "All with suffix -CAP", "Top Copper"]
        processed.append(rowCAP)

#    output = open(sys.argv[1] + ".processed.csv", 'w', newline='', encoding='utf-8')
#    writer = csv.writer(output)
#    writer.writerows(processed)
#    output.close()

    workbook = xlsxwriter.Workbook(sys.argv[1] + ".xlsx")
    worksheet = workbook.add_worksheet()
    rowCount = 0
    for row in processed:
        worksheet.write_row(rowCount,0,row)
        rowCount += 1
    worksheet.autofit()
    workbook.close()
