<?php

/**
 * @file
 * MySqlHandler class.
 */

namespace Drupal\db_maintenance\Module\Db\DbServer\MySql;

use Drupal\db_maintenance\Module\Db\DbServer\DbServerHandlerInterface;

/**
 * MySqlHandler class.
 */
class MySqlHandler implements DbServerHandlerInterface {

  /**
   * Returns list of tables in the active database.
   */
  public function listTables() {
    $result = db_query("SHOW FULL TABLES WHERE Table_type = 'BASE TABLE'",
      array(), array('fetch' => \PDO::FETCH_ASSOC));

    return $result;
  }

  /**
   * Optimizes table in the active database.
   */
  public function optimizeTable($table_name) {
    try {
      db_query("OPTIMIZE TABLE {$table_name}")->execute();
    }
    catch (\Exception $e) {
      watchdog_exception('type', $e);
    }
  }

}
