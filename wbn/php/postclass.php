<? class postClass {
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


var $link;
var $topic;
var $author;
var $time;
var $text;

function setup($link, $topic, $author, $time, $text) {
	$this->link= $link;
	$this->topic = $topic;
	$this->text = $text;
	$this->time = $time;
	$this->author = $author;
}

function getLink() {
	return $this->link;
}

function getTopic() {
	return $this->topic;
}

function getAuthor() {
	return $this->author;
}

function getTime() {
	return gmdate("D M d, Y g:i a", $this->time + (10 * 3600));
}

function getPost() {
	return $this->text;
}

}

?>
