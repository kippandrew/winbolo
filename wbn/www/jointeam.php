<?
require "../php/header.php";
ob_start();
$name = $HTTP_GET_VARS['name'];
$password = $HTTP_POST_VARS['password'];
if ($name == null || $userdata['session_logged_in'] == false) {
	header("Location: index.php");
	exit;
}
openDatabase(false);
$result = doJoinTeam($userdata['username'], $name, $password);
if ($result == "OK") {
	header("Location: teaminfo.php?name=" . $name);
	closeDatabase();
	exit;
}

ob_end_flush();
include "$BASE_FILES/inc_top.php";
$error_body=$result;
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
