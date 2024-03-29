package com.replicanet.DesignUpdater;

import au.com.bytecode.opencsv.CSVReader;
import org.apache.commons.lang3.StringUtils;

import java.awt.*;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.awt.Toolkit;
import java.util.*;
import java.util.List;

public class Main {
    static Robot robot;
    static int updated = 0;
    public static void main(String[] args) throws InterruptedException, IOException, AWTException {
        // Useful to quickly generate positions for the properties file
        if (args.length >= 1 && args[0].compareToIgnoreCase("-d") == 0) {
            while (true) {
                System.out.println(MouseInfo.getPointerInfo().getLocation().x + "    " + MouseInfo.getPointerInfo().getLocation().y);
                String pos = ".pos=" + MouseInfo.getPointerInfo().getLocation().x;
                pos += "," + MouseInfo.getPointerInfo().getLocation().y;
                pos += System.lineSeparator();

                setClipboard(pos);
                try {
                } catch (Exception e) {}

                Thread.sleep(100);

            }
//            System.exit(0);
        }

        Set<String> specifics = null;
        if (args.length >2 ) {
            specifics = new HashSet<>();
            for (int i = 2 ; i < args.length ; i++) {
                specifics.add(args[i]);
            }
        }

        System.getProperties().load(new FileInputStream(args[0]));

        // Expected header ordering: Value,Package,ToPackage,ToCode
        CSVReader csvReader = new CSVReader(new FileReader(args[1]));
        String[] headers = csvReader.readNext();
        List<String[]> records = csvReader.readAll();

        robot = new Robot();

        for (String[] row : records) {
            if (specifics != null) {
                if (!specifics.contains(row[0])) {
                    continue;
                }
            }
            processTypeAndPackage(true, row[0] , row[1] , row[2] , row[3]);
        }

        System.out.println("Updated: " + updated);
    }

    private static void processTypeAndPackage(boolean includePlaced , String type, String packageName, String toPackage, String toCode) throws InterruptedException {
        System.out.println("Processing type and package: " + type + " , " + packageName);

        clickOnElementWithPosition("DesignExplorer.SearchTab");
        clickOnElementWithPosition("DesignExplorer.Reset");

        clickOnElementWithPosition("DesignExplorer.ValueText");

        Thread.sleep(1000);

        setClipboard(type);
        pasteFromClipboard();
        typeEnter();


        if (StringUtils.isNotEmpty(packageName)) {
            clickOnElementWithPosition("DesignExplorer.PackageOpenDropdown");
            Thread.sleep(1000);
            clickOnElementWithPosition("DesignExplorer.PackageAddPackage");
            Thread.sleep(1000);
            setClipboard(packageName);
            pasteFromClipboard();
            typeEnter();
        }

        if (!includePlaced) {
            clickOnElementWithPosition("DesignExplorer.PlacementOpenDropdown");
            clickOnElementWithPosition("DesignExplorer.PlacementPickUnplaced");
        }

        clickOnElementWithPosition("DesignExplorer.Search");
        Thread.sleep(1000);

        String previousID = "";
        while (true) {
            setClipboard("");

            clickOnElementWithPosition("DesignExplorer.FirstRow.Reference");
            Thread.sleep(1000);
            copyToClipboard();

            // Some retries, just to be sure
            String text = getClipboard();
            if(text.isEmpty()) {
                Thread.sleep(1000);
                copyToClipboard();
                text = getClipboard();
            }
            if(text.isEmpty()) {
                Thread.sleep(1000);
                copyToClipboard();
                text = getClipboard();
            }
            if(text.isEmpty()) {
                clickOnElementWithPosition("DesignExplorer.FirstRow.Reference");
                Thread.sleep(1000);
                copyToClipboard();
                text = getClipboard();
            }
            if(text.isEmpty()) {
                System.out.println("Empty reference cell");
                return;
            }

            System.out.println("Got ref: " + text);
            if (text.compareTo(previousID) == 0) {
                System.out.println("Same reference cell");
                return;
            }

            previousID = text;

            // The stock code
            if (StringUtils.isNotEmpty(toCode)) {
                typeEscapeRight();
                typeEscapeRight();
                typeEscapeRight();
                typeEscapeRight();
                typeEscapeRight();
                typeEscapeRight();

                Thread.sleep(1000);

                System.out.println("Set code: " + toCode);
                setClipboard(toCode);
                pasteFromClipboard();
                Thread.sleep(1000);
                typeEnter();
            }

            // Now the package
            clickOnElementWithPosition("DesignExplorer.FirstRow.Reference");

            typeEscapeRight();
            typeEscapeRight();
            typeEscapeRight();

            Thread.sleep(1000);

            System.out.println("Set package: " + toPackage);
            setClipboard(toPackage);
            pasteFromClipboard();
            Thread.sleep(1000);
            typeEnter();

            Thread.sleep(1000);

            updated++;
        }
    }

    private static String getClipboard() {
        String data = "";
        boolean set = false;
        while (!set) {
            try {
                data = (String) Toolkit.getDefaultToolkit().getSystemClipboard().getData(DataFlavor.stringFlavor);
                set = true;
            } catch (Exception e) {
            }
        }
        return data;
    }

    private static void setClipboard(String text) throws InterruptedException {
        checkForMouseAbort();
        boolean set = false;
        while (!set) {
            try {
                StringSelection selection = new StringSelection(text);
                Toolkit.getDefaultToolkit().getSystemClipboard().setContents(selection, selection);
                set = true;
            } catch (Exception e) {
            }
        }

        Thread.sleep(1000);
    }

    public static void clickOnElementWithPosition(String element) throws InterruptedException {
        String value = System.getProperty(element + ".pos");
        String vals[] = value.split(",");

        reliableMoveToFast(Integer.parseInt(vals[0]) , Integer.parseInt(vals[1]));

        mouseLeftClick();
    }

    private static void mouseLeftClick() throws InterruptedException {
        Thread.sleep(50);
        robot.mousePress(InputEvent.BUTTON1_MASK);
        Thread.sleep(50);
        robot.mouseRelease(InputEvent.BUTTON1_MASK);
        Thread.sleep(50);
    }

    public static void reliableMoveToFast(int ixpos, int iypos) throws InterruptedException {
        // This jiggles the mouse around a bit for the GUI to catch-up with the inputs
        Thread.sleep(10);
        robot.mouseMove(ixpos+2, iypos+2);
        Thread.sleep(10);
        robot.mouseMove(ixpos-2, iypos-2);
        Thread.sleep(10);
        robot.mouseMove(ixpos, iypos);
        Thread.sleep(10);
    }

    public static void typeText(String text) throws InterruptedException {
        checkForMouseAbort();
        for (byte letter : text.getBytes()) {
            switch (letter) {
                case ':':
                    Thread.sleep(10);
                    robot.keyPress(KeyEvent.VK_SHIFT);
                    typeKeyCode(KeyEvent.VK_SEMICOLON);
                    Thread.sleep(10);
                    robot.keyRelease(KeyEvent.VK_SHIFT);
                    break;

                default:
                    int keyCode = KeyEvent.getExtendedKeyCodeForChar(letter);
                    typeKeyCode(keyCode);
                    break;
            }
        }
    }

    public static void typeEnter() throws InterruptedException {
        typeKeyCode(KeyEvent.VK_ENTER);
    }

    public static void typeEscapeRight() throws InterruptedException {
        typeKeyCode(KeyEvent.VK_ESCAPE);
        typeKeyCode(KeyEvent.VK_RIGHT);
    }

    public static void typeEscapeLeft() throws InterruptedException {
        typeKeyCode(KeyEvent.VK_ESCAPE);
        typeKeyCode(KeyEvent.VK_LEFT);
    }

    public static void typeKeyCode(int keycode) throws InterruptedException {
        checkForMouseAbort();
        Thread.sleep(10);
        robot.keyPress(keycode);
        Thread.sleep(10);
        robot.keyRelease(keycode);
        Thread.sleep(10);
    }

    public static void copyToClipboard() throws InterruptedException {
        checkForMouseAbort();
        Thread.sleep(100);
        robot.keyPress(KeyEvent.VK_CONTROL);
        Thread.sleep(100);
        robot.keyPress(KeyEvent.VK_C);
        Thread.sleep(100);
        robot.keyRelease(KeyEvent.VK_C);
        Thread.sleep(100);
        robot.keyRelease(KeyEvent.VK_CONTROL);
        Thread.sleep(100);
    }

    public static void pasteFromClipboard() throws InterruptedException {
        checkForMouseAbort();
        Thread.sleep(100);
        robot.keyPress(KeyEvent.VK_CONTROL);
        Thread.sleep(100);
        robot.keyPress(KeyEvent.VK_V);
        Thread.sleep(100);
        robot.keyRelease(KeyEvent.VK_V);
        Thread.sleep(100);
        robot.keyRelease(KeyEvent.VK_CONTROL);
        Thread.sleep(100);
    }

    public static void checkForMouseAbort() {
        if (MouseInfo.getPointerInfo().getLocation().x >= 0) {
            System.out.println("Mouse position abort");
            System.exit(-1);
        }
    }
}
