<?
require "../php/header.php";
$name = $HTTP_POST_VARS['name'];
$confirm = $HTTP_POST_VARS['confirm'];
if ($userdata['session_logged_in']== false) {
	$error_body= "Sorry, you must be logged in to do this.";
	include "$BASE_FILES/inc_top.php";
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";
	exit;
} else if (checkPassword($name, $confirm, "EDITTEAM") == false) {
	$error_body= "Sorry, your confirmation password is missing or incorrect. Please press the back button on your web browser and correct the problem.";
	include "$BASE_FILES/inc_top.php";
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";
	exit;

} else {
        $editpassword = $HTTP_POST_VARS['editpassword'];
        $editpassword2 = $HTTP_POST_VARS['editpassword2'];
        $joinpassword = $HTTP_POST_VARS['joinpassword'];
        $joinpassword2 = $HTTP_POST_VARS['joinpassword2'];
        $blurb = $HTTP_POST_VARS['blurb'];
        $website = $HTTP_POST_VARS['website'];
	$result = doModifyTeam($name, $editpassword, $editpassword2, $joinpassword, $joinpassword2, $blurb, $website, $userdata['user_id']);
}

if ($result == "OK") {
	header("Location: teaminfo.php?name=" . $name);
	exit;
	
}

$error_body=$result;
$html_title="Winbolo.net: Modify Team";
require "$BASE_FILES/inc_top.php";
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>

