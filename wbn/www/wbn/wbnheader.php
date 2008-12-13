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


require "/home/winbolo/php/winbolonetcfg.php" ;
require "/home/winbolo/www/wbn/errors.php";
#Header stuff
header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
header ("Pragma: no-cache");                          // HTTP/1.0

# Extract data
$argstmp = explode("&",getenv('QUERY_STRING'));
while (list($stuff,$crap) = each($argstmp)) {
	list($idx,$val) = explode("=",$crap);
	$args[$idx] = rawurldecode($val);
}
$fullData = $args[data];

# Check length
if (strlen($fullData) < 4) {
	echo "Sorry. Wrong way";
	exit;
}

$header = substr($fullData, 0, 3);
#Check Header
if (ord($header[0]) != $cfg_wbnversion_major || ord($header[1]) != $cfg_wbnversion_minor || ord($header[2]) != $cfg_wbnversion_revision) {
	writeError("Incorrect WinBolo.net Version");
	exit;
}

$messageType = ord(substr($fullData,3,1));
$data = substr($fullData,4);

?>
