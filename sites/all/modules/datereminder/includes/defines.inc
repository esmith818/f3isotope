<?php
/**
 * @file
 * Define common constants.
 */

/**
 * Permissions.
 */
define('DATEREMINDER_ADMINISTER_REMINDERS', 'Administer reminders');
define('DATEREMINDER_VIEW_OTHER_USER_REMINDERS', 'See other user reminders');
define('DATEREMINDER_OTHER_EMAIL', 'Send reminder to arbitrary email address');
define('DATEREMINDER_REQUEST_REMINDER', 'Request reminders');

/**
 * Enable states for a node type
 */
define('DATEREMINDER_TYPE_DISABLED', 0);
define('DATEREMINDER_TYPE_RETAIN', 1);
define('DATEREMINDER_TYPE_ALLOWED', 2);
define('DATEREMINDER_TYPE_ON', 3);


/**
 * Defaults
 */
define('DATEREMINDER_MAX_REMINDERS', 3);
define('DATEREMINDER_CRON_FREQUENCY', 60);

/**
 * Name of file with database apis.
 */
define('DATEREMINDER_DB', 'includes/db7');

/**
 * Where should reminders be displayed? (values of datereminder_display_loc.)
 */
define('DATEREMINDER_IN_NODE', '0');
define('DATEREMINDER_AS_TAB', '1');
define('DATEREMINDER_AS_BLOCK', '2');
