<?php

/**
 *  @file smiley/includes/SmileyStreamWrapper.inc
 *
 */

/**
 *  Create an instance like this:
 *  $smiley = new SmileyStreamWrapper('smiley://my-pack-pack/lol.gif');
 */
class SmileyStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Implements abstract public function getDirectoryPath()
   */
  public function getDirectoryPath() {
    return variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs');
  }

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URI of a public file.
   */
  function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    return $GLOBALS['base_url'] . '/' . self::getDirectoryPath() . '/' . drupal_encode_path($path);
  }
}
