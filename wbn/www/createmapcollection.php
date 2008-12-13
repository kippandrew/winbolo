<?
include "../php/header.php";
include "$BASE_FILES/mapimageclass.php";
$error = "OK";
if ($userdata['session_logged_in'] == false) {
	$error = "Sorry you must be logged in to use this function";
} else {
	$error = mcCreateTopLevelCollection($userdata['user_id']);
}


if ($error == "OK") {
	header("Location: playermodify.php");
} else {
	include "$BASE_FILES/inc_top.php";
	$error_body = $error;
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";

}
?>
