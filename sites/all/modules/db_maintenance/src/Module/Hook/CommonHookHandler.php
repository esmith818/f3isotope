<?php

/**
 * @file
 * CommonHookHandler class.
 */

namespace Drupal\db_maintenance\Module\Hook;

use Drupal\db_maintenance\Module\Config\ConfigHandler;
use Drupal\db_maintenance\Module\Db\DbHandler;
use Drupal\db_maintenance\Module\Interval\IntervalHandler;

/**
 * CommonHookHandler class.
 */
class CommonHookHandler {

  /**
   * Implements hook_help().
   */
  public static function hookHelp($path, $arg) {
    switch ($path) {
      case 'admin/help#db_maintenance':
        $output = '';
        $output .= '<h3>' . t('About') . '</h3>';
        $output .= '<p>' . t('DB Maintenance performs an optimization query on selected tables') . '</p>';
        $output .= '<h3>' . t('Uses') . '</h3>';
        $output .= '<dl>';
        $output .= '<dt>' . t('MyIASM Tables') . '</dt>';
        $output .= '<dd>' . t('OPTIMIZE TABLE repairs a table if it has deleted or split rows, sorts table indexes, and updates table statistics. For BDB and InnoDB, OPTIMIZE rebuilds the table.') . '</dd>';
        $output .= '<dd>' . t('OPTIMIZE works best on tables with large deletions (e.g. cache or watchdog), however, MySQL will reuse old record positions, therefore in most setups, OPTIMIZE TABLE is unnecessary unless you just like defragmenting.') . '</dd>';
        $output .= '<dd>' . t("The Overhead column in phpMyAdmin's database view is the most common way to determine the need of an OPTIMIZE TABLE query. It essentially shows the amount of disk space you would recover by running an optimize/defragmentation query.") . '</dd>';
        $output .= '<dd><i><u>' . t('Note: MySQL locks tables while OPTIMIZE TABLE is running.') . '</u></i></dd>';
        $output .= '<dt>' . t('Postgre SQL Tables') . '</dt>';
        $output .= '<dd>' . t("VACUUM reclaims storage occupied by deleted tuples. In normal PostgreSQL operation, tuples that are deleted or obsoleted by an update are not physically removed from their table; they remain present until a VACUUM is done. It's therefore necessary to VACUUM periodically, especially on frequently-updated tables.") . '</dd>';
        $output .= '</dl>';
        return $output;

      case 'admin/config/system/db_maintenance':
        return t('<p>DB maintenance performs an optimization query on selected tables.</p>
        <p>For MyISAM tables,
        OPTIMIZE TABLE repairs a table if it has deleted or split rows, sorts table indexes,
        and updates table statistics. For BDB and InnoDB, OPTIMIZE rebuilds the table. OPTIMIZE
        works best on tables with large deletions (e.g. cache or watchdog), however MySQL will reuse
        old record positions, therefore in most setups, OPTIMIZE TABLE is unnecessary unless you
        just like defragmenting.</p><p>Note: MySQL locks tables during the time OPTIMIZE TABLE is running.</p>
        <p>The Overhead column in phpMyAdmin\'s database view is the most common way to determine the
        need of an OPTIMIZE TABLE query. It essentially shows the amount of disk space you would
        recover by running an optimize/defragmentation query.</p>
        <p>For PostgreSQL tables, VACUUM reclaims storage occupied by deleted tuples.
        In normal PostgreSQL operation, tuples that are deleted or obsoleted by an update are not
        physically removed from their table; they remain present until a VACUUM is done. Therefore
        it\'s necessary to VACUUM periodically, especially on frequently-updated tables.</p>');
    }
  }

  /**
   * Implements hook_permission().
   */
  public static function hookPermission() {
    return array(
      'administer db maintenance' => array(
        'title' => t('Administer DB Maintenance'),
        'description' => t('Select which tables to optimize during cron jobs.'),
      ),
    );
  }

  /**
   * Implements hook_menu().
   */
  public static function hookMenu() {
    $items = array();

    $items['admin/config/system/db_maintenance'] = array(
      'title' => 'DB maintenance',
      'description' => 'Executes a cron-based query to optimize database tables.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('db_maintenance_admin_settings'),
      'access callback' => 'user_access',
      'access arguments' => array('administer db maintenance'),
      'type' => MENU_NORMAL_ITEM,
      'file' => 'db_maintenance.admin.inc',
    );

    $items['db_maintenance/optimize'] = array(
      'page callback' => 'db_maintenance_optimize_tables_page',
      'access callback' => 'user_access',
      'access arguments' => array('administer db maintenance'),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Implements hook_cron().
   */
  public static function hookCron() {
    // Get current DateTime.
    $timestamp = REQUEST_TIME;
    $dt = new \DateTime();
    $dt->setTimestamp($timestamp);

    if (!IntervalHandler::isTimeIntervalConfirmed($dt)) {
      // Do not proceed if REQUEST_TIME is not in the time interval.
      return;
    }

    $last_run = ConfigHandler::getCronLastRun();
    if (ConfigHandler::getUseTimeInterval()) {
      // Adjust $last_run for 5 minutes to overcome real start time
      // fluctuations. It is important when using time interval
      // for not to miss the interval time frame.
      $last_run -= 300;
    }

    $interval = $timestamp - ConfigHandler::getCronFrequency();
    // Only run cron if enough time has elapsed.
    if ($interval > $last_run) {
      // db_maintenance_optimize_tables();
      DbHandler::optimizeTables();
    }
  }

}
