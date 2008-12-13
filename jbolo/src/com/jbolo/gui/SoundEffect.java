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
 * SoundEffect.java
 *
 * Created on January 3, 2004, 11:41 PM
 */

package com.jbolo.gui;
import java.io.*;
import javax.sound.sampled.*;
import java.net.URL;

/**
 *
 * @author  john
 */
public class SoundEffect {
    /** The clip to play */
    private Clip clip;
    /** The file URL to load the file from */
    private URL fileUrl;
    
    AudioInputStream clone;
    
    private byte[] data;
    /** The input stream for the sound */
    private AudioInputStream stream;
    /** The format for the sound */
    private AudioFormat format;
    
    private int length;
    private DataLine.Info info;
    
    /** Creates a new instance of SoundEffect 
     * @param url File to load
     */
    public SoundEffect(URL url) throws Exception {
	fileUrl = url;
        System.err.println(fileUrl.getPath());
        length = getLength();
        data = new byte[length];
	loadClipData();
    }
    
    public void play() {
        clip.start();
        try {
            loadClipData();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
/*        Clip clip;
        AudioInputStream stream;
        ByteArrayInputStream bis = new ByteArrayInputStream(data);

        try {
        stream = AudioSystem.getAudioInputStream(bis);
	clip = (Clip) AudioSystem.getLine(info);
	clip.open(stream);
        clip.start();
        } catch (Exception e) {
            e.printStackTrace();
        } */
    }
    
    private int getLength() throws Exception {
        int returnValue = 0;
        int val;
      
        FileInputStream fis = new FileInputStream(new File(fileUrl.getPath()));
        val = fis.read();
        while (val != -1) {
            returnValue++;
            val = fis.read();
        }
      
        fis.close();
        return returnValue;
    }
    
    /**
     * Loads our clip from fileUrl
     * @throw Exception on load error
     */
    private void loadClipData() throws Exception {
	AudioFormat tmp;
  //      byte[] data2;
	stream = AudioSystem.getAudioInputStream(fileUrl);
	format = stream.getFormat();

	/**
	 * we can't yet open the device for ALAW/ULAW playback,
         * convert ALAW/ULAW to PCM
         */
        if ((format.getEncoding() == AudioFormat.Encoding.ULAW) || (format.getEncoding() == AudioFormat.Encoding.ALAW)) {
	    tmp = new AudioFormat(AudioFormat.Encoding.PCM_SIGNED, format.getSampleRate(), format.getSampleSizeInBits() * 2, format.getChannels(), format.getFrameSize() * 2, format.getFrameRate(), true);
	    stream = AudioSystem.getAudioInputStream(tmp, stream);
            format = tmp;
	}
	format = stream.getFormat();
	info = new DataLine.Info(Clip.class, stream.getFormat(), ((int) stream.getFrameLength() * format.getFrameSize()));	
/*        int len = stream.read(data);
        int count = 0;
        data2 = new byte[len];
        while (count < len) {
            data2[count] = data[count];
            count++;
        }
        data = data2;
        
        System.err.println("Read " + len); */
	clip = (Clip) AudioSystem.getLine(info);
	clip.open(stream);
//        return clip;
    }
}
