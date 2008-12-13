You must set the Database details in

www/forums/config.php
php/db.php


www/ is the web root.


The www/wbn directory must be aliased to a subdomain. e.g. wbn.winbolo.net - This address can then be configured in the winbolo ini file.

An Anonymous user must be created in the users table with user_id -1

The news forum id must match forum_id 5 to appear on the front page or recent news side bar. This can be changed by editing the inc/functions.php but it is hard coded in the SQL queries.

Forums use phpBB software available from www.phpbb.com. (Included here as well)
