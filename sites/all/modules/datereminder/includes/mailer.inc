<?php
/**
 * @file
 * Support for building and sending reminder emails. (Drupal 7 version.)
 */

/**
 * Returns template for email body, either default or set by admin.
 *
 * @return string
 *   The message body.
 */
function _datereminder_email_body() {
  $dflt = "[node:title] is coming at [datereminder:next-short]\n\n[node:summary]\n\nRead more: [node:url]";
  return variable_get('datereminder_mail_body', $dflt);
}

/**
 * Returns template for email subject, either default or as set by admin.
 *
 * @return string
 *   The message subject.
 */
function _datereminder_email_subject() {
  return variable_get('datereminder_mail_subject',
      'REMINDER from [site:name]: [node:title] at [datereminder:next-short]');
}

/**
 * Return datereminder-specific "from" as set by admin.
 *
 * @return string
 *   The "From string." Empty means use system default.
 */
function _datereminder_email_from() {
  return variable_get('datereminder_mail_from', '');
}


/**
 * Implements hook_mail().
 */
function datereminder_mail($key, &$message, $params) {
  $r = $params['reminder'];
  _datereminder_complete_reminder($r);

  $objects = array(
    'node' => $r->node,
    'user' => $r->user,
    'datereminder' => $r,
    'global' => NULL,
  );

  $options = array();

  $v = token_replace(_datereminder_email_subject(), $objects, $options);
  $message['subject'] = $v;

  $v = token_replace(_datereminder_email_body(), $objects, $options);
  $v = drupal_html_to_text($v);

  $message['body'][] = $v;
}


/**
 * Send reminder email given this reminder object.
 *
 * @param object $r
 *   The reminder.
 */
function _datereminder_send_reminder($r) {
  _datereminder_complete_reminder($r);

  $from = _datereminder_email_from();

  $to = empty($r->email) ? $r->user->mail : $r->email;

  $params = array('reminder' => $r);

  $result = drupal_mail('datereminder', 'reminder', $to, NULL, $params, $from);

  return $result['result'];
}
