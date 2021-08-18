<?php

/**
 * @file
 * PgSqlHandler class.
 */

namespace Drupal\db_maintenance\Module\Db\DbServer\PgSql;

use Drupal\db_maintenance\Module\Db\DbServer\DbServerHandlerInterface;

/**
 * PgSqlHandler class.
 */
class PgSqlHandler implements DbServerHandlerInterface {

  /**
   * Returns list of tables in the active database.
   */
  public function listTables() {
    $result = db_query("SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_name",
      array(), array('fetch' => \PDO::FETCH_ASSOC));

    return $result;
  }

  /**
   * Optimizes table in the active database.
   */
  public function optimizeTable($table_name) {
    try {
      db_query("VACUUM ANALYZE {$table_name}")->execute();
    }
    catch (\Exception $e) {
      watchdog_exception('type', $e);
    }
  }

}
