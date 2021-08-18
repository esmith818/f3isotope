<?php

/**
 * @file
 * ConfigHandler class.
 */

namespace Drupal\db_maintenance\Module\Config;

/**
 * ConfigHandler class.
 */
class ConfigHandler {

  /**
   * Returns last cron run.
   */
  public static function getCronLastRun() {
    $val = variable_get('db_maintenance_cron_last', 0);
    return $val;
  }

  /**
   * Returns cron frequency.
   */
  public static function getCronFrequency() {
    $val = variable_get('db_maintenance_cron_frequency', 86400);
    return $val;
  }

  /**
   * Returns Log variable.
   */
  public static function getWriteLog() {
    $val = variable_get('db_maintenance_log', 0);
    return $val;
  }

  /**
   * Returns UseTimeInterval variable.
   */
  public static function getUseTimeInterval() {
    $val = variable_get('db_maintenance_use_time_interval', 0);
    return $val;
  }

  /**
   * Returns TimeIntervalStart variable.
   */
  public static function getTimeIntervalStart() {
    $val = variable_get('db_maintenance_time_interval_start', '01:30');
    return $val;
  }

  /**
   * Returns TimeIntervalEnd variable.
   */
  public static function getTimeIntervalEnd() {
    $val = variable_get('db_maintenance_time_interval_end', '02:30');
    return $val;
  }

  /**
   * Returns AllTables variable.
   */
  public static function getProcessAllTables() {
    $val = variable_get('db_maintenance_all_tables', 0);
    return $val;
  }

  /**
   * Returns AllTables variable.
   */
  public static function getTableList($database, $default = NULL) {
    $val = variable_get('db_maintenance_table_list_' . $database, $default);
    return $val;
  }

}
