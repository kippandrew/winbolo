<?
require "../php/header.php";
$html_title="WinBolo.net: Signup";
include "$BASE_FILES/inc_top.php";
if ($userdata["session_logged_in"] == 1) {
	$error_body = "You are already logged in.";
	require "$BASE_FILES/inc_error.php";
} else {
	$confirm_id = generateRegisterID();
	if ($confirm_id == "Error") {
		$error_body = "You have exceeded the number of registration attempts for this session. Please try again later.";
		require "$BASE_FILES/inc_error.php";
	} else {
		require "$BASE_FILES/inc_signup.php";
	}
}
require "$BASE_FILES/inc_bottom.php";
?>

