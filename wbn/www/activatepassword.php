<?
require "../php/header.php";
$html_title="Winbolo.net: Activate Lost Password";
include "$BASE_FILES/inc_top.php";
$actkey = $HTTP_GET_VARS['actkey'];

$result = doActivatePassword($actkey);
if ($result == "OK") {
	$error_heading="Password Changed";
	$error_body='Your passsword has been successfully changed.';
} else {
	$error_body=$result;
}
include "$BASE_FILES/inc_error.php";
include "$BASE_FILES/inc_bottom.php";
?>

