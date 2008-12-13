<?
$html_title="WinBolo.net: Team Statistics";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";
$name = $HTTP_GET_VARS['name'];
$team = getTeamClass($name);
if ($team == 0) {
	$error_body = "Team does not exist.";
	include "$BASE_FILES/inc_error.php";
} else {
	include "$BASE_FILES/inc_teamdetails.php";
}
require "$BASE_FILES/inc_bottom.php";
?>
