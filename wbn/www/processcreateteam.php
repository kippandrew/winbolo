<?
require "../php/header.php";
$html_title="Winbolo.net: Create Team";
require "$BASE_FILES/inc_top.php";
if ($userdata['session_logged_in'] == false) {
	$error_body="Sorry, you must be logged in to create teams.";
} else {
	$name = $HTTP_POST_VARS['name'];
	$editpassword = $HTTP_POST_VARS['editpassword'];
	$editpassword2 = $HTTP_POST_VARS['editpassword2'];
	$joinpassword = $HTTP_POST_VARS['joinpassword'];
	$joinpassword2 = $HTTP_POST_VARS['joinpassword2'];
	$blurb = $HTTP_POST_VARS['blurb'];
	$website = $HTTP_POST_VARS['website'];
	$result = doCreateTeam($userdata['username'], $name, $editpassword, $editpassword2, $joinpassword, $joinpassword2, $blurb, $website);
	if ($result == "OK") {
		$error_heading="Team Creation Success";
		$error_body="<br>Team creation success.";
	} else {
		$error_body="<br><b>Error:</b> " . $result . "<br><br>Click the back button on your browser to correct the error and try again.";
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>
