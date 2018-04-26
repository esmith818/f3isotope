<?php
/**
 * @file
 * Smiley plugin for the WYSIWYG module.
 */

/**
 * Implements hook_wysiwyg_plugin().
 *
 * Should be named {$module}_{$plugin}_plugin().
 */
function smiley_wysiwyg_smiley_extra_plugin() {
  $plugins['smiley_extra'] = array(
    'title' => t('Smiley Extra'),
    'vendor url' => 'http://drupal.org/project/smiley/',
    'icon file' => 'smiley.png',
    'icon title' => t('Insert smileys'),
    'settings' => array(),
  );

  return $plugins;
}
