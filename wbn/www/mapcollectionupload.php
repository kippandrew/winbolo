<?
include "../php/header.php";
include "$BASE_FILES/mapimageclass.php";
$error = "OK";
$map = new mapImageClass();
$mapfile = $HTTP_POST_FILES['mapfile'];
$comment = $HTTP_POST_VARS['blurb'];
$level = $HTTP_POST_VARS['id'];
if ($userdata['session_logged_in'] == false) {
	$error = "Not logged in";
} else if (isset($HTTP_POST_VARS['id']) == false) {
	$error = "Missing data";
} else if (mcGetOwnerFromLevelId($level) != $userdata['user_id']) {
	$error = "Not owner";
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
		$name = trim($name);
		$name = mysql_escape_string($name);

		ImagePng($gif, $tmpfname); 
		$sqlMapFile = addslashes(fread(fopen($mapfile['tmp_name'], "r"), filesize($mapfile['tmp_name'])));
		$sqlImgFile = addslashes(fread(fopen($tmpfname, "r"), filesize($tmpfname)));
		$pid = $userdata['user_id'];
		$pills = $map->getNumPills();
		$bases = $map->getNumBases();
		$starts = $map->getNumStarts();
		$comment = stripslashes($comment);
		$comment = mysql_escape_string($comment);
		$level = mysql_escape_string($level);
		$sql = "INSERT INTO map_meta (mm_level, mm_pid, mm_name , mm_comment , mm_bases, mm_pills, mm_starts) VALUES ('$level', '$pid', '$name', '$comment', '$bases', '$pills', '$starts')";
		echo $sql;
		if (!mysql_query($sql)) {
			$error = "Database error";
		} else {
			$id = mysql_insert_id();
			$sql = "insert into map_data (md_id, md_map, md_img) values ('$id', '$sqlMapFile', '$sqlImgFile')";
			mysql_query($sql);
		}
		unlink ($tmpfname);
	}
}


if ($error == "OK") {
	header("Location: mapcollection.php?id=$level");
} else {
	include "$BASE_FILES/inc_top.php";
	$error_body = $error;
	include "$BASE_FILES/inc_error.php";
	include "$BASE_FILES/inc_bottom.php";

}
?>
