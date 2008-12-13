<?
require "../php/header.php";
$html_title="Winbolo.net: Modify Account";
require "$BASE_FILES/inc_top.php";
if ($userdata['session_logged_in'] == false) {
	$error_body = "You must login before you can use this page.";
} else {
        $email = $HTTP_POST_VARS['email'];
	$password = $HTTP_POST_VARS['password'];
	$password2 = $HTTP_POST_VARS['password2'];
	$gametype = $HTTP_POST_VARS['$gametype'];
	$blurb = $HTTP_POST_VARS['blurb'];
	$showemail = $HTTP_POST_VARS['showemail'];
	$showforward = $HTTP_POST_VARS['showforward'];
	$showblurb = $HTTP_POST_VARS['showblurb'];
	$sig = $HTTP_POST_VARS['sig'];
	$website = $HTTP_POST_VARS['website'];
	$icq = $HTTP_POST_VARS['icq'];
	$aim = $HTTP_POST_VARS['aim'];
	$yahoo = $HTTP_POST_VARS['yahoo'];
	$msn = $HTTP_POST_VARS['msn'];
	$occupation = $HTTP_POST_VARS['occupation'];
$result = doPlayerModify($userdata['username'], $email, $password, $password2, $gametype, $blurb, $showemail, $showblurb, $sig, $website, $icq, $aim, $yahoo, $msn, $occupation, $showforward);
	if ($result == "OK") {
		$error_heading = "Player Modified";
		$error_body="Profile successfully modified. Click <A href='index.php'>here</A> to return to the main page";
	} else {
		$error_body = "<b>Error:</b>" . $result;
		$error_body .= "<P>Click the back button on your browser to correct the error and try again.";
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
