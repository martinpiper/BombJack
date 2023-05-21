package com.replicanet.DesignUpdater;

import java.awt.*;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

public class Main {
    public static void main(String[] args) throws InterruptedException, IOException {
        if (args.length >= 1 && args[0].compareToIgnoreCase("-d") == 0) {
            while (true) {
                System.out.println(MouseInfo.getPointerInfo().getLocation().x + "    " + MouseInfo.getPointerInfo().getLocation().y);
                Thread.sleep(100);
            }
//            System.exit(0);
        }

        System.getProperties().load(new FileInputStream(args[0]));

    }
}
