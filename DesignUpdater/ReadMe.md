# Automated package and stock code update for Proteus Design Tool

This tool automatically updates specific component package and stock code information, using a reference file of intended updates. This mitigates the risk due to manual errors when updating large complex designs. This is especially useful if reliable and repeatable updates to multiple design is needed.

While the bulk property search and replacement function in Proteus does exist, it does not allow multiple components using pairs of value and package to be identified and updated with precise mapping and across multiple design files.

### Usage

The jar file with dependencies in the "target" folder can be executed from the java command line.

#### To update a design

The command line takes two parameters: <Properties file for GUI element locators> <CSV file detailing search and replacement values>  

* e.g. DesignExplorer.properties ToSMTSOIC.csv

The CSV file has four columns:

* Value,Package,ToPackage,ToCode

The Value column is the original component value. e.g. 74LS273<br>
The Package column is an optional component package to search for, this can be used to narrow down the search. e.g. DIL20CAP20<br>
The ToPackage column is the package to update the component to. e.g. SOIC127P780X200-20N-CAP
The ToCode column is an optional CODE property to apply to a component, this could be an ordering link from your preferred supplier. e.g. https://www.mouser.sg/ProductDetail/Texas-Instruments/SN74LS273NSR?qs=SL3LIuy2dWxsq%2FCTeVMKZg%3D%3D

#### To update the locator properties file

Current versions of Proteus (version 8) do not respond well to GUI element automation, they are practically opaque to the Window system. [ref](https://support.labcenter.com/forums/viewtopic.php?t=8409)

The properties locator file contains named properties for expected GUI objects (e.g. "DesignExplorer.SearchTab.pos") and their typical coordinates in the Proteus application.
Depending on screen resolution and product version, these may need to be updated.
Running this tool with the command line parameter "-d" will start a debug mode where the curent mouse coordinate is displayed and also copied to the clipboard. The coordinate can then be pasted from the clipboard into the property file for the specific GUI element that needs to be updated.
