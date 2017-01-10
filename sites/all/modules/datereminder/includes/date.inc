<?php
/**
 * @file
 * Some functions dealing with dates.
 */

/**
 * Return a map of supported date type fields.
 *
 * @return array
 *   Array of formats for date types, keyed by date type.
 */
function _datereminder_supported_date_field_types() {
  return array(
    'datetime' => 'Y-m-d H:i:s',
    'date' => 'Y-m-d\TH:i:s',
    'datestamp' => 'U',
  );
}

/**
 * Compute time of next reminder.
 *
 * @param reminder  $r
 *   One reminder.
 * @param string  $datefield
 *   Name of date field if known.
 *
 * @return bool
 *   TRUE if we changed the time of next reminder.
 */
function _datereminder_get_next_reminder(&$r, $datefield = NULL) {
  module_load_include('inc', 'datereminder', 'includes/defines');

  // Fudge factor to deal with reminders that happen between
  // cron runs. Skew is half of the cron period in seconds.
  $skew = variable_get('datereminder_cron_frequency', DATEREMINDER_CRON_FREQUENCY);
  $skew *= 30;
  $skew += 1;

  if (!isset($datefield)) {
    // Caller didn't supply datefield, so we need to find it.
    $datefield = _datereminder_get_datefield($r);
  }

  $lead = $r->leadtime;

  $rtime = _datereminder_get_next_occurance($datefield, $lead + $skew);

  if (isset($rtime)) {
    $rtime->modify("-$lead seconds");
    $rem = _datereminder_date_format_internal($rtime);
    $r->expired = 0;
  }
  elseif (!isset($r->expired) || $r->expired == 0) {
    // If not already expired and there's no new date, set expire date.
    $r->expired = 1;
    // TODO: Expire time should be after last date occurrance.
    $rtime = _datereminder_now(_datereminder_retain_expired());
    $rem = _datereminder_date_format_internal($rtime);
  }
  else {
    return FALSE;
  }

  $ret = !isset($r->next_due) || ($rem != $r->next_due);
  $r->next_due = $rem;
  return $ret;
}

/**
 * Get "now" in UTC as a string, with option offset from "now".
 *
 * @param int $offset
 *   Optional offset from "now" in seconds
 *
 * @return DateTime
 *   Time $offset seconds in tne future as DateTime.
 */
function _datereminder_now($offset = 0) {
  $utz = new DateTimeZone('UTC');
  $now = new DateTime(NULL, $utz);
  if ($offset != 0) {
    $now->modify("+$offset seconds");
  }
  return $now;
}

/**
 * Return string datetime for next occurance after a suitable delay from now.
 *
 * @param field $datefield
 *   The (possibly recurring) date field from the node
 * @param int $lead
 *   How long in the future to skip. Ignore any that happen before this
 *
 * @return DateTime
 *   DateTime of the next occurance. NULL means nothing past specified time.
 */
function _datereminder_get_next_occurance($datefield, $lead = 0) {
  $dt = _datereminder_now($lead);
  $ret = _datereminder_get_occurance_after_date($datefield, $dt);
  return $ret;
}

/**
 * Get date of next occurance after given date.
 *
 * @param field $df
 *   The date field from node.
 * @param DateTime $dt
 *   DateTime object for the date in question
 *
 * @return DateTime
 *   DateTime object for next occurance after $dt.
 */
function _datereminder_get_occurance_after_date($df, $dt) {

  if (isset($df[LANGUAGE_NONE])) {
    // Get map of type to format string.
    $dates = $df[LANGUAGE_NONE];
    $ftmap = _datereminder_supported_date_field_types();
    $fmtstring = $ftmap[$dates[0]['date_type']];
    if (!$fmtstring) {
      // Shouldn't happen. Paranoia.
      return NULL;
    }

    // Deal with other TZ
    $dbTzStr = $dates[0]['timezone_db'];
    if ($dbTzStr != 'UTC') {
      $dbTz = new DateTimeZone($dbTzStr);
      $dt->setTimeZone($dbTz);
    }

    // Format the date, as appropriate for this date field type.
    $datestr = $dt->format($fmtstring);

    foreach ($dates as $dt) {
      $etime = $dt['value'];
      if (!isset($etime)) {
        continue;
      }

      if (strcmp($etime, $datestr) > 0) {
        // This is the one we want. Turn back into DateTime.
        return _datereminder_date_field_value_to_datetime($df, $etime);
      }
    }
  }

  return NULL;
}

/**
 * Utility function - Get datefield out of node for this reminder.
 */
function _datereminder_get_datefield(&$r) {
  _datereminder_complete_reminder($r);
  return _datereminder_get_node_datefield($r->node);
}

/**
 * Utility function - Get the datefield for a reminder node.
 */
function _datereminder_get_node_datefield($node) {
  $type = $node->type;
  $dfname = variable_get("datereminder_datefield_$type", '');
  return $node->{$dfname};
}

/**
 * Return how long retired reminder should be retained.
 *
 * @return int
 *   Time to retain expired reminder
 */
function _datereminder_retain_expired() {
  $retain = variable_get('datereminder_retain_expired', 14);
  // Convert days to seconds.
  return $retain * 60 * 60 * 24;
}

/**
 * Return internal format string for a DateTime.
 *
 * @param DateTime $dt
 *   The date to convert to internal form.
 *
 * @return string
 *   String representation of $dt
 */
function _datereminder_date_format_internal($dt) {
  return $dt->format('U');
}

/**
 * Convert date from date field to DateTime object.
 *
 * @param datelist $datefield
 *   The nodes datefield content. (Needed to discern date format.)
 * @param string $dt
 *   Specific value to convert
 *
 * @return DateTime
 *   $val represented as DateTime
 */
function _datereminder_date_field_value_to_datetime($datefield, $dt) {
  $tzname = $datefield[LANGUAGE_NONE][0]['timezone_db'];
  $datetype = $datefield[LANGUAGE_NONE][0]['date_type'];
  if ($datetype == 'datestamp') {
    // For date stamp, we need to add the '@', but don't need timezone.
    return new DateTime("@$dt");
  }

  $tz = new DateTimeZone($tzname);
  $ret = date_create($dt, $tz);
  return $ret;
}

/**
 * Turn a date in internal string format (from database) into DateTime.
 *
 * @param string $dt
 *   Internal (UTC) date string to convert to DateTime object
 *
 * @return DateTime
 *   The resulting object
 */
function _datereminder_internal_date_to_datetime($dt) {

  $foo = "@$dt";
  // This takes Unix timestamp and returns DateTime in UTC.
  return new DateObject($foo);
}
