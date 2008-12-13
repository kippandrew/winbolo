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

class mapCollectionMapInfoClass extends mapInfoClass {
	var $id;
	var $comments;
	var $numComments;
	var $rating;
	var $numRatings;
	var $parents;
	var $pid;
	var $ownerName;
	var $time;
	var $downloads;

	function setDownloads($num) {
		$this->downloads = $num;
	}

	function getDownloads() {
		return $this->downloads;
	}

	function setId($id) {
		$this->id = $id;
	}

	function getId() {
		return $this->id;
	}

	function setNumRatings($r) {
		$this->numRatings = $r;
	}

	function getNumRatings() {
		return $this->numRatings;
	}

	function setTime($t) {
		$this->time = $t;
	}

	function getTime() {
		return $this->time;
	}
	function setPid($p) {
		$this->pid = $p;
	}

	function getPid() {
		return $this->pid;
	}

	function setComments($comments) {
		$this->comments = $comments;
		$this->numComments = sizeof($comments);
	}

	function getComments() {
		return $this->comments;
	}

	function getNumberComments() {
		return $this->numComments;
	}

	function setOwnerName($name) {
		$this->ownerName = $name;
	}

	function getOwnerName() {
		return $this->ownerName;
	}

	function setParents($p) {
		$this->parents = $p;
	}

	function getParents() {
		return $this->parents;
	}

	function setRating($r) {
		$this->rating = $r;
	}

	function getRating() {
		return $this->rating;
	}
}
?>
