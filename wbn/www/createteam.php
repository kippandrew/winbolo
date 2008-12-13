<?
require "../php/header.php";
$html_title="WinBolo.net: Create Team";
require "$BASE_FILES/inc_top.php";
if ($userdata['session_logged_in'] == false) {
	$error_body="Sorry, you must be logged in to create teams.";
	include "$BASE_FILES/inc_error.php";
} else {
	include "$BASE_FILES/inc_createteam.php";
}

require "$BASE_FILES/inc_bottom.php";
?>

