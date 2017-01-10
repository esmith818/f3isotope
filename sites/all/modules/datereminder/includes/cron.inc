<?php
/**
 * @file
 * Processing for hook_cron().
 */

/**
 * Implements hook_cron().
 *
 * Check to see if we need to send any reminders.
 *
 * If so, send them and update reminders for next occurrance.
 */
function _datereminder_cron() {
  module_load_include('inc', 'datereminder', 'includes/defines');
  module_load_include('inc', 'datereminder', 'includes/mailer');
  module_load_include('inc', 'datereminder', 'includes/date');
  module_load_include('inc', 'datereminder', 'includes/node');
  module_load_include('inc', 'datereminder', DATEREMINDER_DB);

  $skew = variable_get('datereminder_cron_frequency', DATEREMINDER_CRON_FREQUENCY);
  // Convert to seconds in half of the frequency.
  $skew *= 30;

  // Cron only runs every daterminder_cron_frequency seconds, so
  // we'll fire any reminders that are scheduled to be sent
  // up to half way to the next cron run.
  $now = _datereminder_now($skew);
  $dt = _datereminder_date_format_internal($now);

  // Get due reminders.
  $rems = _datereminder_get_due_reminders(intval($dt));

  // Send reminders that are not expired.
  foreach ($rems as $r) {
    if ($r->expired == 0) {
      _datereminder_send_reminder($r);
    }
  }

  // Re-calculate when the past-due reminders should next run.
  _datereminder_update_reminder_nexts($rems);
}
