<?
/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */	


class playerInfoClass {
  var $id;
  var $name;
  var $email;
  var $gametype;
  var $blurb;
  var $showblurb;
  var $showemail;
  var $openNumGames;
  var $openNumPills;
  var $openNumBases;
  var $openNumTanks;
  var $tournNumGames;
  var $tournNumWins;
  var $tournNumLoses;
  var $tournScore;
  var $tournNumPills;
  var $tournNumBases;
  var $tournNumTanks;
  var $tournRank;
  var $strictNumGames;
  var $strictScore;
  var $strictNumWins;
  var $strictNumLoses;
  var $strictNumPills;
  var $strictNumBases;
  var $strictNumTanks;
  var $strictRank;
  var $teams;
  var $icq;
  var $aim;
  var $yahoo;
  var $msn;
  var $website;
  var $occupation;
  var $numPosts;
  var $percentPosts;
  var $postsPerDay;
  var $registerDate;
  var $sig;
  var $activeGames;
  var $views;
  var $onIrc;
  var $hasMapCollection;
  var $emailforward;
  var $showforward;

  function setStuff($id, $name, $email, $gametype, $blurb, $showblurb, $showemail, $openNumGames, $openNumPills, $openNumBases, $openNumTanks, $tournNumGames, $tournNumWins, $tournNumLoses, $tournScore, $tournNumPills, $tournNumBases, $tournNumTanks, $tournRank, $strictNumGames, $strictScore, $strictNumWins, $strictNumLoses, $strictNumPills, $strictNumBases, $strictNumTanks, $strictRank, $icq, $aim, $yahoo, $website, $occupation, $msn, $regDate, $sig, $hasMapCollection, $emailforward, $showforward) {
  $this->id = $id;
  $this->name = $name;
  $this->email = $email;
  $this->gametype = $gametype;
  $this->blurb = $blurb;
  $this->showemail = $showemail;
  $this->showblurb = $showblurb;
  $this->openNumGames = $openNumGames;
  $this->openNumPills = $openNumPills;
  $this->openNumBases = $openNumBases;
  $this->openNumTanks = $openNumTanks;
  $this->tournNumGames = $tournNumGames;
  $this->tournNumWins = $tournNumWins;
  $this->tournNumLoses = $tournNumLoses;
  $this->tournScore = $tournScore;
  $this->tournNumPills = $tournNumPills;
  $this->tournNumBases = $tournNumBases;
  $this->tournNumTanks = $tournNumTanks;
  $this->tournRank = $tournRank;
  $this->strictNumGames = $strictNumGames;
  $this->strictScore = $strictScore;
  $this->strictNumWins = $strictNumWins;
  $this->strictNumLoses = $strictNumLoses;
  $this->strictNumPills = $strictNumPills;
  $this->strictNumBases = $strictNumBases;
  $this->strictNumTanks = $strictNumTanks;
  $this->strictRank = $strictRank;
  $this->icq = $icq;
  $this->aim = $aim;
  $this->yahoo = $yahoo;
  $this->msn = $msn;
  $this->website = $website;
  $this->occupation = $occupation;
  $this->percentPosts = 0;
  $this->postsPerDay = 0;
  $this->registeredDate = 0;
  $this->registerDate = $regDate;
  $this->sig = $sig;
  $this->hasMapCollection = $hasMapCollection;
  $this->emailforward = $emailforward;
  $this->showforward = $showforward;
}

  function getHasMapCollection() {
    return $this->hasMapCollection;
  }

  function getEmailForward() {
  return $this->emailforward;
  }
  function getShowForward() {
  return $this->showforward;
  }

  function setOnIrc($irc) {
    $this->onIrc = $irc;
  }

  function getOnIrc() {
	return $this->onIrc;
  }

  function getId() {
    return $this->id;
  }
  function getName() {
    return $this->name;
  }
  function getEmail() {
  return $this->email;
  }
  function getGameType() {
  return $this->gametype;
  }
  function getBlurb() {
  return $this->blurb;
  }
  function getShowBlurb() {
  return $this->showblurb;
  }
  function getShowEmail() {
  return $this->showemail;
  }
  function getOpenNumGames() {
  return $this->openNumGames;
  }
  function getOpenNumPills() {
  return $this->openNumPills;
  }
  function getOpenNumBases() {
  return $this->openNumBases;
  }
  function getOpenNumTanks() {
  return $this->openNumTanks;
  }
  function getTournNumGames() {
  return $this->tournNumGames;
  }
  function getTournNumWins() {
  return $this->tournNumWins;
  }
  function getTournNumLoses() {
  return $this->tournNumLoses;
  }
  function getTournScore() {
  return $this->tournScore;
  }
  function getTournNumPills() {
  return $this->tournNumPills;
  }
  function getTournNumBases() {
  return $this->tournNumBases;
  }
  function getTournNumTanks() {
  return $this->tournNumTanks;
  }
  function getTournRank() {
  return $this->tournRank ;
  }
  function getStrictNumGames() {
  return $this->strictNumGames;
  }
  function getStrictScore() {
  return $this->strictScore;
  }
  function getStrictNumWins() {
  return $this->strictNumWins;
  }
  function getStrictNumLoses() {
  return $this->strictNumLoses;
  }
  function getStrictNumPills() {
  return $this->strictNumPills;
  }
  function getStrictNumBases() {
  return $this->strictNumBases;
  }
  function getStrictNumTanks() {
  return $this->strictNumTanks;
  }
  function getStrictRank() {
  return $this->strictRank;
  }

  function getIcq() {
  return $this->icq;
  }
  function getAim() {
    return $this->aim;
  }
  function getYahoo() {
    return $this->yahoo;
  }
  function getWebsite() {
  return $this->website;
  }
  function getOccupation() {
    return $this->occupation;
  }
 
  function setNumPosts($posts) {
    $this->numPosts = $posts;
  }
  function getNumPosts() {
    return $this->numPosts;
  }
  function setTeams($teams) {
    $this->teams = $teams;
  }
  function getTeams() {
    return $this->teams;
  }
 function getMsn() {
    return $this->msn;
 }

 function getPercentPosts() {
   return $this->percentPosts;
 }

 function getPostsPerDay() {
   return $this->postsPerDay;
 }
 function getRegisteredDate() {
   return gmdate("D M d, Y g:i a", $this->registerDate + (10 * 3600));
 }
 function getSig() {
  return $this->sig;
 }

function setPageViews($views) {
  $this->views = $views;
}

function getPageViews() {
  return $this->views;
}

 function setPercentPosts($posts) {
   $this->percentPosts = $posts;
 }

 function setPostsPerDay($posts) {
   $this->postsPerDay = $posts;
 }

 function setTournRank($rank) {
	$this->tournRank = $rank;
 }
 function setStrictRank($rank) {
  $this->strictRank = $rank;
 }

 function setActiveGames($games) {
 	$this->activeGames = $games;
 }

 function getActiveGames() {
 	return $this->activeGames;
 }

}

?>
