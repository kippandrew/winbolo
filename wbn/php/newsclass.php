<? 
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


define('IN_PHPBB', true);
include("$WWW_ROOT/forums/includes/bbcode.php");
define('IN_PHPBB', false);

class newsItemClass {
var $subject;
var $poster;
var $time;
var $text;
var $bbcodeUid;
var $postId;

function setup($subject, $poster, $time, $text, $bbcodeUid, $postId) {
	$this->subject = $subject;
	$this->poster = $poster;
	$this->time = $time;
	$this->text = $text;
	$this->postId = $postId;
	$this->bbcodeUid = $bbcodeUid;
}

function getSubject() {
	return $this->subject;
}

function getPoster() {
	return $this->poster;
}

function getTime() {
	return gmdate("D M d, Y g:i a", $this->time + (10 * 3600));
//	return $this->time;
}

function getRawText() {
	return $this->text;
}

function getPostId() {
	return $this->postId;
}


function getPosterUrl() {
	return urlencode($this->poster);
}

function getText() {
	$text = ereg_replace ("\n", "<br>", $this->text);
	$text = bbencode_second_pass($text, $this->bbcodeUid);
	return $text;
}

}
?>
