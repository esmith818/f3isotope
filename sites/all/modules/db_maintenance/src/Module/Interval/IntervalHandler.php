<?php

/**
 * @file
 * IntervalHandler class.
 */

namespace Drupal\db_maintenance\Module\Interval;

use Drupal\db_maintenance\Module\Config\ConfigHandler;

/**
 * IntervalHandler class.
 */
class IntervalHandler {

  /**
   * Checks, if table optimization is allowed in consistency with
   * Time Interval configuration.
   */
  public static function isTimeIntervalConfirmed($datetime) {
    // If Time Interval checking is not turned on,
    // allow processing without further check.
    if (!ConfigHandler::getUseTimeInterval()) {
      return TRUE;
    }

    $time_start = ConfigHandler::getTimeIntervalStart();
    $time_end = ConfigHandler::getTimeIntervalEnd();

    return self::isInInterval($datetime, $time_start, $time_end);
  }

  /**
   * Checks, if $time is in interval between $time_start and $time_end.
   */
  public static function isInInterval($datetime, $time_start, $time_end) {

    // Get current DateTime.
    $dt = new \DateTime();
    $dt->setTimestamp(REQUEST_TIME);

    $dt_start = clone $dt;
    $dt_end = clone $dt;

    if(!self::setTimePart($time_start, $dt_start)) {
      return FALSE;
    }

    if(!self::setTimePart($time_end, $dt_end)) {
      return FALSE;
    }

    // Now $dt, $dt_start, $dt_end have the same day part.

    if ($dt_start <= $dt_end) {
      // No midnight between $time_start and $time_end like 01:00 - 03:00.
      if(($dt_start <= $datetime) && ($datetime <= $dt_end)) {
        // Like more than 01:00 AND less than 03:00 (or equal).
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
    else {
      // There is midnight between $time_start and $time_end like 23:00 - 01:00.
      if(($dt_start <= $datetime) || ($datetime <= $dt_end)) {
        // Like more than 23:00 OR less than 01:00 (or equal).
        return TRUE;
      }
      else {
        return FALSE;
      }
    }

  }

  /**
   * Checks, if $time is in 24 hour format H:i (HH:MM) like 23:30 or 01:00.
   */
  public static function checkTime($time) {

	  if(!isset($time)) {
      return FALSE;
    }

	  if(mb_strlen($time) != 5) {
      return FALSE;
    }

    $hour = (int) mb_substr($time, 0, 2);
    $minute = (int) mb_substr($time, 3, 2);

    if(($hour < 0) || ($hour > 23)) {
      return FALSE;
    }

    if(($minute < 0) || ($minute > 59)) {
      return FALSE;
    }

    return TRUE;

  }

  /**
   * Sets time part to $dt from $time.
   *
   * Also checks, if $time is in 24 hour format H:i (HH:MM) like 23:30 or 01:00.
   */
  public static function setTimePart($time, \DateTime &$dt) {

	  if(!isset($time)) {
      return FALSE;
    }

	  if(mb_strlen($time)!=5) {
      return FALSE;
    }

    $hour = (int) mb_substr($time, 0, 2);
    $minute = (int) mb_substr($time, 3, 2);

    if(($hour < 0) || ($hour > 23)) {
      return FALSE;
    }

    if(($minute < 0) || ($minute > 59)) {
      return FALSE;
    }

    $dt->setTime($hour, $minute);

    return TRUE;

  }

}
