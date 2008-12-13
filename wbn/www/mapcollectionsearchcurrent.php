<?
require "../php/header.php";
$name = $HTTP_GET_VARS['name'];
if (isset($HTTP_GET_VARS['name']) && strlen($name) > 0) {
	#name search
	$rating = false;
	$useName = true;
	$useDate = false;
	if ($inOrder == "newest") {
		$order = false;
	} else {
		$order = true;
	}
} else {
	header("Location: mapcollection.php");
	exit(0);
}

$results = mcSearchResults($name, true, false, false, false);
$len = sizeof($results);
if ($len == 1) {
	$md = $results[0];
	header("Location: mapcollectioninfo.php?id=" . $md->getId());
} else {
	$html_title="WinBolo.net: Map Collections";
	require "$BASE_FILES/inc_mapcollection_top.php";
	require "$BASE_FILES/inc_mapcollection_searchresults.php";
	require "$BASE_FILES/inc_mapcollection_bottom.php";
}
?>

