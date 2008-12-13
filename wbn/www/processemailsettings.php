<?
require "../php/header.php";
$error_body = "OK";
$option=$HTTP_GET_VARS['option'];

if ($userdata['session_logged_in'] == false) {
        $error_body = "You must login before you can use this page.";
} else if (!isset($option) || !isset($HTTP_GET_VARS['option']) || ($option != "enable" && $option != "disable")) {
	$error_body = "Missing options.";
} else {
	if ($option == "enable") {
		$val = true;
	} else {
		$val = false;
	}
	$error_body = setEmailForward($userdata['username'], $val);
}

if ($error_body == "OK") {
echo "OK";
	header("Location: /playermodify.php");
} else {
	include "$BASE_FILES/inc_top.php";
	include "$BASE_FILES/inc_error.php";
	require "$BASE_FILES/inc_bottom.php";
}
?>
