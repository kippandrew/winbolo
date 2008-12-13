<?
require "../php/header.php";
$name = $HTTP_POST_VARS['name'];
$players = $HTTP_POST_VARS['players'];
$confirm = $HTTP_POST_VARS['confirm'];
if ($userdata['session_logged_in']== false) {
	$result = "Sorry, you must be logged in to do this.";
} else if (checkPassword($name, $confirm, "EDITTEAM") == true) {
	$result = doModifyTeamMembers($name, $players);
} else {
	$result = "Sorry, your confirmation password is missing or incorrect. Please press the back button on your web browser and correct the problem.";
}

if ($result == "OK") {
	header("Location: teaminfo.php?name=" . $name);
	exit;
	
}

$html_title="Winbolo.net: Modify Team";
$error_body=$result;
require "$BASE_FILES/inc_top.php";
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
