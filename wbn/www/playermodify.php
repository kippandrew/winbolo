<?
$html_title="WinBolo.net: Modify Player";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";
if ($userdata['session_logged_in'] == false) {
	$error_message="You must login before you can use this page.";
	include "$BASE_FILES/inc_error.php";
} else {
	$player = getPlayerClass($userdata['username']);
	include "$BASE_FILES/inc_playermodify.php";
}
include "$BASE_FILES/inc_bottom.php";
?>
