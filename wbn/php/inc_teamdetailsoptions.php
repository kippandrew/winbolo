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
if (isPlayerInTeam($userdata['username'], $name) == true) {
	echo "To leave this team click <a href=\"leaveteam.php?name=" . $name. "\"> here</a>.";
} else {
	echo "<form action=\"jointeam.php?name=" . $name . "\" method=\"POST\">I wish to join this team. Enter team join password: <input type=password name=\"password\" size=10>&nbsp;<input type=submit value=\"Join\"></form><P>";
}
echo "<form action=\"teammodify.php?name=" . $name . "\" method=\"POST\">I wish to edit this team. Enter team edit password: <input type=password name=\"password\" size=10>&nbsp;<input type=submit value=\"Edit\"></form><P>";

?>
