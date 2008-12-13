<?
$html_title="WinBolo.net: Statistics";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";
?>
<table width="480" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="20"><img src="images/stleft.png" width="20" height="20"></td>
<td class="topborder" bgcolor="#3366CC"> <div class="heading">Statistics
</div></td>
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
<td>
<tr>
<td> <div class="subheading">1 vs 1 Ranks</div></td>
</tr>
<tr><td>
<center>
<table border=0 width=90% cellpadding=5>
<tr><td class="bodytext"><b>Open</b></td><td class="bodytext"><b>Tournament</b></td><td class="bodytext"><b>Strict Tournament</b></td></tr>
<tr><td class="bodytext">N/A</td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=player">Top Players</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=player">Top Players</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=player&sub_type=bases">Base Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=player&sub_type=bases">Base Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=player&sub_type=bases">Base Captures</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=player&sub_type=pills">Pillbox Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=player&sub_type=pills">Pillbox Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=player&sub_type=pills">Pillbox Captures</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=player&sub_type=tanks">Tank Kills</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=player&sub_type=tanks">Tank Kills</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=player&sub_type=tanks">Tank Kills</a></td></tr>
</table>
<br>
</center></td>
</tr>
<tr><td><div class="subheading">Team Rankings</div></td></tr>
<tr><td>
<center>
<table border=0 cellpadding=5 width=90%>
<tr><td class="bodytext"><b>Open</b></td><td class="bodytext"><b>Tournament</b></td><td class="bodytext"><b>Strict Tournament</b></td></tr>
<tr><td class="bodytext">N/A</td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=teams">Top Teams</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=teams">Top Teams</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=teams&sub_type=bases">Base Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=teams&sub_type=bases">Base Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=teams&sub_type=bases">Base Captures</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=teams&sub_type=pills">Pillbox Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=teams&sub_type=pills">Pillbox Captures</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=teams&sub_type=pills">Pillbox Captures</a></td></tr>
<tr><td class="bodytext"><a href="/showstats.php?game_type=open&stats_type=teams&sub_type=tanks">Tank Kills</a></td><td class="bodytext"><a href="/showstats.php?game_type=tournament&stats_type=teams&sub_type=tanks">Tank Kills</a></td><td class="bodytext"><a href="/showstats.php?game_type=strict&stats_type=teams&sub_type=tanks">Tank Kills</a></td></tr>
</center>
</table>
</td></tr>


</table>

 
<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

	
<?
require "$BASE_FILES/inc_bottom.php";
?>
