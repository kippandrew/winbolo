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
$name = getPlayerName($mc->getPid());
?>
<table border="0" width=100%>
<tr><td><div class="subheading"><b>Navigation</b></div></td></tr><tr>
<td>
<div class="bodytext">
<?
if ($mc->getId() != '') {
	include "$BASE_FILES/inc_mapcollection_location.php";
} else {
echo "Welcome to WinBolo.net players map collections. You can navigate the map collection by clicking on the folders section below. To move back up a folder click on the folder name in the navigation section or click the back button on your web browser.<P>To download a map simply click on the map picture or click on the map name. To bring up more information click on the word info next to the map. If you are logged in you can write reviews of maps.<P>To create a map collection login and goto your profile. At the bottom of the page you will find a create map collection button. Press it to create a collection. After creating a collection you will be able to browse the map collection and add maps and folders into your own map collection. Please only upload maps that you have created or modified, this will save on resources. <P>For more information about features to come as well as feedback please see the forums";
echo "<P>";
echo "It is now possible to <a href='mapcollectionsearch.php'>Search the map collection</a>.";
echo "<P><P>";
}

echo "Path: ";
$parents = $mc->getParents();
$count = sizeof($parents) - 1;
while ($count >= 0) {
	$current = $parents[$count];
	$id = $current->getId();
	$name = htmlspecialchars($current->getName());
	echo "<a href='mapcollection.php?id=$id'>$name</a>";
	if ($count > 0) {
		echo " / ";
	}
	$count--;
}
$id = $mc->getId();
$name = htmlspecialchars($mc->getName());
echo " / <a href='mapcollection.php?id=$id'>$name</a>";
?>
</div></td></tr>
</table>
<P>

