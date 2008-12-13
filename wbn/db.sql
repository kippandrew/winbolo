-- MySQL dump 10.11
--
-- Host: localhost    Database: winbolo
-- ------------------------------------------------------
-- Server version	5.0.32-Debian_7etch8

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `active_player`
--

DROP TABLE IF EXISTS `active_player`;
CREATE TABLE `active_player` (
  `ap_pid` int(11) NOT NULL default '0',
  `ap_key` char(32) NOT NULL default '',
  `ap_serverkey` char(32) NOT NULL default '',
  `ap_left` int(11) NOT NULL default '0',
  UNIQUE KEY `ap_key` (`ap_key`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `active_team`
--

DROP TABLE IF EXISTS `active_team`;
CREATE TABLE `active_team` (
  `at_serverkey` char(32) default NULL,
  `at_key` char(32) default NULL,
  `at_team_number` int(11) default NULL,
  `at_pid` int(11) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `archived_game_event`
--

DROP TABLE IF EXISTS `archived_game_event`;
CREATE TABLE `archived_game_event` (
  `age_serverkey` char(32) NOT NULL default '',
  `age_eventtype` int(11) NOT NULL default '0',
  `age_playera` int(11) default NULL,
  `age_playerb` int(11) default NULL,
  `age_timestamp` int(11) NOT NULL default '0',
  KEY `archived_game_event_serverkey_index` (`age_serverkey`),
  KEY `ags_eventplayerindex` (`age_eventtype`,`age_playera`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `archived_game_server`
--

DROP TABLE IF EXISTS `archived_game_server`;
CREATE TABLE `archived_game_server` (
  `ags_ip` varchar(255) default NULL,
  `ags_port` int(11) NOT NULL default '0',
  `ags_mapname` varchar(36) NOT NULL default '',
  `ags_password` char(1) NOT NULL default '',
  `ags_numbases` int(11) NOT NULL default '0',
  `ags_numpills` int(11) NOT NULL default '0',
  `ags_mines` char(1) NOT NULL default '',
  `ags_freebases` int(11) NOT NULL default '0',
  `ags_freepills` int(11) NOT NULL default '0',
  `ags_numtanks` int(11) NOT NULL default '0',
  `ags_starttime` int(11) NOT NULL default '0',
  `ags_firstheard` int(11) NOT NULL default '0',
  `ags_lastheard` int(11) NOT NULL default '0',
  `ags_ai` char(1) NOT NULL default '',
  `ags_gametype` char(1) NOT NULL default '',
  `ags_version` varchar(4) default NULL,
  `ags_log_available` char(1) default 'N',
  `ags_log_size` int(11) default NULL,
  `ags_log_downloads` int(11) default NULL,
  `ags_log_rating` double default '0',
  `ags_log_numratings` int(11) default '0',
  `ags_key` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`ags_key`),
  KEY `archived_game_server_time_index` (`ags_lastheard`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `auth_access`
--

DROP TABLE IF EXISTS `auth_access`;
CREATE TABLE `auth_access` (
  `group_id` mediumint(8) NOT NULL default '0',
  `forum_id` smallint(5) unsigned NOT NULL default '0',
  `auth_view` tinyint(1) NOT NULL default '0',
  `auth_read` tinyint(1) NOT NULL default '0',
  `auth_post` tinyint(1) NOT NULL default '0',
  `auth_reply` tinyint(1) NOT NULL default '0',
  `auth_edit` tinyint(1) NOT NULL default '0',
  `auth_delete` tinyint(1) NOT NULL default '0',
  `auth_sticky` tinyint(1) NOT NULL default '0',
  `auth_announce` tinyint(1) NOT NULL default '0',
  `auth_vote` tinyint(1) NOT NULL default '0',
  `auth_pollcreate` tinyint(1) NOT NULL default '0',
  `auth_attachments` tinyint(1) NOT NULL default '0',
  `auth_mod` tinyint(1) NOT NULL default '0',
  KEY `group_id` (`group_id`),
  KEY `forum_id` (`forum_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `banlist`
--

DROP TABLE IF EXISTS `banlist`;
CREATE TABLE `banlist` (
  `ban_id` mediumint(8) unsigned NOT NULL auto_increment,
  `ban_userid` mediumint(8) NOT NULL default '0',
  `ban_ip` varchar(8) NOT NULL default '',
  `ban_email` varchar(255) default NULL,
  PRIMARY KEY  (`ban_id`),
  KEY `ban_ip_user_id` (`ban_ip`,`ban_userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `categories`
--

DROP TABLE IF EXISTS `categories`;
CREATE TABLE `categories` (
  `cat_id` mediumint(8) unsigned NOT NULL auto_increment,
  `cat_title` varchar(100) default NULL,
  `cat_order` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cat_id`),
  KEY `cat_order` (`cat_order`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Table structure for table `config`
--

DROP TABLE IF EXISTS `config`;
CREATE TABLE `config` (
  `config_name` varchar(255) NOT NULL default '',
  `config_value` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`config_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `confirm`
--

DROP TABLE IF EXISTS `confirm`;
CREATE TABLE `confirm` (
  `confirm_id` char(32) NOT NULL default '',
  `session_id` char(32) NOT NULL default '',
  `code` char(6) NOT NULL default '',
  PRIMARY KEY  (`session_id`,`confirm_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `disallow`
--

DROP TABLE IF EXISTS `disallow`;
CREATE TABLE `disallow` (
  `disallow_id` mediumint(8) unsigned NOT NULL auto_increment,
  `disallow_username` varchar(25) NOT NULL default '',
  PRIMARY KEY  (`disallow_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `forum_prune`
--

DROP TABLE IF EXISTS `forum_prune`;
CREATE TABLE `forum_prune` (
  `prune_id` mediumint(8) unsigned NOT NULL auto_increment,
  `forum_id` smallint(5) unsigned NOT NULL default '0',
  `prune_days` smallint(5) unsigned NOT NULL default '0',
  `prune_freq` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`prune_id`),
  KEY `forum_id` (`forum_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `forums`
--

DROP TABLE IF EXISTS `forums`;
CREATE TABLE `forums` (
  `forum_id` smallint(5) unsigned NOT NULL default '0',
  `cat_id` mediumint(8) unsigned NOT NULL default '0',
  `forum_name` varchar(150) default NULL,
  `forum_desc` text,
  `forum_status` tinyint(4) NOT NULL default '0',
  `forum_order` mediumint(8) unsigned NOT NULL default '1',
  `forum_posts` mediumint(8) unsigned NOT NULL default '0',
  `forum_topics` mediumint(8) unsigned NOT NULL default '0',
  `forum_last_post_id` mediumint(8) unsigned NOT NULL default '0',
  `prune_next` int(11) default NULL,
  `prune_enable` tinyint(1) NOT NULL default '0',
  `auth_view` tinyint(2) NOT NULL default '0',
  `auth_read` tinyint(2) NOT NULL default '0',
  `auth_post` tinyint(2) NOT NULL default '0',
  `auth_reply` tinyint(2) NOT NULL default '0',
  `auth_edit` tinyint(2) NOT NULL default '0',
  `auth_delete` tinyint(2) NOT NULL default '0',
  `auth_sticky` tinyint(2) NOT NULL default '0',
  `auth_announce` tinyint(2) NOT NULL default '0',
  `auth_vote` tinyint(2) NOT NULL default '0',
  `auth_pollcreate` tinyint(2) NOT NULL default '0',
  `auth_attachments` tinyint(2) NOT NULL default '0',
  PRIMARY KEY  (`forum_id`),
  KEY `forums_order` (`forum_order`),
  KEY `cat_id` (`cat_id`),
  KEY `forum_last_post_id` (`forum_last_post_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `game_event`
--

DROP TABLE IF EXISTS `game_event`;
CREATE TABLE `game_event` (
  `ge_serverkey` varchar(32) NOT NULL default '',
  `ge_eventtype` int(11) NOT NULL default '0',
  `ge_playera` varchar(32) default NULL,
  `ge_playerb` varchar(32) default NULL,
  `ge_timestamp` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `game_server`
--

DROP TABLE IF EXISTS `game_server`;
CREATE TABLE `game_server` (
  `gs_key` varchar(32) default NULL,
  `gs_ip` varchar(255) default NULL,
  `gs_port` int(11) NOT NULL default '0',
  `gs_mapname` varchar(36) NOT NULL default '',
  `gs_password` char(1) NOT NULL default '',
  `gs_mines` char(1) NOT NULL default '',
  `gs_numbases` int(11) NOT NULL default '0',
  `gs_numpills` int(11) NOT NULL default '0',
  `gs_freebases` int(11) NOT NULL default '0',
  `gs_freepills` int(11) NOT NULL default '0',
  `gs_numtanks` int(11) NOT NULL default '0',
  `gs_starttime` int(11) NOT NULL default '0',
  `gs_firstheard` int(11) NOT NULL default '0',
  `gs_lastheard` int(11) NOT NULL default '0',
  `gs_ai` char(1) NOT NULL default '',
  `gs_gametype` char(1) NOT NULL default '',
  `gs_version` varchar(4) default NULL,
  `gs_locked` char(1) default '0',
  UNIQUE KEY `gs_key` (`gs_key`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `groups`
--

DROP TABLE IF EXISTS `groups`;
CREATE TABLE `groups` (
  `group_id` mediumint(8) NOT NULL auto_increment,
  `group_type` tinyint(4) NOT NULL default '1',
  `group_name` varchar(40) NOT NULL default '',
  `group_description` varchar(255) NOT NULL default '',
  `group_moderator` mediumint(8) NOT NULL default '0',
  `group_single_user` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`group_id`),
  KEY `group_single_user` (`group_single_user`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

--
-- Table structure for table `journal`
--

DROP TABLE IF EXISTS `journal`;
CREATE TABLE `journal` (
  `j_key` char(32) NOT NULL default '',
  `j_pid` mediumint(8) NOT NULL default '0',
  PRIMARY KEY  (`j_key`,`j_pid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `log_comment`
--

DROP TABLE IF EXISTS `log_comment`;
CREATE TABLE `log_comment` (
  `lc_id` int(11) NOT NULL auto_increment,
  `lc_key` varchar(32) NOT NULL default '',
  `lc_pid` int(11) NOT NULL default '0',
  `lc_rating` tinyint(4) default '0',
  `lc_timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `lc_comment` blob,
  PRIMARY KEY  (`lc_id`),
  KEY `log_comment_key_index` (`lc_key`),
  KEY `log_comment_pid_index` (`lc_pid`)
) ENGINE=MyISAM AUTO_INCREMENT=80 DEFAULT CHARSET=latin1;

--
-- Table structure for table `map`
--

DROP TABLE IF EXISTS `map`;
CREATE TABLE `map` (
  `pid` int(11) default NULL,
  `name` varchar(255) default NULL,
  `comment` blob,
  `map` blob,
  `img` blob,
  `bases` int(11) default NULL,
  `pills` int(11) default NULL,
  `starts` int(11) default NULL,
  UNIQUE KEY `pid` (`pid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `map_comment`
--

DROP TABLE IF EXISTS `map_comment`;
CREATE TABLE `map_comment` (
  `mc_id` int(11) NOT NULL auto_increment,
  `mc_mapid` int(11) NOT NULL default '0',
  `mc_pid` int(11) NOT NULL default '0',
  `mC_rating` tinyint(4) NOT NULL default '0',
  `mc_timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `mC_comment` blob,
  PRIMARY KEY  (`mc_id`),
  KEY `mc_mapid` (`mc_mapid`),
  KEY `mc_mapid_index` (`mc_mapid`)
) ENGINE=MyISAM AUTO_INCREMENT=3797 DEFAULT CHARSET=latin1;

--
-- Table structure for table `map_data`
--

DROP TABLE IF EXISTS `map_data`;
CREATE TABLE `map_data` (
  `md_id` int(11) NOT NULL default '0',
  `md_map` blob,
  `md_img` blob,
  PRIMARY KEY  (`md_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `map_meta`
--

DROP TABLE IF EXISTS `map_meta`;
CREATE TABLE `map_meta` (
  `mm_id` int(11) NOT NULL auto_increment,
  `mm_level` int(11) default NULL,
  `mm_pid` int(11) default NULL,
  `mm_name` varchar(255) default NULL,
  `mm_bases` int(11) default NULL,
  `mm_pills` int(11) default NULL,
  `mm_starts` int(11) default NULL,
  `mm_rating` double default '0',
  `mm_timestame` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `mm_comment` blob,
  `mm_downloads` int(11) default '0',
  PRIMARY KEY  (`mm_id`),
  KEY `map_meta_level_index` (`mm_level`)
) ENGINE=MyISAM AUTO_INCREMENT=7304 DEFAULT CHARSET=latin1;

--
-- Table structure for table `map_structure`
--

DROP TABLE IF EXISTS `map_structure`;
CREATE TABLE `map_structure` (
  `ms_id` int(11) NOT NULL auto_increment,
  `ms_pid` int(11) default NULL,
  `ms_name` varchar(64) default NULL,
  `ms_parent` int(11) default NULL,
  `ms_timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `ms_comment` blob,
  PRIMARY KEY  (`ms_id`),
  KEY `map_structure_pid_index` (`ms_pid`),
  KEY `map_structure_parent_index` (`ms_parent`)
) ENGINE=MyISAM AUTO_INCREMENT=742 DEFAULT CHARSET=latin1;

--
-- Table structure for table `posts`
--

DROP TABLE IF EXISTS `posts`;
CREATE TABLE `posts` (
  `post_id` mediumint(8) unsigned NOT NULL auto_increment,
  `topic_id` mediumint(8) unsigned NOT NULL default '0',
  `forum_id` smallint(5) unsigned NOT NULL default '0',
  `poster_id` mediumint(8) NOT NULL default '0',
  `post_time` int(11) NOT NULL default '0',
  `poster_ip` varchar(8) NOT NULL default '',
  `post_username` varchar(25) default NULL,
  `enable_bbcode` tinyint(1) NOT NULL default '1',
  `enable_html` tinyint(1) NOT NULL default '0',
  `enable_smilies` tinyint(1) NOT NULL default '1',
  `enable_sig` tinyint(1) NOT NULL default '1',
  `post_edit_time` int(11) default NULL,
  `post_edit_count` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`post_id`),
  KEY `forum_id` (`forum_id`),
  KEY `topic_id` (`topic_id`),
  KEY `poster_id` (`poster_id`),
  KEY `post_time` (`post_time`)
) ENGINE=MyISAM AUTO_INCREMENT=29918 DEFAULT CHARSET=latin1;

--
-- Table structure for table `posts_text`
--

DROP TABLE IF EXISTS `posts_text`;
CREATE TABLE `posts_text` (
  `post_id` mediumint(8) unsigned NOT NULL default '0',
  `bbcode_uid` varchar(10) NOT NULL default '',
  `post_subject` varchar(60) default NULL,
  `post_text` text,
  PRIMARY KEY  (`post_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `privmsgs`
--

DROP TABLE IF EXISTS `privmsgs`;
CREATE TABLE `privmsgs` (
  `privmsgs_id` mediumint(8) unsigned NOT NULL auto_increment,
  `privmsgs_type` tinyint(4) NOT NULL default '0',
  `privmsgs_subject` varchar(255) NOT NULL default '0',
  `privmsgs_from_userid` mediumint(8) NOT NULL default '0',
  `privmsgs_to_userid` mediumint(8) NOT NULL default '0',
  `privmsgs_date` int(11) NOT NULL default '0',
  `privmsgs_ip` varchar(8) NOT NULL default '',
  `privmsgs_enable_bbcode` tinyint(1) NOT NULL default '1',
  `privmsgs_enable_html` tinyint(1) NOT NULL default '0',
  `privmsgs_enable_smilies` tinyint(1) NOT NULL default '1',
  `privmsgs_attach_sig` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`privmsgs_id`),
  KEY `privmsgs_from_userid` (`privmsgs_from_userid`),
  KEY `privmsgs_to_userid` (`privmsgs_to_userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `privmsgs_text`
--

DROP TABLE IF EXISTS `privmsgs_text`;
CREATE TABLE `privmsgs_text` (
  `privmsgs_text_id` mediumint(8) unsigned NOT NULL default '0',
  `privmsgs_bbcode_uid` varchar(10) NOT NULL default '0',
  `privmsgs_text` text,
  PRIMARY KEY  (`privmsgs_text_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ranks`
--

DROP TABLE IF EXISTS `ranks`;
CREATE TABLE `ranks` (
  `rank_id` smallint(5) unsigned NOT NULL auto_increment,
  `rank_title` varchar(50) NOT NULL default '',
  `rank_min` mediumint(8) NOT NULL default '0',
  `rank_special` tinyint(1) default '0',
  `rank_image` varchar(255) default NULL,
  PRIMARY KEY  (`rank_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `search_results`
--

DROP TABLE IF EXISTS `search_results`;
CREATE TABLE `search_results` (
  `search_id` int(11) unsigned NOT NULL default '0',
  `session_id` varchar(32) NOT NULL default '',
  `search_array` text NOT NULL,
  `search_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`search_id`),
  KEY `session_id` (`session_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `search_wordlist`
--

DROP TABLE IF EXISTS `search_wordlist`;
CREATE TABLE `search_wordlist` (
  `word_text` varchar(50) character set latin1 collate latin1_bin NOT NULL default '',
  `word_id` mediumint(8) unsigned NOT NULL auto_increment,
  `word_common` tinyint(1) unsigned NOT NULL default '0',
  PRIMARY KEY  (`word_text`),
  KEY `word_id` (`word_id`)
) ENGINE=MyISAM AUTO_INCREMENT=57955 DEFAULT CHARSET=latin1;

--
-- Table structure for table `search_wordmatch`
--

DROP TABLE IF EXISTS `search_wordmatch`;
CREATE TABLE `search_wordmatch` (
  `post_id` mediumint(8) unsigned NOT NULL default '0',
  `word_id` mediumint(8) unsigned NOT NULL default '0',
  `title_match` tinyint(1) NOT NULL default '0',
  KEY `post_id` (`post_id`),
  KEY `word_id` (`word_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `sessions`
--

DROP TABLE IF EXISTS `sessions`;
CREATE TABLE `sessions` (
  `session_id` char(32) NOT NULL default '',
  `session_user_id` mediumint(8) NOT NULL default '0',
  `session_start` int(11) NOT NULL default '0',
  `session_time` int(11) NOT NULL default '0',
  `session_ip` char(8) NOT NULL default '0',
  `session_page` int(11) NOT NULL default '0',
  `session_logged_in` tinyint(1) NOT NULL default '0',
  `session_admin` tinyint(2) NOT NULL default '0',
  PRIMARY KEY  (`session_id`),
  KEY `session_user_id` (`session_user_id`),
  KEY `session_id_ip_user_id` (`session_id`,`session_ip`,`session_user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `sessions_keys`
--

DROP TABLE IF EXISTS `sessions_keys`;
CREATE TABLE `sessions_keys` (
  `key_id` varchar(32) NOT NULL default '0',
  `user_id` mediumint(8) NOT NULL default '0',
  `last_ip` varchar(8) NOT NULL default '0',
  `last_login` int(11) NOT NULL default '0',
  PRIMARY KEY  (`key_id`,`user_id`),
  KEY `last_login` (`last_login`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `smilies`
--

DROP TABLE IF EXISTS `smilies`;
CREATE TABLE `smilies` (
  `smilies_id` smallint(5) unsigned NOT NULL auto_increment,
  `code` varchar(50) default NULL,
  `smile_url` varchar(100) default NULL,
  `emoticon` varchar(75) default NULL,
  PRIMARY KEY  (`smilies_id`)
) ENGINE=MyISAM AUTO_INCREMENT=43 DEFAULT CHARSET=latin1;

--
-- Table structure for table `spam`
--

DROP TABLE IF EXISTS `spam`;
CREATE TABLE `spam` (
  `user_id` mediumint(8) NOT NULL default '0',
  `user_emailforward` char(1) default '0',
  `post_count` bigint(21) NOT NULL default '0',
  `map_count` bigint(21) default NULL,
  `game_count` bigint(21) default NULL,
  `team_count` int(11) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `spam2`
--

DROP TABLE IF EXISTS `spam2`;
CREATE TABLE `spam2` (
  `user_id` mediumint(8) NOT NULL default '0',
  UNIQUE KEY `spam_index` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `stats_player`
--

DROP TABLE IF EXISTS `stats_player`;
CREATE TABLE `stats_player` (
  `pid` int(10) NOT NULL default '0',
  `open_num_games` int(11) default '0',
  `open_num_bases` int(11) default '0',
  `open_num_pills` int(11) default '0',
  `open_num_tanks` int(11) default '0',
  `tourn_num_games` int(11) default '0',
  `tourn_score` int(11) default '0',
  `tourn_wins` int(11) default '0',
  `tourn_loses` int(11) default '0',
  `tourn_num_bases` int(11) default '0',
  `tourn_num_pills` int(11) default '0',
  `tourn_num_tanks` int(11) default '0',
  `strict_num_games` int(11) default '0',
  `strict_score` int(11) default '0',
  `strict_wins` int(11) default '0',
  `strict_loses` int(11) default '0',
  `strict_num_bases` int(11) default '0',
  `strict_num_pills` int(11) default '0',
  `strict_num_tanks` int(11) default '0',
  `locked` char(1) NOT NULL default '',
  PRIMARY KEY  (`pid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `stats_team`
--

DROP TABLE IF EXISTS `stats_team`;
CREATE TABLE `stats_team` (
  `tid` int(11) NOT NULL auto_increment,
  `name` varchar(32) NOT NULL default '',
  `owner_password` varchar(32) NOT NULL default '',
  `join_password` varchar(32) NOT NULL default '',
  `owner` int(11) NOT NULL default '0',
  `blurb` blob,
  `open_num_games` int(11) default '0',
  `open_num_bases` int(11) default '0',
  `open_num_pills` int(11) default '0',
  `open_num_tanks` int(11) default '0',
  `tournnum_games` int(11) default '0',
  `tourn_score` int(11) default '0',
  `tourn_wins` int(11) default '0',
  `tourn_loses` int(11) default '0',
  `tourn_num_bases` int(11) default '0',
  `tourn_num_pills` int(11) default '0',
  `tourn_num_tanks` int(11) default '0',
  `strict_num_games` int(11) default '0',
  `strict_score` int(11) default '0',
  `strict_wins` int(11) default '0',
  `strict_loses` int(11) default '0',
  `strict_num_bases` int(11) default '0',
  `strict_num_pills` int(11) default '0',
  `strict_num_tanks` int(11) default '0',
  `website` varchar(100) default NULL,
  `regdate` varchar(20) default NULL,
  PRIMARY KEY  (`tid`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=661 DEFAULT CHARSET=latin1;

--
-- Table structure for table `team_members`
--

DROP TABLE IF EXISTS `team_members`;
CREATE TABLE `team_members` (
  `tid` int(11) default NULL,
  `pid` int(11) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `themes`
--

DROP TABLE IF EXISTS `themes`;
CREATE TABLE `themes` (
  `themes_id` mediumint(8) unsigned NOT NULL auto_increment,
  `template_name` varchar(30) NOT NULL default '',
  `style_name` varchar(30) NOT NULL default '',
  `head_stylesheet` varchar(100) default NULL,
  `body_background` varchar(100) default NULL,
  `body_bgcolor` varchar(6) default NULL,
  `body_text` varchar(6) default NULL,
  `body_link` varchar(6) default NULL,
  `body_vlink` varchar(6) default NULL,
  `body_alink` varchar(6) default NULL,
  `body_hlink` varchar(6) default NULL,
  `tr_color1` varchar(6) default NULL,
  `tr_color2` varchar(6) default NULL,
  `tr_color3` varchar(6) default NULL,
  `tr_class1` varchar(25) default NULL,
  `tr_class2` varchar(25) default NULL,
  `tr_class3` varchar(25) default NULL,
  `th_color1` varchar(6) default NULL,
  `th_color2` varchar(6) default NULL,
  `th_color3` varchar(6) default NULL,
  `th_class1` varchar(25) default NULL,
  `th_class2` varchar(25) default NULL,
  `th_class3` varchar(25) default NULL,
  `td_color1` varchar(6) default NULL,
  `td_color2` varchar(6) default NULL,
  `td_color3` varchar(6) default NULL,
  `td_class1` varchar(25) default NULL,
  `td_class2` varchar(25) default NULL,
  `td_class3` varchar(25) default NULL,
  `fontface1` varchar(50) default NULL,
  `fontface2` varchar(50) default NULL,
  `fontface3` varchar(50) default NULL,
  `fontsize1` tinyint(4) default NULL,
  `fontsize2` tinyint(4) default NULL,
  `fontsize3` tinyint(4) default NULL,
  `fontcolor1` varchar(6) default NULL,
  `fontcolor2` varchar(6) default NULL,
  `fontcolor3` varchar(6) default NULL,
  `span_class1` varchar(25) default NULL,
  `span_class2` varchar(25) default NULL,
  `span_class3` varchar(25) default NULL,
  `img_size_poll` smallint(5) unsigned default NULL,
  `img_size_privmsg` smallint(5) unsigned default NULL,
  PRIMARY KEY  (`themes_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `themes_name`
--

DROP TABLE IF EXISTS `themes_name`;
CREATE TABLE `themes_name` (
  `themes_id` smallint(5) unsigned NOT NULL default '0',
  `tr_color1_name` char(50) default NULL,
  `tr_color2_name` char(50) default NULL,
  `tr_color3_name` char(50) default NULL,
  `tr_class1_name` char(50) default NULL,
  `tr_class2_name` char(50) default NULL,
  `tr_class3_name` char(50) default NULL,
  `th_color1_name` char(50) default NULL,
  `th_color2_name` char(50) default NULL,
  `th_color3_name` char(50) default NULL,
  `th_class1_name` char(50) default NULL,
  `th_class2_name` char(50) default NULL,
  `th_class3_name` char(50) default NULL,
  `td_color1_name` char(50) default NULL,
  `td_color2_name` char(50) default NULL,
  `td_color3_name` char(50) default NULL,
  `td_class1_name` char(50) default NULL,
  `td_class2_name` char(50) default NULL,
  `td_class3_name` char(50) default NULL,
  `fontface1_name` char(50) default NULL,
  `fontface2_name` char(50) default NULL,
  `fontface3_name` char(50) default NULL,
  `fontsize1_name` char(50) default NULL,
  `fontsize2_name` char(50) default NULL,
  `fontsize3_name` char(50) default NULL,
  `fontcolor1_name` char(50) default NULL,
  `fontcolor2_name` char(50) default NULL,
  `fontcolor3_name` char(50) default NULL,
  `span_class1_name` char(50) default NULL,
  `span_class2_name` char(50) default NULL,
  `span_class3_name` char(50) default NULL,
  PRIMARY KEY  (`themes_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `topics`
--

DROP TABLE IF EXISTS `topics`;
CREATE TABLE `topics` (
  `topic_id` mediumint(8) unsigned NOT NULL auto_increment,
  `forum_id` smallint(8) unsigned NOT NULL default '0',
  `topic_title` char(60) NOT NULL default '',
  `topic_poster` mediumint(8) NOT NULL default '0',
  `topic_time` int(11) NOT NULL default '0',
  `topic_views` mediumint(8) unsigned NOT NULL default '0',
  `topic_replies` mediumint(8) unsigned NOT NULL default '0',
  `topic_status` tinyint(3) NOT NULL default '0',
  `topic_vote` tinyint(1) NOT NULL default '0',
  `topic_type` tinyint(3) NOT NULL default '0',
  `topic_first_post_id` mediumint(8) unsigned NOT NULL default '0',
  `topic_last_post_id` mediumint(8) unsigned NOT NULL default '0',
  `topic_moved_id` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`topic_id`),
  KEY `forum_id` (`forum_id`),
  KEY `topic_moved_id` (`topic_moved_id`),
  KEY `topic_status` (`topic_status`),
  KEY `topic_type` (`topic_type`)
) ENGINE=MyISAM AUTO_INCREMENT=2706 DEFAULT CHARSET=latin1;

--
-- Table structure for table `topics_watch`
--

DROP TABLE IF EXISTS `topics_watch`;
CREATE TABLE `topics_watch` (
  `topic_id` mediumint(8) unsigned NOT NULL default '0',
  `user_id` mediumint(8) NOT NULL default '0',
  `notify_status` tinyint(1) NOT NULL default '0',
  KEY `topic_id` (`topic_id`),
  KEY `user_id` (`user_id`),
  KEY `notify_status` (`notify_status`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `user_group`
--

DROP TABLE IF EXISTS `user_group`;
CREATE TABLE `user_group` (
  `group_id` mediumint(8) NOT NULL default '0',
  `user_id` mediumint(8) NOT NULL default '0',
  `user_pending` tinyint(1) default NULL,
  KEY `group_id` (`group_id`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `user_id` mediumint(8) NOT NULL default '0',
  `user_active` tinyint(1) default '1',
  `username` varchar(25) NOT NULL default '',
  `user_password` varchar(32) NOT NULL default '',
  `user_session_time` int(11) NOT NULL default '0',
  `user_session_page` smallint(5) NOT NULL default '0',
  `user_lastvisit` int(11) NOT NULL default '0',
  `user_level` tinyint(4) default '0',
  `user_posts` mediumint(8) unsigned NOT NULL default '0',
  `user_timezone` decimal(5,2) NOT NULL default '0.00',
  `user_style` tinyint(4) default NULL,
  `user_lang` varchar(255) default NULL,
  `user_dateformat` varchar(14) NOT NULL default 'd M Y H:i',
  `user_new_privmsg` smallint(5) unsigned NOT NULL default '0',
  `user_unread_privmsg` smallint(5) unsigned NOT NULL default '0',
  `user_last_privmsg` int(11) NOT NULL default '0',
  `user_emailtime` int(11) default NULL,
  `user_viewemail` tinyint(1) default NULL,
  `user_attachsig` tinyint(1) default NULL,
  `user_allowhtml` tinyint(1) default '1',
  `user_allowbbcode` tinyint(1) default '1',
  `user_allowsmile` tinyint(1) default '1',
  `user_allowavatar` tinyint(1) NOT NULL default '1',
  `user_allow_pm` tinyint(1) NOT NULL default '1',
  `user_allow_viewonline` tinyint(1) NOT NULL default '1',
  `user_notify` tinyint(1) default '0',
  `user_notify_pm` tinyint(1) NOT NULL default '0',
  `user_popup_pm` tinyint(1) NOT NULL default '0',
  `user_rank` int(11) default '0',
  `user_avatar` varchar(100) default NULL,
  `user_avatar_type` tinyint(4) NOT NULL default '0',
  `user_email` varchar(255) default NULL,
  `user_icq` varchar(15) default NULL,
  `user_website` varchar(100) default NULL,
  `user_from` varchar(100) default NULL,
  `user_sig` text,
  `user_sig_bbcode_uid` varchar(10) default NULL,
  `user_aim` varchar(255) default NULL,
  `user_yim` varchar(255) default NULL,
  `user_msnm` varchar(255) default NULL,
  `user_occ` varchar(100) default NULL,
  `user_interests` varchar(255) default NULL,
  `user_actkey` varchar(32) default NULL,
  `user_newpasswd` varchar(32) default NULL,
  `user_game_type` char(1) default NULL,
  `user_blurb` blob,
  `user_viewblurb` char(1) default NULL,
  `user_registerip` varchar(15) default NULL,
  `user_regdate` varchar(32) default NULL,
  `user_onirc` int(2) default '0',
  `user_emailforward` char(1) default '0',
  `user_showforward` char(1) default '0',
  `user_login_tries` smallint(5) unsigned NOT NULL default '0',
  `user_last_login_try` int(11) NOT NULL default '0',
  PRIMARY KEY  (`user_id`),
  KEY `user_session_time` (`user_session_time`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `view_player`
--

DROP TABLE IF EXISTS `view_player`;
CREATE TABLE `view_player` (
  `id` mediumint(8) NOT NULL default '0',
  `view_count` mediumint(8) default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `view_team`
--

DROP TABLE IF EXISTS `view_team`;
CREATE TABLE `view_team` (
  `id` mediumint(8) NOT NULL default '0',
  `view_count` mediumint(8) default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `vote_desc`
--

DROP TABLE IF EXISTS `vote_desc`;
CREATE TABLE `vote_desc` (
  `vote_id` mediumint(8) unsigned NOT NULL auto_increment,
  `topic_id` mediumint(8) unsigned NOT NULL default '0',
  `vote_text` text NOT NULL,
  `vote_start` int(11) NOT NULL default '0',
  `vote_length` int(11) NOT NULL default '0',
  PRIMARY KEY  (`vote_id`),
  KEY `topic_id` (`topic_id`)
) ENGINE=MyISAM AUTO_INCREMENT=288 DEFAULT CHARSET=latin1;

--
-- Table structure for table `vote_results`
--

DROP TABLE IF EXISTS `vote_results`;
CREATE TABLE `vote_results` (
  `vote_id` mediumint(8) unsigned NOT NULL default '0',
  `vote_option_id` tinyint(4) unsigned NOT NULL default '0',
  `vote_option_text` varchar(255) NOT NULL default '',
  `vote_result` int(11) NOT NULL default '0',
  KEY `vote_option_id` (`vote_option_id`),
  KEY `vote_id` (`vote_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `vote_voters`
--

DROP TABLE IF EXISTS `vote_voters`;
CREATE TABLE `vote_voters` (
  `vote_id` mediumint(8) unsigned NOT NULL default '0',
  `vote_user_id` mediumint(8) NOT NULL default '0',
  `vote_user_ip` char(8) NOT NULL default '',
  KEY `vote_id` (`vote_id`),
  KEY `vote_user_id` (`vote_user_id`),
  KEY `vote_user_ip` (`vote_user_ip`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `words`
--

DROP TABLE IF EXISTS `words`;
CREATE TABLE `words` (
  `word_id` mediumint(8) unsigned NOT NULL auto_increment,
  `word` char(100) NOT NULL default '',
  `replacement` char(100) NOT NULL default '',
  PRIMARY KEY  (`word_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2008-11-26 16:20:54
