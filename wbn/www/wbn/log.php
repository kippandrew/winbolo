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


# Simple log file uploader
require "../../php/header.php";
require "zip.php";
require "errors.php";

# If you do not wish log files to be uploaded uncomment the next 3 lines
#echo chr(1);
#echo chr(1);
#exit();



$error = "OK";
$logfile = $HTTP_POST_FILES['logfile'];
$key = $HTTP_GET_VARS['key'];
$proceed = true;
if (is_uploaded_file($logfile['tmp_name']) == false || !isset($HTTP_GET_VARS['key'])) {
	$error = "File not uploaded";
	$proceed = false;
}

$size = $logfile['size'];
# Don't save files over 20MB
if ($size > 20000000) {
	$error = "File size too large";
	$proceed = false;
}

# Check file
if ($proceed == true) {
	$fileSize = $logfile['size'];
	$zip = new Zip($logfile['tmp_name']);
	$res = $zip->getData();
	if ($res == "Error") {
		$proceed = false;
		$error = "Zip bit failed";
	} else if ($res['key'] != $key) {
		$proceed = false;
		$error = "Key's dont match";
	}
}

# Check key
if ($proceed == true) {
	$sqlKey = mysql_escape_string($key);
	$sqlVersion = mysql_escape_string($res['version']);
	$sqlMap = mysql_escape_string($res['mapname']);
	$query = "select * from archived_game_server where ags_key = '$sqlKey' and ags_version = '$sqlVersion' and ags_mapname = '$sqlMap'";
	$result = mysql_query($query);
	if (mysql_num_rows($result) == 0) {
		$error = "Not a valid game: " . $key;
		$proceed = false;
	}
}

# Try creating updating table 
if ($proceed == true) {
	$query = "update archived_game_server set ags_log_available = 'Y', ags_log_size = $size, ags_log_downloads = 0 where ags_key = '$sqlKey'";
	$result2 = mysql_query($query);
	if (!$result2) {
		$proceed = false;
		$error = "Create error . $query";
	} else {
		$path = $cfg_upload_log_path . $key;
	}
}

if ($proceed == true) {
	$proceed = move_uploaded_file($logfile['tmp_name'], $path);
	if ($proceed == false) {
		$error = "Move failed";
		$query = "update archived_game_server set ags_log_available='N' where ags_key = '$sqlKey'";
		mysql_query($query);
	}
}

if ($error == "OK") {
	echo chr(1);
	echo chr(1);
	exit();
} else {
	writeError($error);
	exit();
}

?>
