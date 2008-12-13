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

class mapClass {
  var $name;
  var $map;
  var $numStarts;
  var $numPills;
  var $numBases;

// Base is 17, pill is 18, start is 19
  function setup($name) {
  $this->name = $name;
  $this->map = array();
  $count = 0;
  $numStarts = 0;
  $numBases = 0;
  $numPills = 0;
  while ($count < 255 * 255) {
	$this->map[$count] = 255;
	$count++;
  }
}

function getName() {
	return $this->name;
}

function setName($name) {
	$this->name = $name;
}

function getNumPills() {
	return $this->numPills;
}

function setNumPills($num) {
	$this->numPills= $num;
}

function getNumStarts() {
	return $this->numStarts;
}

function setNumStarts($num) {
	$this->numStarts = $num;
}

function getNumBases() {
	return $this->numBases;
}

function setNumBases($num) {
	$this->numBases= $num;
}

// Base is 17, pill is 18, start is 19
function getMapPos($x, $y) {
	return $this->map[$x * 255 + $y];
}

function setMapPosGround($x, $y, $t) {
	$current = $this->getMapPos($x, $y);
	if ($current < 17 || $current > 19) {
		$this->map[$x * 255 + $y] = $t;
	}
}


// Base is 17, pill is 18, start is 19
function setMapPos($x, $y, $t) {
	$this->map[$x * 255 + $y] = $t;
}

function readPills($fp) {
	$count = 0;
	while ($count < $this->getNumPills() && !feof($fp)) {
		$xStr = fread($fp,1);
		$x = unpack("Cid", $xStr);
		$yStr = fread($fp,1);
		$y = unpack("Cid", $yStr);
		$garbage = fread($fp, 3);
		$this->setMapPos($x[id], $y[id], 18);
		$count++;
	}
}

function readBases($fp) {
	$count = 0;
	while ($count < $this->getNumBases() && !feof($fp)) {
		$xStr = fread($fp,1);
		$x = unpack("Cid", $xStr);
		$yStr = fread($fp,1);
		$y = unpack("Cid", $yStr);
		$garbage = fread($fp, 4);
		$this->setMapPos($x[id], $y[id], 17);
		$count++;
	}
}

function readStarts($fp) {
	$count = 0;
	while ($count < $this->getNumStarts() && !feof($fp)) {
		$xStr = fread($fp,1);
		$x = unpack("Cid", $xStr);
		$yStr = fread($fp,1);
		$y = unpack("Cid", $yStr);
		$garbage = fread($fp,1);
		$this->setMapPos($x[id], $y[id], 19);
		$count++;
	}
}



//highlen = 0, lowlen = 1, highsame = 2, logsame = 3, highDiff = 4, lowDiff = 5
function processRun($fp, $dataLen, $yValue, $startX, $endX) {
	$count = 0;
	$returnValue = 1;
	$state = 0;
	$mapPos = $startX;
	$len = 0;
	$itemStr = fread($fp, 1);
	$itemArr = unpack("Cid", $itemStr);
	$item = $itemArr[id];
	while ($count < $dataLen && !feof($fp)) {
		$needRead = FALSE;
		$highNibble = $lowNibble = $item;
		/* Extract Nibbles */
		$highNibble >>= 4;
	//	$lowNibble <<= 4;
		$lowNibble = $item - ($highNibble << 4);
		while ($needRead == FALSE) {
			switch ($state) {
			case 0:
				$len = $highNibble;
				if ($len < 8) {
					$state = 5;
				$len++;
				} else {
					$state = 3;
				}
				break;

			case 1:
				$len = $lowNibble;
				if ($len < 8) {
					$state = 4;
					$len++;
				} else {
					$state = 2;
				}
				$needRead = TRUE;
				break;

			case 5:
				$this->setMapPosGround($mapPos, $yValue, $lowNibble);
				$mapPos++;
				$len--;
				if ($len == 0) {
					$state = 0;
				} else {
					$state = 4;
				}
				$needRead = TRUE;
				break;
			case 4:
				$this->setMapPosGround($mapPos, $yValue, $highNibble);
				$mapPos++;
				$len--;
				if ($len == 0) {
					$state = 1;
				} else {
					$state = 5;
				}
				break;
			case 3:
				$count2 = 0;
				while ($count2 < ($len-6)) {
					$this->setMapPosGround($mapPos + $count2, $yValue, $lowNibble);
					$count2++;
				}
				$mapPos = ($mapPos + $count2);
				$state = 0;
				$needRead = TRUE;
				break;
			case 2:
				$count2 = 0;
				while ($count2 < ($len-6)) {
					$this->setMapPosGround($mapPos+$count2,$yValue, $highNibble);
					$count2++;
				}
				$mapPos = ($mapPos + $count2);
				$state = 1;
				break;
			}		
		}
		$count++;
		if ($count != $dataLen) {
			$itemStr = fread($fp,1);
			$itemArr = unpack("Cid", $itemStr);
			$item = $itemArr[id];
		}
	}
	/* Check all read correctly */
	if ($count != $dataLen || $mapPos != $endX)  { /*  || mapPos != endX */ 
		$returnValue = 0;
	}
	return $returnValue;
}

function readRuns($fp) {
	$done = false;
	$returnValue = 1;
	
	while (feof($fp) == false && $done == false) {
		$dataLenStr = fread($fp,1);
		$dataLen = unpack("Cid", $dataLenStr);
		$yStr = fread($fp,1);
		$y = unpack("Cid", $yStr);
		$startXStr = fread($fp,1);
		$startX = unpack("Cid", $startXStr);
		$endXStr = fread($fp,1);
		$endX = unpack("Cid", $endXStr);
		if ($dataLen[id] == 4 && $y[id] == 255 && $startX[id] == 255 && $endX[id] == 255) {
			$done = true;
		} else {
			if ($this->processRun($fp, $dataLen[id]-4, $y[id], $startX[id], $endX[id]) == 0) {
				$returnValue = false;
				$done = false;
			}
		}
	}
	return $returnValue;
}

function readMap($fileName) {
	set_magic_quotes_runtime(false); 
	$fp = fopen($fileName, "rb");
	if (!$fp || feof($fp)) {
		return false;
	}
	$header = fread($fp, 8);
	if ($header == FALSE) {
		return false;
	}
	if ($header != "BMAPBOLO") {
		return false;
	}
	$header = fread($fp,1);
	$header1 = unpack("Cid",$header);
	if ($header1[id] != 1)  {
		return false;
	}
	$item = fread($fp,1);
	$item1 = unpack("Cid", $item);
	$this->setNumPills($item1[id]);
	$item = fread($fp,1);
	$item1 = unpack("Cid", $item);
	$this->setNumBases($item1[id]);
	$item = fread($fp,1);
	$item1 = unpack("Cid", $item);
	$this->setNumStarts($item1[id]);
	if ($this->getNumPills() > 16 || $this->getNumStarts > 16 || $this->getNumBases() > 16 || feof($fp)) {
		return false;
	}

	$this->readPills($fp);
	$this->readBases($fp);
	$this->readStarts($fp);
	if (feof($fp)) {
		return false;
	}
	return $this->readRuns($fp);
}
}

?>
