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


package com.jbolo.gui;

import com.jbolo.bolo.GameWorld;
import com.jbolo.bolo.GameClient;
import com.jbolo.bolo.FrontEndClient;
import com.jbolo.bolo.TankButtons;
import com.jbolo.bolo.GameType;
import com.jbolo.bolo.network.Network;
import com.jbolo.gui.dialog.*;
import java.awt.*;
import java.applet.*;
import java.util.*;
import java.awt.event.*;
import java.applet.Applet;
import java.awt.image.*;

public class MapLoad extends Applet implements KeyListener, GraphicsInterface, FocusListener, SysTimers, FrontEndClient {
    public GameWorld gw;
    static MapLoad _me = null;
    public Draw draw;
    private FrameTimer ft;
    private GameTimer gt;
    private TankButtons tb;
    private Sound sound;
    private BoloCursor cursor;
    public Window w;
    private Frame f;
    private SystemInfoDialog dialogSysInfo;
    private GameInfoDialog dialogGameInfo;
    private SendMessageDialog dialogSendMessage;
    private MainViewCanvas c;
    private CheckboxMenuItem[] playerMenuItem;    
    private CheckboxMenuItem mShowGunsight;
    private CheckboxMenuItem mAutoScroll;
    private MenuItem mRequestAlliance;
    private MenuItem mLeaveAlliance;

    
    public MapLoad(String args[]) {
	_me = this;
	
	f = new Frame("JBolo");
	f.addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {gw.shutdown();  System.exit(0);}
	});
	sound = new Sound();
	
	createMenus();
	
	System.err.println("");

        Skin.GetInstance().loadSkin(BoloPreferences.GetInstance().getPrivateProfileString("GAME OPTIONS", "SKIN", ""));
        
	try {
	    draw = new Draw(this, 1);
	    sound.setup();
	} catch (Exception e) {
	    System.exit(0);
	}
	System.err.print("Creating Game world...");
	gw = new GameWorld(GameType.OPEN_GAME_TYPE, true, draw, sound, this);


	cursor = new BoloCursor(f, draw, gw);

	c = new MainViewCanvas(gw, draw, cursor);
	c.setSize(515+3,325+42);

	f.add(c);
	f.pack();
	f.setSize(515+3,325+42);
	f.setMaximizedBounds(new Rectangle(515,325+42));
	f.addFocusListener(this);
	c.addMouseListener(cursor);
	c.addMouseMotionListener(cursor);

	f.setVisible(true);
	f.addKeyListener((KeyListener) this);
	c.addKeyListener((KeyListener) this);
	tb = new TankButtons();
	ft = new FrameTimer(gw, draw, 30);
	gt = new GameTimer(gw, 20, tb);
	dialogSysInfo = new SystemInfoDialog(f, this);
	dialogGameInfo = new GameInfoDialog(f, (GameClient) gw);
	dialogSendMessage = new SendMessageDialog(f, gw);

        if (args.length == 0) {
            System.err.print("done\nLoading map file...");
            //gw.read("/Users/john/jbolo/map.map");
            if (gw.read("c:/jbolo/map.map") == false) {
                System.exit(0);
            }
            System.err.println("done.");
            draw.redrawAll((Graphics2D) c.getGraphics(), gw);
        } else {
            if (gw.joinGame(Network.NET_GAME_TYPE_UDP, 50000, "127.0.0.1", 27500, "Elvis", null, false, "tracker.winbolo.com", 27500, false, false, false, "") == false) {
               System.err.println("unable to join network game");
               System.exit(0);
            }
        }
        
        
        
	try {
	    Thread.currentThread().sleep(500);
	} catch (Exception e) {
	}
	ft.start();
	gt.start();
    }
    
    private MenuItem createMenuItem(Menu m, String name, boolean enabled) {
	MenuItem mi = new MenuItem(name);
	mi.setEnabled(enabled);
	mi.setActionCommand(m.getLabel() + "-" + name);
	m.add(mi);
	
	return mi;
    }
    
    private CheckboxMenuItem createCheckedMenuItem(Menu m, String name, boolean enabled, boolean checked) {
	CheckboxMenuItem mi = new CheckboxMenuItem(name);
	mi.setEnabled(enabled);
	mi.setState(checked);
	
	mi.setActionCommand(m.getLabel() + "-" + name);
	m.add(mi);
	return mi;
    }
    
    private void createMenus() {
	MenuBar mb = new MenuBar();
	Menu m;
	Menu sm;
	MenuItem mi;
	CheckboxMenuItem cmi;
	
	playerMenuItem = new CheckboxMenuItem[16];
	
	/* File Menu */
	m = new Menu("File", false);
	createMenuItem(m, "New", false);
	mi = createMenuItem(m, "Save Map", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { FileDialog fd = new FileDialog(f, "Save Map...", FileDialog.SAVE); fd.setVisible(true); if (fd.getDirectory() != null && fd.getFile() != null) { String fn = fd.getDirectory() + fd.getFile(); if (fn.toLowerCase().endsWith(".map") == false) { fn += ".map"; } if (gw.saveMap(fn) == false) { new MessageBox(f, "Error saving map. (Drive full?"); } }   }   });
	
	m.addSeparator();
	mi = createMenuItem(m, "Game Info", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { dialogGameInfo.setVisible(true);;}});
	mi = createMenuItem(m, "System Info", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { dialogSysInfo.setVisible(true);;}});
	createMenuItem(m, "Network Info", false);
	m.addSeparator();
	mi = createMenuItem(m, "Quit", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { System.exit(0);}});
	
	mb.add(m);
	/* Edit Menu */
	m = new Menu("Edit", false);
	createMenuItem(m, "Frame Rate", false);
	createMenuItem(m, "Window Size", false);
	m.addSeparator();
        mAutoScroll = createCheckedMenuItem(m, "Automatic Scrolling", true, false);
	mAutoScroll.addItemListener(new ItemListener() { public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource(); gw.setAutoScroll(cmi.getState()); }});
	mShowGunsight = createCheckedMenuItem(m, "Show Gunsight", true, true);
	mShowGunsight.addItemListener(new ItemListener() { public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource(); gw.setGunsightShowing(cmi.getState()); }});
	
	sm = new Menu("Message Sender Names", false);
	createCheckedMenuItem(sm, "Short", true, true);
	createCheckedMenuItem(sm, "Long", true, false);
	m.add(sm);
	sm = new Menu("Tank Labels", false);
	createCheckedMenuItem(sm, "None", true, true);
	createCheckedMenuItem(sm, "Short", true, false);
	createCheckedMenuItem(sm, "Long", true, false);
	createCheckedMenuItem(sm, "Don't label own tank", true, true);
	m.add(sm);
	createCheckedMenuItem(m, "Pillbox Labels", true, false);
	createCheckedMenuItem(m, "Refuelling Base Labels", true, false);
	m.addSeparator();
	createCheckedMenuItem(m, "Hide Main View", false, false);
	mb.add(m);
	/* J Bolo */
	m = new Menu("JBolo", false);
	createCheckedMenuItem(m, "Allow New Players", true, true);
	createCheckedMenuItem(m, "Allow Alliance Requests", true, true);
	mi = createMenuItem(m, "Set Keys", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { new KeySetupDialog(f, gw);  }});

	mi = createMenuItem(m, "Change Player Name", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { new SetPlayerNameDialog(f, true, gw, gw.getPlayerName());  }});
	
	m.addSeparator();
	createCheckedMenuItem(m, "Sound Effects", sound.canPlay(), sound.isEnabled());
	createCheckedMenuItem(m, "Background Sound", false, true);
	m.addSeparator();
	cmi = createCheckedMenuItem(m, "Newsire Messages", true, true);
	cmi.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.showNewswireMessages(cmi.getState()); }});
	cmi = createCheckedMenuItem(m, "Assistant Messages", true, true);
	cmi.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.showAssistantMessages(cmi.getState()); }});
	cmi = createCheckedMenuItem(m, "AI Brain Messages", true, true);
	cmi.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.showBrainMessages(cmi.getState()); }});
	cmi = createCheckedMenuItem(m, "Network Status Messages", true, true);
	cmi.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.showNetworkMessages(cmi.getState()); }});
	cmi = createCheckedMenuItem(m, "Network Debug Messages", true, false);
	cmi.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.showDebugMessages(cmi.getState()); }});
	m.addSeparator();
	mRequestAlliance = createMenuItem(m, "Request Alliance", false);
	mLeaveAlliance = createMenuItem(m, "Leave Alliance", false);
	mb.add(m);
	/* Players */
	m = new Menu("Players", false);
	mi = createMenuItem(m, "Send Message", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { dialogSendMessage.setVisible(true); }});
	
	
	
	m.addSeparator();
	mi = createMenuItem(m, "Select All", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { gw.selectAll();}});	
	mi = createMenuItem(m, "Select None", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { gw.selectNone();}});
	mi = createMenuItem(m, "Select Allies", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { gw.selectAllies();}});
	mi = createMenuItem(m, "Select Nearby Tanks", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { gw.selectNearby();}});
	m.addSeparator();

	/* Entries on the players Menu */
	playerMenuItem[0] = createCheckedMenuItem(m, "1", false, false);
	playerMenuItem[1] = createCheckedMenuItem(m, "2", false, false);
	playerMenuItem[2] = createCheckedMenuItem(m, "3", false, false);
	playerMenuItem[3] = createCheckedMenuItem(m, "4", false, false);
	playerMenuItem[4] = createCheckedMenuItem(m, "5", false, false);
	playerMenuItem[5] = createCheckedMenuItem(m, "6", false, false);
	playerMenuItem[6] = createCheckedMenuItem(m, "7", false, false);
	playerMenuItem[7] = createCheckedMenuItem(m, "8", false, false);
	playerMenuItem[8] = createCheckedMenuItem(m, "9", false, false);
	playerMenuItem[9] = createCheckedMenuItem(m, "10", false, false);
	playerMenuItem[10] = createCheckedMenuItem(m, "11", false, false);
	playerMenuItem[11] = createCheckedMenuItem(m, "12", false, false);
	playerMenuItem[12] = createCheckedMenuItem(m, "13", false, false);
	playerMenuItem[13] = createCheckedMenuItem(m, "14", false, false);
	playerMenuItem[14] = createCheckedMenuItem(m, "15", false, false);
	playerMenuItem[15] = createCheckedMenuItem(m, "15", false, false);
	playerMenuItem[0].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(0, cmi.getState()); }});
	playerMenuItem[1].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(1, cmi.getState()); }});
	playerMenuItem[2].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(2, cmi.getState()); }});
	playerMenuItem[3].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(3, cmi.getState()); }});
	playerMenuItem[4].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(4, cmi.getState()); }});
	playerMenuItem[5].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(5, cmi.getState()); }});
	playerMenuItem[6].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(6, cmi.getState()); }});
	playerMenuItem[7].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(7, cmi.getState()); }});
	playerMenuItem[8].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(8, cmi.getState()); }});
	playerMenuItem[9].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(9, cmi.getState()); }});
	playerMenuItem[10].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(10, cmi.getState()); }});
	playerMenuItem[11].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(11, cmi.getState()); }});
	playerMenuItem[12].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(12, cmi.getState()); }});
	playerMenuItem[13].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(13, cmi.getState()); }});
	playerMenuItem[14].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(14, cmi.getState()); }});
	playerMenuItem[15].addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { CheckboxMenuItem cmi = (CheckboxMenuItem) ie.getSource();  gw.setPlayerCheckedState(15, cmi.getState()); }});

	mb.add(m);
	m = new Menu("Brains", false);
	mb.add(m);
	m = new Menu("Help", false);
	mi = createMenuItem(m, "About", true);
	mi.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) {new MessageBox(f, "JBolo - Version 0.1\n(c) 2004 John Morrison", "JBolo");}});
	
	mb.setHelpMenu(m);
	f.setMenuBar(mb);
    }
    
    
    public TankButtons getTankButtons() {
	return tb;
    }
    
    
    public void paint(Graphics graphics) {
	super.paint(graphics);
    }
    
 /*   public static void main(String args[]) {
	MapLoad ml = new MapLoad();
	ml.f.add("Center", (Applet) ml);
	ml.init();
	
	ml.gw.screenUpdate(0);
    } */
    
    public void keyPressed(java.awt.event.KeyEvent event) {
	int keyCode = event.getKeyCode();
	
//	System.err.println("pressed");
	tb.keyDown(keyCode);
	if (keyCode ==  38) {
	    gw.screenUpdate(1);
	} else if (keyCode == 40) {
	    gw.screenUpdate(2);
	} else if (keyCode == 37) {
	    gw.screenUpdate(3);
	} else if (keyCode == 39) {
	    gw.screenUpdate(4);
	} else if (keyCode == 16) {
	    gw.tankLayMine();
	} else if (keyCode == 13) {
	    gw.tankView();
	} else if (keyCode == 61) {
	    // +
	    gw.changeGunsightRange(true);
	} else if (keyCode == 45) {
	    // -
	    gw.changeGunsightRange(false);
	}
    }
    
    public void keyReleased(java.awt.event.KeyEvent keyEvent) {
	tb.keyUp(keyEvent.getKeyCode());
//	System.err.println("released");
    }
    
    public void keyTyped(java.awt.event.KeyEvent keyEvent) {
//	System.err.println("typed");
    }
    
    
    /**
     * Gets the graphics instanace for this object
     * @return Graphics2D
     */
    public Graphics2D getGraphicsObject() {
	if (c == null) {
		return (Graphics2D) this.getGraphics();
	}
	
	return (Graphics2D) c.getGraphics();
    }
    
    public void focusGained(java.awt.event.FocusEvent focusEvent) {
	//System.err.println("Get Gained - " + focusEvent.getSource());
    }
    
    public void focusLost(java.awt.event.FocusEvent focusEvent) {
	//System.err.println("Get Lost - " + focusEvent.getSource());
    }

    /** 
     * Gets the current frame rate of the game
     * @return frame rate of the game
     */
    public int getFrameRate() {
	return ft.getFrameRate();
    }
    
    /** 
     * Gets the current Simulation modelling time of the game
     * @return time spent in milliseconds
     */ 
    public int getSimulationModellingTime() {
	return gt.getProcessingTime();
    }
    
    /** 
     * Gets the current network time of the game
     * @return time spent in milliseconds
     */
    public int getNetworkTime() {
	return 0;
    }
    
    /** 
     * Gets the current time spent in a brain 
     * @return time spent in milliseconds
     */
    public int getAITime() {
	return 0;
    }
    
    /** 
     * Gets the current time spent drawing in the game
     * @return time spent in milliseconds
     */
    public int getDrawingTime() {
	return ft.getProcessingTime();
    }
    
    
    /**
     * Clears a players entry from the players menu
     * @param playerNum player to clear (0-15)
     */
    public void feClearPlayer(int playerNum) {
	if (playerNum >= 0 && playerNum < GameClient.MAX_TANKS) {
	    playerMenuItem[playerNum].setEnabled(false);
	    playerMenuItem[playerNum].setState(false);
	    playerMenuItem[playerNum].setLabel(Integer.toString(playerNum+1));
	}
    }
    
    /**
     * Sets a players menu entry in the players menu
     * @param playerNum player to set (0-15)
     * @param name Name to set
     */
    public void feSetPlayer(int playerNum, String name) {
	if (playerNum >= 0 && playerNum < GameClient.MAX_TANKS) {
	    playerMenuItem[playerNum].setEnabled(true);
	    playerMenuItem[playerNum].setState(false);
	    playerMenuItem[playerNum].setLabel(name);
	}
    }

    
    /**
     * Sets the check state of a player
     * @param playerNum Player to set state for (0-15)
     * @param checkState State to set
     */
    public void feSetPlayerCheckState(int playerNum, boolean checkState) {
	if (playerNum >= 0 && playerNum < GameClient.MAX_TANKS) {
	    playerMenuItem[playerNum].setState(checkState);
	}
    }
    
    /**
     * Sets whether the gunsight menu item is checked or not
     *
     * @param isShown is the gunsight shown
     */
    public void feShowGunsight(boolean isShown) {
       mShowGunsight.setState(isShown); 
    }
    
    /**
     * Enables/Disables the request alliance menu ite
     *
     * @param enable Whether to enable it or not
     */
    public void feEnableRequestAllyMenu(boolean enable) {
        mRequestAlliance.setEnabled(enable);
    }
    
    /**
     * Enables/Disables the leave alliance menu ite
     *
     * @param enable Whether to enable it or not
     */
    public void feEnableLeaveAllyMenu(boolean enable) {
        mLeaveAlliance.setEnabled(enable);
    }
}
