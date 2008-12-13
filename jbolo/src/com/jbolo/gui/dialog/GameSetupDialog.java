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
 * GameSetupDialog.java
 *
 * Created on January 11, 2004, 4:20 PM
 */

package com.jbolo.gui.dialog;
import java.awt.*;
import java.awt.event.*;
import com.jbolo.bolo.TestMapLoad;


/**
 *
 * @author  John Morrison
 */
public class GameSetupDialog extends CentredDialog implements ActionListener {
    /** Text to prefix map name in the dialog */
    private static String SELECTED_MAP = "Selected Map: ";
    
    /** The label containing the map name */
    private Label lblMapName;
    /** The Open game option */
    private Checkbox chkOpenGame;
    private Checkbox chkTournamentGame;
    private Checkbox chkStrictGame;
    private Checkbox chkAllowHiddenMines;
    private Checkbox chkAllowComputerTanks;
    private Checkbox chkAdvComputerTanks;
    private Checkbox chkPassword;
    private TextField txtPassword;
    private Checkbox chkStartDelay;
    private NumberTextField txtStartDelay;
    private Checkbox chkGameLimit;
    private NumberTextField txtGameLimit;
    private String mapName;
    private String mapNamePath;
    
    /** Creates a new instance of GameSetupDialog */
    public GameSetupDialog(String mapNameDisplay, String mapNamePath) {
	super(new Frame(), "Game Setup", true);
	Panel c;
	Button b;
	CheckboxGroup cbg = new CheckboxGroup();

	mapName = mapNameDisplay;
	this.mapNamePath = mapNamePath;
	setResizable(false);
	addWindowListener(new WindowAdapter() {public void windowClosing(WindowEvent e) {System.exit(0);}});
	setLayout(new GridLayout(0, 1, 5, 5));
	c = new Panel();
	c.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
	c.add(new Label("Please select the options you want for your new game"));
	add(c);
	c = new Panel();
	c.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
	b = new Button("Choose Map");
	b.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { loadMap() ;}});	

	c.add(b);
	lblMapName = new Label(SELECTED_MAP + mapName);
	c.add(lblMapName);
	add(c);
	c = new Panel();
	c.setLayout(new GridLayout(0,1,0,0));
	chkOpenGame = new Checkbox("Open Game (each new tank comes pre-armed)", true, cbg);
	chkTournamentGame = new Checkbox("Tournament (free ammunition early in the game)", false, cbg);
	chkStrictGame = new Checkbox("Strict Tournament (no free ammunition at all)", false, cbg);
	c.add(chkOpenGame);
	c.add(chkTournamentGame);
	c.add(chkStrictGame);
	add(c);
	c = new Panel();
	c.setLayout(new GridLayout(0, 2, 5, 5));
	chkAllowHiddenMines = new Checkbox("Allow Hidden Mines", false);
	c.add(chkAllowHiddenMines);
	c.add(new Label(""));
	chkAdvComputerTanks = new Checkbox("and give them an advantage", false);
	chkAdvComputerTanks.setEnabled(false);
	chkAllowComputerTanks = new Checkbox("Allow Computer Tanks", false);
	chkAllowComputerTanks.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { Checkbox cb = (Checkbox) ie.getSource();  chkAdvComputerTanks.setEnabled(cb.getState()); if (cb.getState() == false) { chkAdvComputerTanks.setState(false);}  }}  );
	c.add(chkAllowComputerTanks);
	c.add(chkAdvComputerTanks);
	txtPassword = new TextField(20);
	txtPassword.setEnabled(false);
	txtPassword.setEchoChar('*');
	chkPassword = new Checkbox("Game Password", false);
	chkPassword.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { Checkbox cb = (Checkbox) ie.getSource();  txtPassword.setEnabled(cb.getState()); }}  );
	
	c.add(chkPassword);
	c.add(txtPassword);
	add(c);
	c = new Panel();
	c.setLayout(new GridLayout(0, 3, 2 ,2));
	txtStartDelay = new NumberTextField(2);
	txtStartDelay.setEnabled(false);
	chkStartDelay = new Checkbox("Game start delay", false);
	chkStartDelay.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { Checkbox cb = (Checkbox) ie.getSource();  txtStartDelay.setEnabled(cb.getState()); }}  );
	c.add(chkStartDelay);
	c.add(txtStartDelay);
	c.add(new Label("seconds"));
	txtGameLimit= new NumberTextField(3);
	txtGameLimit.setEnabled(false);
	chkGameLimit = new Checkbox("Game time limit", false);
	chkGameLimit.addItemListener(new ItemListener(){ public void itemStateChanged(ItemEvent ie) { Checkbox cb = (Checkbox) ie.getSource();  txtGameLimit.setEnabled(cb.getState()); }}  );
	c.add(chkGameLimit);
	c.add(txtGameLimit);
	c.add(new Label("minutes"));
	add(c);
	c = new Panel();
	c.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 0));
	b = new Button("OK");
	b.addActionListener(this);
	c.add(b);
	b = new Button("Cancel");
	b.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { System.exit(0);}});	
	
	c.add(b);
	add(c);
	pack();
	centre();
	setVisible(true);
    }
    
    private void loadMap() {
	FileDialog fd;
	TestMapLoad gw;
	
	fd = new FileDialog(new Frame(), "Choose Map...");
	fd.setVisible(true);
	if (fd.getDirectory() != null && fd.getFile() != null) {
	    gw = new TestMapLoad();
	    if (gw.loadMap(fd.getDirectory() + fd.getFile()) == true) {
		mapNamePath = fd.getDirectory() + fd.getFile();
		lblMapName.setText(SELECTED_MAP + gw.getMapName());
	    } else {
		new MessageBox(this, "Error Loading Map File", "JBolo");
	    }
	    gw = null;
	}
    }
    
    
    public static void main(String args[]) {
	new GameSetupDialog("Everard Island (In built)", "");
    }
    
    public void actionPerformed(java.awt.event.ActionEvent actionEvent) {

    }
    
}
