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
?>

<?
include "$BASE_FILES/inc_mapcollection_mapdetails_detail.php";
include "$BASE_FILES/inc_mapcollections_mapcomments.php";
if ($userdata['session_logged_in'] == true) {
	
	include "$BASE_FILES/inc_mapcollection_addcomment.php";
} else {
	echo "If you were logged in you could comment on this map<BR>";
}
if ($userdata['session_logged_in'] == true && $userdata['user_id'] == $md->getPid()) {
	include "$BASE_FILES/inc_mapcollections_mapoptions.php";
}

?>
