<?
require "../php/header.php";
$html_title="Winbolo.net: Signup";
require "$BASE_FILES/inc_top.php";

$login = $HTTP_POST_VARS['login'];
$password = $HTTP_POST_VARS['password'];
$email = $HTTP_POST_VARS['email'];
$password2 = $HTTP_POST_VARS['password2'];
$fullname = $HTTP_POST_VARS['fullname'];
$gametype = $HTTP_POST_VARS['gametype'];
$blurb = $HTTP_POST_VARS['blurb'];
$showemail = $HTTP_POST_VARS['showemail'];
$showblurb = $HTTP_POST_VARS['showblurb'];
$sig = $HTTP_POST_VARS['sig'];
$website = $HTTP_POST_VARS['website'];
$icq = $HTTP_POST_VARS['icq'];
$aim = $HTTP_POST_VARS['aim'];
$yahoo = $HTTP_POST_VARS['yahoo'];
$msn = $HTTP_POST_VARS['msn'];
$occupation = $HTTP_POST_VARS['occuption'];
$confirm = $HTTP_POST_VARS['confirm_id'];
$confirmCode = $HTTP_POST_VARS['confirm_code'];
$confirmVersion = $HTTP_POST_VARS['confirm_version'];
$result = doPlayerSignUp($login, $password, $password2, $fullname, $email, $country, $platform, $gametype, $blurb, $showemail, $showblurb, $occupation, $sig, $interests, $website, $icq, $aim, $yahoo, $msn, $confirm, $confirmCode, $confirmVersion);
if ($result == "OK") {
	$error_heading="Signup Success";
	$error_body="Signup success. Click <A href=\"index.php\">here</A> to return to the main page";
} else {
	$error_body="<b>Error: </b>$result<P>Click the back button on your browser to correct the error and try again.";
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>

