<?
require "../php/header.php";
$html_title="WinBolo.net: Lost Password";
require "$BASE_FILES/inc_top.php";
$error_heading = "Forgotten your password?";
if ($HTTP_GET_VARS['login'] == true) {
	$error_body = "<br><B>Login Failed</B><BR>";
} else {
	$error_body = "";
}
$error_body .= '<br>If you have forgotten your password fill it this form, and a new password will be sent to your email address.<br><br><form method="post" action="processlostpassword.php">Player name: <input type="text" name="username" size="10">&nbsp;Email address: <input type="text" name="email" size="10">&nbsp;<input type=submit value="Process"></form>';
require "$BASE_FILES/inc_error.php";
require "$BASE_FILES/inc_bottom.php"; ?>

