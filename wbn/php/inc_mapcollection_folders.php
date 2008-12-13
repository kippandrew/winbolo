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

<div class="subheading"><b>Folders</b></div>
<div class="bodytext">The following Folders are available.<p>
<center>
<table width="80%" border=1><tr>
<?
$folders = $mc->getSubFolders();
$count = 0;
$total = sizeof($folders);
while ($count < $total) {
	$current = $folders[$count];
	$id = $current->getId();
	$name = htmlspecialchars($current->getName());
	$amount = $current->getSubItems();
	echo "<td align='center' class='bodytext'><a href='mapcollection.php?id=$id'>$name</a> ($amount)</td>";
	$count++;
	if ($count % 4 == 0) {
		echo "</tr><tr>";
	}
}
?>
</tr></table>
</center>
<P>


