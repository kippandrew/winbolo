<?
require "../php/header.php";
$html_title="WinBolo.net: Active Games";
include "$BASE_FILES/inc_top.php";

$type = $HTTP_GET_VARS['type'];
$pw = $HTTP_GET_VARS['pw'];
$lock = $HTTP_GET_VARS['lock'];
$start = $HTTP_GET_VARS['start'];


if ($type != "") {
  setcookie("cookie_activegame_type", $type, time() + 3600);
  setcookie("cookie_activegame_pw", $pw, time() + 3600);
  setcookie("cookie_activegame_lock", $lock, time() + 3600);
  setcookie("cookie_activegame_start", $start, time() + 3600);
}
else {
  $type = $HTTP_COOKIE_VARS['cookie_activegame_type'];
  $pw = $HTTP_COOKIE_VARS['cookie_activegame_pw'];
  $lock = $HTTP_COOKIE_VARS['cookie_activegame_lock'];
  $start = $HTTP_COOKIE_VARS['cookie_activegame_start'];
}

$servers = getActiveGames();

if (sizeof($servers) > 0) {
	require "$BASE_FILES/inc_active.php";
} else {
	$error_heading="Active Games";
	$error_body="Sorry there are no active games right now.";
	include "$BASE_FILES/inc_error.php";
}

include "$BASE_FILES/inc_bottom.php";
?>

