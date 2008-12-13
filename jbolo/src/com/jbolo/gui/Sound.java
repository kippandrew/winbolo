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
 * Sound.java
 *
 * Created on December 30, 2003, 9:43 PM
 */

package com.jbolo.gui;

import java.applet.Applet;
import java.applet.AudioClip;
import java.net.URL;
import javax.sound.sampled.*;
import java.io.*;
import com.jbolo.bolo.SoundDist;



/** Provides sound output
 * @author John Morrison
 */
public class Sound implements FrontEndSound {
    
    /** The base path for all sounds to be loaded from */
    private String baseUrlPath;
    /** Are we playing any sounds at all */
    private boolean isPlayable;
    /** Is sounds enabled in the menu */
    private boolean isEnabledInMenu;
    /** Sound Effect Audio Clip */
    private Clip clipBigExplosionFar;
    /** Sound Effect Audio Clip */
    private Clip clipBigExplosionNear;
    /** Sound Effect Audio Clip */
    private Clip clipBubbles;
    /** Sound Effect Audio Clip */
    private Clip clipFarmingTreeFar;
    /** Sound Effect Audio Clip */
    private Clip clipFarmingTreeNear;
    /** Sound Effect Audio Clip */
    private Clip clipHitTankFar;
    /** Sound Effect Audio Clip */
    private Clip clipHitTankNear;
    /** Sound Effect Audio Clip */
    private Clip clipHitTankSelf;
    /** Sound Effect Audio Clip */
    private Clip clipManBuildingFar;
    /** Sound Effect Audio Clip */
    private Clip clipManBuildingNear;
    /** Sound Effect Audio Clip */
    private Clip clipManDyingFar;
    /** Sound Effect Audio Clip */
    private Clip clipManDyingNear;
    /** Sound Effect Audio Clip */
    private Clip clipManLayingMineNear;
    /** Sound Effect Audio Clip */
    private Clip clipMineExplosionFar;
    /** Sound Effect Audio Clip */
    private Clip clipMineExplosionNear;
    /** Sound Effect Audio Clip */
    private Clip clipShootFar;
    /** Sound Effect Audio Clip */
    private Clip clipShootNear;
    /** Sound Effect Audio Clip */
    private Clip clipShootSelf;
    /** Sound Effect Audio Clip */
    private Clip clipShotBuildingFar;
    /** Sound Effect Audio Clip */
    private Clip clipShotBuildingNear;
    /** Sound Effect Audio Clip */
    private Clip clipShotTreeFar;
    /** Sound Effect Audio Clip */
    private Clip clipShotTreeNear;
    /** Sound Effect Audio Clip */
    private Clip clipTankSinkingFar;
    /** Sound Effect Audio Clip */
    private Clip clipTankSinkingNear;
    
    private SoundEffect test;
    
    int test2;
    
    /** Creates a new instance of Sound */
    public Sound() {
	isPlayable = false;
	isEnabledInMenu = false;
	baseUrlPath = "file:///c:/jbolo/sounds/";
	//	clip = Applet.newAudioClip(url);
	//	clip.loop();
	//	    clip = applet.getAudioClip(url);
	//	    clip.play();
    }
    
    /** Gets if sounds are enabled
     * @return If sounds are enabled
     */
    public boolean canPlay() {
	return isPlayable;
    }
    
    /**
     * Gets if sounds are enabled in the menu
     * @return If the sound is enabled
     */
    public boolean isEnabled() {
	return isEnabledInMenu;
    }
    
    /**
     * Sets whether sounds are enabled in the menus
     * @param value Value to set
     */
    public void setEnabled(boolean value) {
	isEnabledInMenu = value;
    }
    
    /**
     * Sets up and loads all sounds
     * @return If all were loaded successfully or not 
     */
    public boolean setup() {
	isPlayable = true;
	System.err.print("Loading sounds...");
	try {
        test = new SoundEffect(new URL(this.baseUrlPath + "shooting_self.wav"));

            clipBigExplosionFar = loadSound("big_explosion_far.wav");
	    clipBigExplosionNear = loadSound("big_explosion_near.wav");
	    clipBubbles = loadSound("bubbles.wav");
	    clipFarmingTreeFar = loadSound("farming_tree_far.wav");
	    clipFarmingTreeNear = loadSound("farming_tree_near.wav");
	    clipHitTankFar = loadSound("hit_tank_far.wav");
	    clipHitTankNear = loadSound("hit_tank_near.wav");
	    clipHitTankSelf = loadSound("hit_tank_self.wav");
	    clipManBuildingFar = loadSound("man_building_far.wav");
	    clipManBuildingNear = loadSound("man_building_near.wav");
	    clipManDyingFar = loadSound("man_dying_far.wav");
	    clipManDyingNear = loadSound("man_dying_near.wav");
	    clipManLayingMineNear = loadSound("man_lay_mine_near.wav");
	    clipMineExplosionFar = loadSound("mine_explosion_far.wav");
	    clipMineExplosionNear = loadSound("mine_explosion_near.wav");
	    clipShootFar = loadSound("shooting_far.wav");
	    clipShootNear = loadSound("shooting_near.wav");
	    clipShootSelf = loadSound("shooting_self.wav");
	    clipShotBuildingFar = loadSound("shot_building_far.wav");
	    clipShotBuildingNear = loadSound("shot_building_near.wav");
	    clipShotTreeFar = loadSound("shot_tree_far.wav");
	    clipShotTreeNear = loadSound("shot_tree_near.wav");
	    clipTankSinkingFar = loadSound("tank_sinking_near.wav");
	    clipTankSinkingNear = loadSound("tank_sinking_far.wav");
//	    test = loadSound2("shooting_self.wav");
	} catch (Exception e) {
	    e.printStackTrace();
	    isPlayable = false;
	}
	isEnabledInMenu = isPlayable;
	System.err.println("Done");
	return isPlayable;
    }
    
    /** Loads a sound effect
     * @return Clip containing the loaded sound
     * @throw Exception on load error
     * @param fileName Filename of the sound to load
     * @throws Exception On load error
     */
    private Clip loadSound(String fileName) throws Exception {
	Clip returnValue; // Value to return
        Object currentSound;
        
        System.err.print(".");
        
        
        currentSound = AudioSystem.getAudioInputStream(new URL(this.baseUrlPath + fileName));

        AudioInputStream stream = (AudioInputStream) currentSound;
        AudioFormat format = stream.getFormat();

        /**
         * we can't yet open the device for ALAW/ULAW playback,
         * convert ALAW/ULAW to PCM
         */
        if ((format.getEncoding() == AudioFormat.Encoding.ULAW) ||
            (format.getEncoding() == AudioFormat.Encoding.ALAW)) 
        {
            AudioFormat tmp = new AudioFormat(
                                      AudioFormat.Encoding.PCM_SIGNED, 
                                      format.getSampleRate(),
                                      format.getSampleSizeInBits() * 2,
                                      format.getChannels(),
                                      format.getFrameSize() * 2,
                                      format.getFrameRate(),
                                      true);
            stream = AudioSystem.getAudioInputStream(tmp, stream);
            format = tmp;
        }
        DataLine.Info info = new DataLine.Info(
                                  Clip.class, 
                                  stream.getFormat(), 
                                  ((int) stream.getFrameLength() *
                                      format.getFrameSize()));

        Clip clip = (Clip) AudioSystem.getLine(info);
        clip.open(stream);
        
        return clip;
    
    
    }
    
    private SoundEffect loadSound2(String fileName) throws Exception {
    	return new SoundEffect(new URL(baseUrlPath + fileName));
    }
    
    /**
     * Plays a sound effect of type effectType
     * @param effectType The sound effect type to play
     */
    public void fePlaySound(int effectType) {
	if (isPlayable == true && isEnabledInMenu == true) {
/*	    switch (effectType) {
		case SoundDist.BIG_EXPLOSION_FAR:
		    clipBigExplosionFar.play();
		    break;
		case SoundDist.BIG_EXPLOSION_NEAR:
		    clipBigExplosionNear.play();
		    break;
		case SoundDist.BUBBLES:
		    clipBubbles.play();
		    break;
		case SoundDist.FARMING_TREE_FAR:
		    clipFarmingTreeFar.play();
		    break;
		case SoundDist.FARMING_TREE_NEAR:
		    clipFarmingTreeNear.play();
		    break;
		case SoundDist.HIT_TANK_FAR:
		    clipHitTankFar.play();
		    break;
		case SoundDist.HIT_TANK_NEAR:
		    
		    clipHitTankNear.play();
		    break;
		case SoundDist.HIT_TANK_SELF:
		    clipHitTankSelf.play();
		    break;
		case SoundDist.MAN_BUILDING_FAR:
		    clipManBuildingFar.play();
		    break;
		case SoundDist.MAN_BUILDING_NEAR:
		    clipManBuildingNear.play();
		    break;
		case SoundDist.MAN_DYING_FAR:
		    clipManDyingFar.play();
		    break;
		case SoundDist.MAN_DYING_NEAR:
		    clipManDyingNear.play();
		    break;
		case SoundDist.MAN_LAYING_MINE_NEAR:
		    clipManLayingMineNear.play();
		    break;
		case SoundDist.MINE_EXPLOSION_FAR:
		    clipMineExplosionFar.play();
		    break;
		case SoundDist.MINE_EXPLOSION_NEAR:
		    clipMineExplosionNear.play();
		    break;
		case SoundDist.SHOOT_FAR:
		    clipShootFar.play();
		    break;
		case SoundDist.SHOOT_NEAR:
		    clipShootNear.play();
		    break;
		case SoundDist.SHOOT_SELF:
		    clipShootSelf.play();
		    break;
		case SoundDist.SHOT_BUILDING_FAR:
		    clipShotBuildingFar.play();
		    break;
		case SoundDist.SHOT_BUILDING_NEAR:
		    clipShotBuildingNear.play();
		    break;
		case SoundDist.SHOT_TREE_FAR:
		    clipShotTreeFar.play();
		    break;
		case SoundDist.SHOT_TREE_NEAR:
		    clipShotTreeNear.play();
		    break;
		case SoundDist.TANK_SINK_FAR:
		    clipTankSinkingFar.play();
		    break;
		case SoundDist.TANK_SINK_NEAR:
		    clipTankSinkingNear.play();
		default:
		    System.err.println("Sound error: " + effectType);
		    break;
		    
	    } */
	}
    }
}
