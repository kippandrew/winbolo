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
$totalResults = sizeof($results);
$searchCount = 0;
if ($totalResults == 0) {
	echo "<center class='bodytext'>Sorry, no results found<br><a href='mapcollectionsearch.php'>Search again?</a></center>";
} else {
	while ($searchCount < $totalResults) {
		$md = $results[$searchCount];
		include "$BASE_FILES/inc_mapcollection_mapdetails_detail2.php";
		$searchCount++;
	}
}
?>
