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


class mapLevelClass {
	var $id;
	var $parents;
	var $subfolders;
	var $maps;
	var $name;
	var $comment;
	var $time;
	var $pid;
	
	function setup($id, $name, $comment, $time, $pid) {
		$this->id = $id;
		$this->name = $name;
		$this->comment = $comment;
		$this->time = $time;
		$this->pid = $pid;
	}
	function getId() {
		return $this->id;
	}

	function getPid() {
		return $this->pid;
	}

	function getTime() {
		return $this->time;
	}

	function setParents($parents) {
		$this->parents = $parents;
	}

	function getParents() {
		return $this->parents;
	}

	function setSubFolders($sf) {
		$this->subfolders = $sf;
	}

	function getSubFolders() {
		return $this->subfolders;
	}
	
	function getName() {
		return $this->name;
	}

	function getComment() {
		return $this->comment;
	}

	function setMaps($maps) {
		$this->maps = $maps;
	}

	function getMaps() {
		return $this->maps;
	}
}

?>
