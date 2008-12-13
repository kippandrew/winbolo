<?
include "../php/header.php";
include "$BASE_FILES/mapimageclass.php";
$error = "OK";
if ($userdata['session_logged_in']== false) {
	$error = "Not logged in";
} else {
	openDatabase(false);
	$pid = $userdata['user_id'];
	$sql = "delete from map where pid = '$pid'";
	if (!mysql_query($sql)) {
		$error = "Database error";
	}
	closeDatabase();
}


if ($error == "OK") {
	header("Location: playermodify.php");
} else {
	include "$BASE_FILES/inc_top.php";
	echo "<tr><td>Error removing map: ". $error  . " </td></tr>";
	include "$BASE_FILES/inc_bottom.php";

}
?>
