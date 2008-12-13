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
 * SystemInfo.java
 *
 * Created on January 2, 2004, 11:18 AM
 */

package com.jbolo.gui.dialog;

import com.jbolo.gui.SysTimers;
import java.lang.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.text.DecimalFormat;


/** SystemInfoDialog provides the system information dialog window.
 * @author John Morrison
 */
public class SystemInfoDialog extends InfoDialog {
    /** Label for displaying frame rate information */
    private Label frameRate;
    /** Label for displaying draw time percentage information */
    private Label draw;
    /** Label for displaying simulation time percentage information */
    private Label sim;
    /** Label for displaying network time percentage information */
    private Label network;
    /** Label for displaying AI time information */
    private Label ai;
    /** Label for displaying total time information */
    private Label total;
    /** The CPU Timers */
    private SysTimers sysTimers;
    /** The formatting object for percentages */
    private DecimalFormat df;
    /** The formatting object for kilobytes */
    private DecimalFormat df2;
    
    /** Amount of memory used by the JVM */
    private Label usedMemory;
    /** Free memory used by the JVM */
    private Label freeMemory;
    /** Total memory availble to the JVM */
    private Label totalMemory;
    
    /** Creates a new instance of SystemInfo
     * @param owner Owner of the dialog window
     * @param timers Timers interface
     */
    public SystemInfoDialog(Frame owner, SysTimers timers) {
	super(owner, "System Information");
	Button b;
	
	sysTimers = timers;
	frameRate = new Label("0");
	draw = new Label("0%");
	sim = new Label("0%");
	network = new Label("0%");
	ai = new Label("0%");
	draw = new Label("0%");
	total = new Label("0%");
	usedMemory = new Label("");
	freeMemory = new Label("");
	totalMemory = new Label("");
	
	add(new Label("CPU"));
	add(new Label(""));
	add(new Label("Simulation modeling:"));
	add(sim);
	add(new Label("Communications processing:"));
	add(network);
	add(new Label("Graphics display:"));
	add(draw);
	add(new Label("AI tank control processing:"));
	add(ai);
	add(new Label("Total:"));
	add(total);
	add(new Label("JVM"));
	b = new Button("GC");
	b.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent ae) { System.gc();}});
	
	add(b);
	add(new Label("Used memory:"));
	add(usedMemory);
	add(new Label("Free memory:"));
	add(freeMemory);
	add(new Label("Total memory"));
	add(totalMemory);
	add(new Label("Graphics frames per second:"));
	add(frameRate);
	add(new Label(""));
	df = new DecimalFormat("#0.00%");
	df2 = new DecimalFormat("#,###K");
	update();
	pack();
    }
    
    /**
     * Converts the number of number of milliseconds into a percentage of time
     * @param milli The number of milliseconds to convert
     * @return String containing the percentage
     */
    private String convertMilliToPercentString(double milli) {
	double val;
	StringBuffer result  = new StringBuffer();
	java.text.FieldPosition fp = new java.text.FieldPosition(0);
	val = milli / SECOND;
	
	result = df.format(val, result, fp);
	return result.toString();
    }
    
    /**
     * Converts the number of bytes into a formatted string containgin the number of kilobytes
     * @param val The value in bytes to convert
     * @return String containing the byte value as number of kilobytes
     */
    public String convertBytesToString(long val) {
	StringBuffer result = new StringBuffer();
	java.text.FieldPosition fp = new java.text.FieldPosition(0);
	
	result = df2.format(val/1024, result, fp);
	return result.toString();
    }
    
    /**
     * Update is called once a second to update the visible counts
     */
    void update() {
	double time;
	double totalTime;
	Runtime rt = Runtime.getRuntime();
	
	time = sysTimers.getSimulationModellingTime();
	totalTime = time;
	sim.setText(convertMilliToPercentString(time));
	time = sysTimers.getNetworkTime();
	totalTime += time;
	network.setText(convertMilliToPercentString(time));
	time = sysTimers.getDrawingTime();
	totalTime += time;
	draw.setText(convertMilliToPercentString(time));
	time = sysTimers.getAITime();
	totalTime += time;
	ai.setText(convertMilliToPercentString(time));
	total.setText(convertMilliToPercentString(totalTime));
	frameRate.setText(Integer.toString(sysTimers.getFrameRate()));
	
	freeMemory.setText(convertBytesToString(rt.freeMemory()));
	totalMemory.setText(convertBytesToString(rt.totalMemory()));
	usedMemory.setText(convertBytesToString(rt.totalMemory() - rt.freeMemory()));
    }
}
