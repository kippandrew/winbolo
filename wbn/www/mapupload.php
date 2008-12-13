<?
include "../php/header.php";
include "$BASE_FILES/mapimageclass.php";
$error = "OK";
$map = new mapImageClass();
$mapfile = $HTTP_POST_FILES['mapfile'];
$blurb = $HTTP_POST_VARS['blurb'];
if ($userdata['session_logged_in'] == false) {
	$error = "Not logged in";
} else if (is_uploaded_file($mapfile['tmp_name']) == false) {
	$error = "File not uploaded";
} else {
	$map->setup();
	if ($map->readMap($mapfile['tmp_name']) == false) {
		$error = "Not a valid WinBolo Map";
	} else {
		$gif = $map->getImg();
		$tmpfname = tempnam ("/tmp", "IMG");
		$name = stripslashes(trim($mapfile['name']));
		
		$end = substr($name, -4);
		if (strstr(strtolower($end), ".map") != false) {
			# trim off end .map
			$name = substr($name, 0, -4);
		}
		$name = mysql_escape_string($name);

		ImagePng($gif, $tmpfname); 
		$sqlMapFile = addslashes(fread(fopen($mapfile['tmp_name'], "r"), filesize($mapfile['tmp_name'])));
		$sqlImgFile = addslashes(fread(fopen($tmpfname, "r"), filesize($tmpfname)));
		$pid = getPid($userdata['username']);
		$pills = $map->getNumPills();
		$bases = $map->getNumBases();
		$starts = $map->getNumStarts();
		$blurb = stripslashes($blurb);
		$blurb = mysql_escape_string($blurb);

		$sql = "delete from map where pid = '$pid'";
		if (!mysql_query($sql)) {
			$error = "Database error";
			unlink ($tmpfname);
		} else {
			$sql = "INSERT INTO map (pid , name , comment , map, img, bases, pills, starts) VALUES ('$pid', '$name', '$blurb', '$sqlMapFile', '$sqlImgFile', '$bases', '$pills', '$starts')";
			if (!mysql_query($sql)) {
				$error = "Database error";
			}
			unlink ($tmpfname);
		}
	}
}


if ($error == "OK") {
	header("Location: playermodify.php");
} else {
	include "$BASE_FILES/inc_top.php";
	$error_body = $error;
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";

}
?>
