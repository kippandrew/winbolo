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
<td><div class="bodytext">
<br>
<?
if ($isTeam == true) {
	$url = "http://www.winbolo.net/teaminfo.php?name=";
} else {
	$url = "http://www.winbolo.net/playerinfo.php?name=";
}
$urlA = urlencode($playerA);
$urlB = urlencode($playerB);
echo "Results for <a href='$url$urlA'>$playerA</a> vs <a href='$url$urlB'>$playerB</a><P>";
?>
<P>
</td></tr>
<tr>
<td> <div class="subheading">
<? echo "$playerA defeats $playerB";?></div></td>
</tr>
<tr>
<td>
<P>
<table border=0 cellpadding=5>
<tr>
<td class="bodytext"></td><td class="bodytext"><b>Old Score</b></td><td class="bodytext"><b>New Score</b></td></tr>
<tr><td class="bodytext"><?echo $playerA;?></td><td class="bodytext"><?echo $scoreA;?></td><td class="bodytext"><?echo $newScoreA_1;?></td></tr>
<tr><td class="bodytext"><?echo $playerB;?></td><td class="bodytext"><?echo $scoreB;?></td><td class="bodytext"><?echo $newScoreB_1;?></td></tr>
</table>
<P>
</td></tr>
<tr>
<td> <div class="subheading">
<? echo "$playerB defeats $playerA";?></div></td>
</tr>
<tr><td> 

<P>
<table border=0 cellpadding=5>
<tr>
<td class="bodytext"></td><td class="bodytext"><b>Old Score</b></td><td class="bodytext"><b>New Score</b></td></tr>
<tr><td class="bodytext"><?echo $playerA;?></td><td class="bodytext"><?echo $scoreA;?></td><td class="bodytext"><?echo $newScoreA_2;?></td></tr>
<tr><td class="bodytext"><?echo $playerB;?></td><td class="bodytext"><?echo $scoreB;?></td><td class="bodytext"><?echo $newScoreB_2;?></td></tr>
</table>
<P>
<a href="/whatif.php">Start Over?</a><P>
</div></td>
</tr>
</table>


<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>


