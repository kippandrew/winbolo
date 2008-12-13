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

class mapCollectionCommentClass {
	var $time;
	var $comment;
	var $pid;
	var $name;
	var $rating;

	function setup($time, $comment, $pid, $rating) {
		$this->time = $time;
		$this->comment = $comment;
		$this->pid = $pid;
		$this->rating = $rating;
	}

	function getTime() {
		return $this->time;
	}

	function getComment() {
		return $this->comment;
	}

	function getPid() {
		return $this->pid;
	}

	function getRating() {
		return $this->rating;
	}

	function setName($name) {
		$this->name = $name;
	}

	function getName() {
		return $this->name;
	}

}
