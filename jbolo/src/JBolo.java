/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * JBolo.java
 *
 * Created on January 3, 2004, 10:40 AM
 */

import com.jbolo.gui.MapLoad;


import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.network.Network;
import com.jbolo.gui.*;


/**
 *
 * @author  John Morrison
 */
public class JBolo {
    
	// Network test
    public static void main2(String args[]) {
        GameWorld gw = new GameWorld(0 , false, null, null, null);
        Network n = new Network(gw);
        
        if (n.setup(Network.NET_GAME_TYPE_UDP, 50000, "127.0.0.1", 27500, "Elvis", null, false, "tracker.winbolo.com", 50000, false, false, false, "") == true) {
            System.err.println("Got through join. Map Name was " + gw.getMapName());
        } else {
            System.err.println("Unable to join game.");
        } 
        
        int count = 0;
        while (count < 20) {
            n.check();
            try {
                Thread.sleep(100);
            } catch (Exception e) {
                
            }
            count++;
        }
        System.err.println("Shutting down...");
        n.shutdown();
    }
    
    public static void main3(String args[]) {
        Skin s = new Skin();
        
        s.test();
    }
    
    
    public static void main(String args[]) {
        
        if (args.length == 1) {
            if (args[0].equals("-test") == true) {
                main2(args);
                return;
            } else if (args[0].equals("-skin") == true) {
                main3(args);
                return;
            }
        }
        
        MapLoad ml = new MapLoad(args);


		ml.gw.screenUpdate(0);
    }    
}
