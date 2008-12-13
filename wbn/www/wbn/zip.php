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


# Simple Zip class for testing a zip file is a winbolo log file
# Uses splitter C program
class Zip {
	var $fileName = "";
	var $mapLength = 0;

	function Zip($fileName) {
		$this->fileName = $fileName;
	}

	function convert($input) {
		$end = strlen($input);
		$multiplier = 1;
		for($i=0; $i < $end; $i++) {
			$output = $output + (ord($input[$i]) * $multiplier);
			$multiplier = $multiplier * 256;
		}
		return $output;
	}

	function getMapName($data) {
		$this->mapLength = ord($data[9]);
		$name = substr($data, 10, $this->mapLength);
		return $name;
	}

	function getVersion($data) {
		$versionMajor = ord($data[$this->mapLength+15]);
		$versionMinor = ord($data[$this->mapLength+16]);
		$versionRevision = ord($data[$this->mapLength+17]);
		$version = "$versionMajor.$versionMinor$versionRevision";
		return $version;
	}

	function getKey($data) {
		$add = 28;
		if (ord($data[$this->mapLength+$add]) == 0) {
			return "";
		}
		return substr($data, $this->mapLength+$add, 32);
	}
	
	function checkData($data) {
		# Header
		$header = substr($data, 0, 8);
		if (strlen($data) != 200) {
			return false;
		}
		if ($header != "WBOLOMOV") {
			return false;
		}
		# Version
		if (ord($data[8]) != 0) {
			return "Error";
		}
		# Map size
		if (ord($data[9]) > 36) {
			return "Error";
		}
		return true;
	}

	function getData() {
		global $cfg_splitter_app;
		$fileInfo = "Error";
		$fp = fopen($this->fileName, "rb");
		$content = fread($fp,30);
		$id = substr($content,0,4);

		if (!$fp) {
			return "Error";
		}

		if ($id == "\x50\x4b\x03\x04") {
			$fileInfo = Array();
        		$fileInfo['file-size'] = $this->convert(substr($content,18,4));
			$fileInfo['filename-size'] = $this->convert(substr($content,26,2));
			$fileInfo['compression-type'] = $this->convert(substr($content,8,2));
			$fileInfo['crc'] = $this->convert(substr($content,14,4));
			$fileInfo['filename'] = fread($fp,$fileInfo['filename-size']);
			if ($fileInfo['filename'] == "log.dat") {
				$data = `unzip -p $this->fileName | $cfg_splitter_app`;
				$fileInfo['data'] = $data;
				if ($this->checkData($fileInfo['data']) == false) {
					return "Error";
				}
				$fileInfo['mapname'] = $this->getMapName($data);
				$fileInfo['version'] = $this->getVersion($data);
				$fileInfo['key'] = $this->getKey($data);
			} else {
				fclose($fp);
				return "Error";
			}
		}
		fclose($fp);
		return $fileInfo;
	}
}

?>
