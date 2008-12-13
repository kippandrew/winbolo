<?
$html_title="WinBolo.net: Player Statistics";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";

$name = $HTTP_GET_VARS['name'];
$name = str_replace("\\", "", $name);

$pid = getPid($name);
if ($pid == 0 || $name == "Administrator" || $name == "Anonymous") {
	$error_body = "Player does not exist.";
	require "$BASE_FILES/inc_error.php";
} else {
	$player = getPlayerClass($name);
	$map = getPlayerMap($player->getId());
	include "$BASE_FILES/inc_playerdetails.php";
}
require "$BASE_FILES/inc_bottom.php";
?>
