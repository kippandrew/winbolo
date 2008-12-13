<?
require "../php/header.php";
ob_start();
$name = $HTTP_GET_VARS['name'];
if ($name == null) {
	header("Location: index.php");
	exit;
}
openDatabase(false);
$players = getPlayers($name, 50);
if (count($players) == 1) {
	if (getPid($name) != 0) {
		header("Location: playerinfo.php?name=" . urlencode($name));
		closeDatabase();
		exit;
	}
	}

ob_end_flush();
$html_title = "WinBolo.net: Search Results for $name";
include "$BASE_FILES/inc_top.php";
?>
<?
$error_heading="Search Results";
if (count($players) == 0) {
	$error_body="Sorry, no matches to your query.";
} else {
	$error_body="Search Results (Limit 50 matches)<P>";
	$count = 0;
	while ($count < count($players)) {
		$error_body .= "<a href=\"playerinfo.php?name=" . urlencode($players[$count]) . "\">" . $players[$count] . "</A><BR>";
		$count++;
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
