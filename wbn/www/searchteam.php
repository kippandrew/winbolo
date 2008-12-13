<?
require "../php/header.php";
ob_start();
$name = $HTTP_GET_VARS['name'];
if ($name == null) {
	header("Location: index.php");
	exit;
}
openDatabase(false);
$teams = getTeams($name, 50);
if (count($teams) == 1) {
	if (getTid($name) != -1) {
		header("Location: teaminfo.php?name=" . urlencode($name));
		closeDatabase();
		exit;
	}
	}

ob_end_flush();
include "$BASE_FILES/inc_top.php";
$error_heading="Search Results";
if (count($teams) == 0) {
	$error_body="Sorry, no matches to your query.";
} else {
	$error_body="Search Results (Limit 50 matches)<P>";
	$count = 0;
	while ($count < count($teams)) {
		$error_body .="<a href=\"teaminfo.php?name=" . urlencode($teams[$count]) . "\">" . $teams[$count] . "</A><BR>";
		$count++;
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
