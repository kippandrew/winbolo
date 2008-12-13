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
 * Messages.java
 *
 * Created on 7 October 2003, 21:19
 */

package com.jbolo.bolo;

/** Messages for this player are stored here
 *
 * @provides bolo/messages.c
 * @author John Morrison
 */
public class Messages {

    /** Message time between scrolling */
    public static int MESSAGE_SCROLL_TIME = 4;
    /** Message Type Neswire */
    public static int NEWSWIRE_MESSAGE = 0;
    /** Message Type Assistant */
    public static int ASSISTANT_MESSAGE = 1;
    /** Message Type AI */
    public static int AI_MESSAGE = 2;
    /** Message Type Network */
    public static int NETWORK_MESSAGE = 3;
    /** Message Type Networl status */
    public static int NETWORK_STATUS_MESSAGE = 4;
    /** Empty String used for blank messages rather then creating new strings */
    public static String EMPTY_STRING = "";
    /** The amount to add to a player number to get its offset */
    public static int PLAYER_MESSAGE_OFFSET = 5;
    
    /** Width of the message buffer on the screen */    
    private static final int MESSAGE_WIDTH = 68;
    /** A space character */    
    private static final char SPACE = ' ';
    
    /** Setting for showing newswrite messages */    
    private boolean showNewswire;
    
    /** Setting for showing LGM messages */    
    private boolean showAssistant;
    
    /** Setting for showing AI messages */    
    private boolean showAI;
    
    /** Setting for showing network message */    
    private boolean showNetwork;
    
    /** Setting for showing network status messages */    
    private boolean showNetStatus;
    
    /** The top line of the message ticker */    
    private String topLine;
    
    /** The bottom line of the message ticker */    
    private String bottomLine;
    
    /** Letters waiting to scroll onto the top line of the message ticker */    
    private StringBuffer waitingTop;
    
    /** Letters waiting to scroll onto the bottom line of the message ticker */    
    private StringBuffer waitingBottom;
    
    /** The last type of message received */    
    private int lastFrom;
    
    /** Game World these messages belong too */
    private GameWorld gw;
    
    
    /** Creates a new instance of Messages 
     * @param gw GameWorld these messages belong to
     */
    public Messages(GameWorld gw) {
        int count = 0;
	this.gw = gw;
        
        showNewswire = true;
        showAssistant = true;
        showAI = true;
        showNetwork = false;
        showNetStatus = false;
        topLine = new String();
        bottomLine = new String();
        while (count <= MESSAGE_WIDTH) {
            topLine += SPACE;
            bottomLine += SPACE;
            count++;
        }
        
        waitingTop = new StringBuffer(MESSAGE_WIDTH);
        waitingBottom = new StringBuffer(MESSAGE_WIDTH);
	/* Value so that first emssage will show up */
        lastFrom = 10000;
    }
    
    /**
     * @param msgType The type of message received. Eg player 1, newswire
     * @param top THe top line of the message
     * @param bottom The bottom line of the message
     */    
    public void add(int msgType, String top, String bottom) {
	boolean show  = true; // Do we want to show this message
	
	if (msgType == NEWSWIRE_MESSAGE && showNewswire == false) {
	    show = false;
	} else if (msgType == ASSISTANT_MESSAGE && showAssistant == false) {
	    show = false;
	} else if (msgType == AI_MESSAGE && showAI == false) {
	    show = false;
	} else if (msgType == NETWORK_STATUS_MESSAGE && showNetStatus == false) {
	    show = false;
	} else if (msgType == NETWORK_MESSAGE && showNetwork == false) {
	    show = false;
	}

        
	if (show == true) {
	    if (msgType == lastFrom) {
		addToQueue(EMPTY_STRING, bottom);
	    } else {
		lastFrom = msgType;
		addToQueue(top, bottom);
	    }
	}
    }
    
    /** Called every game tick to update the message line. */    
    public void update() {
        if (waitingTop.length() > 0) {
            topLine = topLine.substring(1) + waitingTop.charAt(0);
            bottomLine = bottomLine.substring(1) + waitingBottom.charAt(0);
            waitingTop.deleteCharAt(0);
            waitingBottom.deleteCharAt(0);
	    gw.getFrontEndDraw().feWriteMessages(topLine, bottomLine);

	}
    }
    
    /** Returns the top line of the message ticker
     * @return Top line of the message ticker
     */    
    public String getMessageTopLine() {
        return topLine;
    }
    
    /** Returns the bottom line of the message ticker
     * @return Bottom line of the message ticker
     */    
    public String getMessageBottomLine() {
        return bottomLine;
    }
    
    /** Sets whether newswire messages are shown.
     * @param isShown Value to set.
     */    
    public void setNewswire(boolean isShown) {
        showNewswire = isShown;
    }
    
    /** Sets whether LGM messages are shown.
     * @param isShown Value to set.
     */    
    public void setAssistant(boolean isShown) {
        showAssistant = isShown;
    }
    
    /** Sets whether AI messages are shown.
     * @param isShown Value to set.
     */    
    public void setAI(boolean isShown) {
        showAI = isShown;
    }
    
    /** Sets whether network messages are shown.
     * @param isShown Value to set.
     */    
    public void setNetwork(boolean isShown) {
        showNetwork = isShown;
    }
    
    /** Sets whether network status messages are shown.
     * @param isShown Value to set.
     */    
    public void setNetStatus(boolean isShown) {
        showNetStatus = isShown;
    }
    
    public void getNewMessage(boolean isShown) {
        // FIXME
    }
    
    /** Returns if newswire messages are shown
     * @return If meesages are shown
     */    
    public boolean getNewswire() {
        return showNewswire;
    }
    
    /** Returns if LGM messages are shown
     * @return If meesages are shown
     */    
    public boolean getAssistant() {
        return showAssistant;
    }
    
    /** Returns if AI messages are shown
     * @return If meesages are shown
     */    
    public boolean getAI() {
        return showAI;
    }
    
    /** Returns if network messages are shown
     * @return If meesages are shown
     */    
    public boolean getNetwork() {
        return showNetwork;
    }
    
    /** Returns if network status messages are shown
     * @return If meesages are shown
     */    
    public boolean getNetStatus() {
        return showNetStatus;
    }
    
    /** Adds the new message the internal queue of letters to show
     * @param top Top line of the message ticker to be added
     * @param bottom Bottom line of the message ticker to be added
     */    
    private void addToQueue(java.lang.String top, java.lang.String bottom) {
        StringBuffer stop = new StringBuffer(top);
        StringBuffer sbottom = new StringBuffer(bottom);
        int lenTop = top.length();
        int lenBottom = bottom.length();
        int longest = lenTop > lenBottom ? lenTop : lenBottom;
        int count = 0;
        
        while (count < longest) {
            if (count >= lenTop) {
                waitingTop.append(SPACE);
            } else {
                waitingTop.append(stop.charAt(count));
            }
            if (count >= lenBottom) {
                waitingBottom.append(SPACE);
            } else {
                waitingBottom.append(sbottom.charAt(count));
            }
            count++;
        }
	/* Add a space at the end */
	waitingTop.append(SPACE);
	waitingBottom.append(SPACE);
    }
}
