<?
require "../php/header.php";
$html_title="Winbolo.net: Leave Team";
include "$BASE_FILES/inc_top.php";

$name = $HTTP_GET_VARS['name'];
if ($userdata['session_logged_in']== false) {
	$error_body="Sorry, you must be logged in to leave teams.";
} else {
	$result = doLeaveTeam($userdata['username'], $name);
	if ($result == "OK") {
		$error_heading="Leave Team";
		$error_body="Leave success!. Click <A href=\"index.php\">here</A> to return to the main page";
	} else {
		$error_body = $result . "<P>Click the back button on your browser to correct the error and try again.";
	}
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>

