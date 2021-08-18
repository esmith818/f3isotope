<?php

/**
 * @file
 * DbServerHandler interface.
 */

namespace Drupal\db_maintenance\Module\Db\DbServer;

/**
 * DbServerHandler interface.
 */
interface DbServerHandlerInterface {

  /**
   * Returns list of tables in the active database.
   */
  public function listTables();

  /**
   * Optimizes table in the active database.
   */
  public function optimizeTable($table_name);

}
