<?
$html_title="WinBolo.net: What If Scoring Calculator";
require "../php/header.php";
include "$BASE_FILES/inc_top.php";


$playerA = $HTTP_GET_VARS['playera'];
$playerB = $HTTP_GET_VARS['playerb'];
$gameType = $HTTP_GET_VARS['gametype'];
$gt = 2;
if ($gameType != "tourn") {
	$gameType = "strict";
	$gt = 3;
}
$first = false;
$found = false;
$isTeam = false;

if ($HTTP_GET_VARS['team'] == 'teams') {
	$isTeam = true;
}

if ($playerA == "" && $playerB == "") {
	$first = true;
} else {
	if ($isTeam == true) {
		if ($playerA != "") {
			if (getTid($playerA) != -1) {
				$listA = array();
				$listA[0] = $playerA;
			} else {
				$listA = getTeams($playerA, 50);
			}
		}
		if ($playerB != "") {
			if (getTid($playerB) != -1) {
				$listB = array();
				$listB[0] = $playerB;
			} else {
				$listB = getTeams($playerB, 50);
			}
		}
	} else {
		if ($playerA != "") {
			if (getPid($playerA) != 0) {
				$listA = array();
				$listA[0] = $playerA;
			} else {
				$listA = getPlayers($playerA, 50);
			}
		}
		if ($playerB != "") {
			if (getPid($playerB) != 0) {
				$listB = array();
				$listB[0] = $playerB;
			} else {
				$listB = getPlayers($playerB, 50);
			}
		}
	}

	if (sizeof($listA) == 1 && sizeof($listB) == 1) {
		if ($listA[0] == $playerA && $listB[0] == $playerB) {
			#We have found a match
			$found = true;
			if ($isTeam == true) {
				$idA = getTid($listA[0]);
				$idB = getTid($listB[0]);
			} else {
				$idA = getPid($listA[0]);
				$idB = getPid($listB[0]);
			}
			$scoreA = getScore($idA, $gt, $isTeam);
			$scoreB = getScore($idB, $gt, $isTeam);
			$newScoreA_1 = floor(calculateElo($scoreA, $scoreB, true));
			$newScoreB_1 = floor(calculateElo($scoreB, $scoreA, false));
			$newScoreA_2 = floor(calculateElo($scoreA, $scoreB, false));
			$newScoreB_2 = floor(calculateElo($scoreB, $scoreA, true));
		}
	}
}
if ($found == true) {
	include "$BASE_FILES/inc_whatif_results.php";
} else {
	include "$BASE_FILES/inc_whatif.php";
}
include "$BASE_FILES/inc_bottom.php";
?>
