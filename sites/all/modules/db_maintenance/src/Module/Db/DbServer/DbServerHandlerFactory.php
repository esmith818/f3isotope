<?php

/**
 * @file
 * DbServerHandlerFactory class.
 */

namespace Drupal\db_maintenance\Module\Db\DbServer;
use Drupal\db_maintenance\Module\Db\DbServer\MySql\MySqlHandler;
use Drupal\db_maintenance\Module\Db\DbServer\PgSql\PgSqlHandler;


/**
 * DbServerHandlerFactory class.
 */
class DbServerHandlerFactory {

  /**
   * Returns proper DbServerHandler.
   */
  public static function getDbServerHandler() {

    if (db_driver() == 'mysql') {
      $handler = new MySqlHandler();
    }
    elseif (db_driver() == 'pgsql') {
      $handler = new PgSqlHandler();
    }
    else {
      throw new \Exception(t('Unsupported DB server type.'));
    }

    return self::cast($handler);

  }

  /**
   * Returns typed $handler as DbServerHandlerInterface.
   */
  public static function cast(DbServerHandlerInterface &$object = NULL) {
    return $object;
  }

}
