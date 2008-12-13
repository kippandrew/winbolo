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
<td class="topborder" bgcolor="#3366CC"> <div class="heading">
Modify Team Details</div></td>
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
<p>
To modify your team, fill out the following form.</span>
            </font> 
                    <form method="post" action="processmodifyteam.php">
<input type="hidden" name="name" value="<?echo $team->getName();?>">
<table width="95%" border="0" class="bodytext">
<tr><td>Edit Password</td>
<td><input type="password" name="editpassword" size=30></td></tr>
<tr><td>Renter Edit Password</td>
<td><input type="password" name="editpassword2" size=30></td></tr>
<tr><td></td><td>Leave blank for no change</td></tr>
<tr><td>Join Password</td>
<td><input type="password" name="joinpassword" size=30></td></tr>
<tr><td>Renter Join Password</td>
<td><input type="password" name="joinpassword2" size=30></td></tr>
<tr><td></td><td>Leave blank for no change</td></tr>
                        <tr> 
                          <td>Team Blurb</td>
                          <td> 
                            <textarea name="blurb" cols="30" rows="6">
<? echo $team->getBlurb();?></textarea>
                          </td>
                        </tr>

<tr><td>Team Website Address</td>
<td><input type="text" name="website" value="<? echo $team->getWebsite();?>"size=30></td></tr>
			</tr>
		    </table><br>
<b>Enter your current password to confirm changes:</b>&nbsp;<input type="password" size=20 name="confirm"><br><br>
                      <div align="right"> 
                        <input type="submit" name="Submit" value="Modify Team">
</div>
                    </form>
          </td>
        </tr>
	<br><br>
	</div></td></tr>
	<tr><td><div class="subheading">Modify Team Members</div></td></tr>
<tr><td><div class="bodytext">
            <p>To modify your team members click on each team member you wish to remove then click the remove players button.
                    <form method="post" action="processmodifyteammembers.php">
<input type="hidden" name="name" value="<?echo $team->getName();?>">
<select name="players[]" size=8 width=100 multiple>
<?
$players = $team->getPlayers();
$count = 0;
while ($count < sizeof($players)) {
if ($players[$count] != $team->getOwner()) {
echo "<option value=\"" . $players[$count] . "\">" . $players[$count] . "</option>";
}
$count++;
}
?>
</select><br><br>
<b>Enter your current password to confirm changes:</b>&nbsp;<input type="password" size=20 name="confirm"><br><br>
                      <div align="right"> 
                        <input type="submit" name="Submit" value="Remove Players">
                    </form>
</div></td></tr>
</table>

<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>




