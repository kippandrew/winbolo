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

class mapCollectionItemClass {
	var $name;
	var $id;
	var $ratings;
	var $subItems;

	function setup($id, $name) {
		$this->id = $id;
		$this->name = $name;
	}

	function getSubItems() {
		return $this->subItems;
	}

	function setSubItems($items) {
		$this->subItems = $items;
	}

	function getName() {
		return $this->name;
	}

	function getId() {
		return $this->id;
	}

	function setRating($ratings) {
		$this->ratings = $ratings;
	}

	function getRatings() {
		return $this->ratings;
	}
}
