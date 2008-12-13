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

<table width="480" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="20"><img src="images/stleft.png" width="20" height="20"></td>
<td class="topborder" bgcolor="#3366CC"> <div class="heading">Search the Map Collections</div></td>
<td width="20"><img src="images/stright.png" width="20" height="20"></td>
</tr>
</table></td>
</tr>
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="1" class="leftborder">&nbsp;</td>
<td width="282">
<table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
</tr>
<tr>
<td> <div class="bodytext">
You may search the map collections by <i>Map Name</i>, <i>Rating</i> or <i>show the newest maps</i>
<br><br>
<form action="mapcollectionsearchresults.php" method="POST">
Search by map name: <input type=text name="name" length=20><p>
<input type=radio name="search" value="name" checked>Search by Name<br>
<b>OR</b><p>
<input type=radio name="search" value="rating">Search by Rating<br>
<input type=radio name="search" value="date">Show newest maps<p>
<P>Order these by <select name="order">
<option value="newest" select>Alphabetical/Highest Rated/Newest</option>
<option value="oldest" select>Reverse Alphabetical/Lowest Rated/Oldest</option>
</select>
<P>
<input type="submit" value="search">
</form>
<p>Please note that all search results are currently limited to 50 results.
<p>&nbsp;</p>
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

