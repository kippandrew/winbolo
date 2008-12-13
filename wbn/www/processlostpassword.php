<?
$username = $HTTP_POST_VARS['username'];
$email = $HTTP_POST_VARS['email'];
require "../php/header.php";
$html_title="Winbolo.net: Lost Password";
require "$BASE_FILES/inc_top.php";
if ($HTTP_POST_VARS['email'] == "" || $HTTP_POST_VARS['username'] == "") {
	$error_body = "Please fill in both fields " . strlen($email);
} else {
	$result = doLostPassword($HTTP_POST_VARS['username'], $HTTP_POST_VARS['email']);
	if ($result == "OK") {
		$error_heading="Email Password";
		$error_body="An email containing a new password has been sent out to you. Please follow the instructions contained within it. Click <A href=\"index.php\">here</A> to return to the main page";
	} else {
		$error_body="<b>Error:</b>" . $result . "<P>Click the back button on your browser to correct the error and try again.";
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>

