<?
require "../php/header.php";
$html_title="WinBolo.net: Modify Player";
openDatabase(false);
$password = $HTTP_POST_VARS['password'];
$name= $HTTP_GET_VARS['name'];

$passwordOk = checkPassword($name, $password, "EDITTEAM");
include "$BASE_FILES/inc_top.php";
if ($userdata['session_logged_in'] == false) {
	$error_body = "You must login before you can use this page.";
	include "$BASE_FILES/inc_error.php";
} else if ($passwordOk == true)  {
	$team = getTeamClass($name);
#	$cookieTeam = $team->getName();
	include "$BASE_FILES/inc_teammodify.php";
} else {
	$error_body = "Your password is incorrect.";
	include "$BASE_FILES/inc_error.php";
}
require "$BASE_FILES/inc_bottom.php";
?>
