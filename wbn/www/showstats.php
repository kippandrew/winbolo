<?
$html_title="WinBolo.net: Statistics";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";

$stats_type = $HTTP_GET_VARS['stats_type'];
$game_type = $HTTP_GET_VARS['game_type'];
$sub_type = $HTTP_GET_VARS['sub_type'];
$hide_players = $HTTP_GET_VARS['hide'];

$error = "Missing data";
$ok = true;
$limit = 100;
$title2 = "Score";
$st = false;
$addSql = "";

if ($stats_type == "teams") {
	$link = "teaminfo.php";
	$titleSubText = "Teams";
	$team = true;
} else if ($stats_type == "player") {
	$link = "playerinfo.php";
	$titleSubText = "Players";
	$team = false;
} else {
	$ok = false;
}

if ($ok == true) {
	if ($game_type == "tournament") {
		$title = "Top 100 $titleSubText Tournament games";
		$sqlFieldName = "tourn_";
	} else if ($game_type == "strict") {
		$title = "Top 100 $titleSubText Strict Tournament Games";
		$sqlFieldName = "strict_";
	} else {
		$title = "Top 100 $titleSubText Open Games";
		$sqlFieldName = "open_";
	}
	// Set the sub types
	if ($sub_type == "pills") {
		$sqlFieldName .= "num_pills";
		$title .= " - Pill Captures";
	} else if ($sub_type == "bases") {
		$sqlFieldName .= "num_bases";
		$title .= " - Base Captures";
	} else if ($sub_type == "tanks") {
		$sqlFieldName .= "num_tanks";
		$title .= " - Tank Kills";
	} else {
		if ($sqlFieldName == "open_") {
			$ok = false;
			$error = "You can't get rankings for open games!";
		} else {
			$sqlFieldNameStart = $sqlFieldName;
			$sqlFieldName .= "score";
			if ($hide_players != "no") {
				$st = true;
			}
		}
	}
}

if ($ok == true) {
	if ($team == true) {
		if ($st == true) {
			$addSql = " where " . $sqlFieldNameStart . "wins > 0 or " . $sqlFieldNameStart . "loses > 0";
		}
		$sql = "SELECT name, $sqlFieldName as score from stats_team $addSql order by $sqlFieldName desc limit $limit";
	} else {
		if ($st == true) {
			$addSql = " and ( p." . $sqlFieldNameStart . "wins > 0 or p." . $sqlFieldNameStart . "loses > 0)";
		}

		$sql = "SELECT u.username as name, p." . $sqlFieldName . " as score from users u, stats_player p where p.pid = u.user_id $addSql order by p." . $sqlFieldName . " desc limit $limit";
	}
	$result = mysql_query($sql);
	if (!$result) {
		$ok = false;
		$error = "Database error";
	}
}

if ($ok == false) {
	$error_body = $error;
	require "$BASE_FILES/inc_error.php";
	require "$BASE_FILES/inc_bottom.php";
	exit;
}
?>

<table width="480" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="20"><img src="images/stleft.png" width="20" height="20"></td>
<td class="topborder" bgcolor="#3366CC"> <div class="heading">
 <? echo $title;?></b></td>
 </div></td>
 <td width="20"><img src="images/stright.png" width="20" height="20"></td>
 </table></td>
 </tr>
 <tr>
 <td> <table width="480" border="0" cellspacing="0" cellpadding="0">
 <tr>
 <td width="1" class="leftborder">&nbsp;</td>

 <td width="282">
 <table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<TR>
<td >
<b>Rank</b></td>
<TD>
<b>Name</B></TD>
<TD>
<?

echo "<b>$title2</B></TD></TR>\n";
$count = 0;
$lastScore = 10000000;
$num = mysql_numrows($result);
$rank = 0;
while ($count < $num) {
	echo "<TR>";
	if ($count < $num) {
		$name = mysql_result($result,$count,"name");
		$score = mysql_result($result,$count,"score");
		if ($score < $lastScore) {
			$rank += 1;
			$lastScore = $score;
		}
		echo "<td>";
		echo $rank;
		echo "</td><td><a href=\"$link?name=" . urlencode($name) . "\">";
		echo $name;
		echo "</a></td><td>";
		echo $score;
		echo "</TD>";
	} else {
		echo "</TD><TD></TD><TD></TD>";
	}
	echo "</TR>";
	$count++;
}
?>
	</tr>
</div></td>
</tr>
</table>
<div class="bodytext">
<?
if ($st == true) {
	$urls = $HTTP_SERVER_VARS['REQUEST_URI'] . "&hide=no";
	echo "<P>";
	echo "Players/Teams who have not completed (i.e lost or won) any games have been hidden from this ranking. You can view the rankings with them <a href='$urls'>included</a> or view the <a href='/forums/memberlist.php'>members list</a> to see a complete listing of players.";
}
?>
<!-- end content -->
</td>
<td width="1" class="rightborder">&nbsp;</td>
</tr>
</table>

	
<?
require "$BASE_FILES/inc_bottom.php";
?>
