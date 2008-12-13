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
<td class="topborder" bgcolor="#3366CC"> <div class="heading">What If Scoring Calculator</div></td>
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
<B>Please select your options from below.</b><P>
<form action="/whatif.php" method="GET">
<?
if ($first == true && $userdata['session_logged_in'] == true) {
	$listA = Array();
	$listA[0] = $userdata['username'];
}
if (sizeof($listA) <= 1) {
	echo "<input name='playera' type='text' width=20 value='" . htmlspecialchars($listA[0]) . "'>";
} else {
	echo "<select name='playera'>\n";
	$count = 0;
	while ($count < sizeof($listA)) {
		echo "<option value='" . htmlspecialchars($listA[$count]) . "'>" . htmlspecialchars($listA[$count]) . "</option>\n";
		$count++;
	}
	echo "</select>\n";
}
echo " defeats ";
if (sizeof($listB) <= 1) {
	echo "<input name='playerb' type='text' width=20 value='" . htmlspecialchars($listB[0]) . "'>\n";
} else {
	echo "<select name='playerb'>\n";
	$count = 0;
	while ($count < sizeof($listB)) {
		echo "<option value='" . htmlspecialchars($listB[$count]) . "'>" . htmlspecialchars($listB[$count]) . "</option>\n";
		$count++;
	}
	echo "</select>\n";
}
echo " in a <select name='gametype'><option value='strict'";
if ($gameType == "strict") { echo " selected"; }
echo ">Strict Tournament</option>\n<option value='tourn'";
if ($gameType == "tourn") { ECHO " selected"; }
echo ">Tournament</option>\n</select> <select name='team'><option value='players'";
if ($isTeam == false) { echo " selected"; }
echo ">1x1</option><option value='teams'";
if ($isTeam == true) { echo " selected"; }
echo ">Team's</option></select> game.";
?>
<input type=submit value="Calculate"></form>

</div></td>
</tr>
</table>


<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>


