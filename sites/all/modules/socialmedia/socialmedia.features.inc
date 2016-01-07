<?php


/**
 * Implementation of hook_features_export_options()
 */
function socialmedia_profile_features_export_options() {
  $options = array();
  // load site profiles
  $profiles = (array) socialmedia_profile_load_all(null, 0);
  foreach ($profiles as $name => $profile) {
    $username = ($profile['uid']) ? t('user') . ' ' . $profile['uid'] : t('site');
    $options[$profile['platform'] . ':' . $profile['uid']] = t('!username profile for !platform',
      array(
        '!username' => $username,
        '!platform' => $profile['platform'],
      )
    );
  }
  return $options;
}

/**
 * Implementation of hook_features_export()
 */
function socialmedia_profile_features_export($data, &$export, $module_name = '') {
  $pipe = array();
  
  //$export['dependencies'][] = 'config';

  foreach ((array) $data as $name) {
    list($platform, $uid) = explode(':', $name);
    $profile = socialmedia_profile_load($platform, $uid);
    if (!empty($profile)) {
      $export['features']['socialmedia_profile'][$name] = $name;
      //if (isset($profile['module']) && !in_array($profile['module'], $export['dependencies'])) {
      //  $export['dependencies'][] = $profile['module'];
      //}
    }
  }

  return $pipe;
}

/**
 * Implementation of hook_features_export_render()
 */
function socialmedia_profile_features_export_render($module_name, $data) {
  $profiles = array();

  foreach ($data as $name) {
    list($platform, $uid) = explode(':', $name);
    $profile = socialmedia_profile_load($platform, $uid);
    if (!empty($profile)) {
      $profile['uid'] = $uid;
      $profile['platform'] = $platform;
      if (isset($profile['result'])) {
        unset($profile['result']);
      }
      $profiles[$name] = $profile;
    }
  }
  $code = '  $data = ' . features_var_export($profiles, '  ') . ';' . PHP_EOL;
  $code .= '  return $data;';

  return array('socialmedia_profile_defaults' => $code);
}

/**
 * Implementation of hook_features_rebuild()
 */
function socialmedia_profile_features_rebuild($module) {
  socialmedia_profile_features_revert($module);
}

/**
 * Implementation of hook_features_revert()
 */
function socialmedia_profile_features_revert($module) {
  if ($data = features_get_default('socialmedia_profile', $module)) {
    foreach ($data as $name => $profile) {
      socialmedia_profile_save($profile);
    }
  }
}