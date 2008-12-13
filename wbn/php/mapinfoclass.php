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
	
class mapInfoClass {
var $numStarts;
var $numPills;
var $numBases;
var $name;
var $comment;

function setup($name, $numBases, $numPills, $numStarts, $comment) {
	$this->name = $name;
	$this->numBases = $numBases;
	$this->numPills= $numPills;
	$this->numStarts = $numStarts;
	$this->comment = $comment;
}
function getNumPills() {
	return $this->numPills;
}
function getNumStarts() {
	return $this->numStarts;
}
function getNumBases() {
	return $this->numBases;
}
function getName() {
	return $this->name;
}
function getComment() {
	return $this->comment;
}

}
?>
