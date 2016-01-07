<?php

/**
 * @file
 * Token processing for social media
 */

function socialmedia_token_info() {
  $type = array(
    'name' => t('Social media'), 
    'description' => t('Tokens related to social media sites.'), 
    //'needs-data' => 'socialmedia',
  );
  $tokens = array(
    'socialmedia' => array(),
    'site' => array(),
    'user' => array(), 
    'current-page' => array(), 
  );

  $profiles = socialmedia_platform_definitions();
  foreach ($profiles AS $platform => $data) {
    if (is_array($data['tokens'])) {
      foreach ($data['tokens'] AS $ttype => $tdata) {
        if ($ttype == 'multi') {
          foreach ($tdata AS $key => $ttdata) {
            $tokens['socialmedia']['sm-' . $key] = $ttdata;
            $tokens['site']['sm-' . $key] = $ttdata;
            $tokens['user']['sm-' . $key] = $ttdata;
          }
        }
        else {
          foreach ($tdata AS $key => $ttdata) {
            $tokens[$ttype]['sm-' . $key] = $ttdata;
          }
        }
      } 
    }
  }
  /*
  // Core tokens for nodes.
  $socialmedia['twitter_profile_url'] = array(
    'name' => t("Twitter profile url"), 
    'description' => t("URL to twitter profile."),
  );
  */
  
  $platforms = socialmedia_icon_platforms();
  foreach ($platforms AS $platform => $name) {
    $tokens['socialmedia']['sm-' . $platform . '_icon-path:?'] = array(
      'name' => t('@platform icon path', array('@platform' => $name)),
      'description' => t("The path to the @platform icon", array('@platform' => $name)),
    );
  }
  
  $defaults = array(
    'width' => t('Default width'),
    'height' => t('Default height'),
    'color_body_background' => t('Default body background color'),
    'color_body_background' => t('Default body background color'),
    'color_body_text' => t('Default body text color'),
    'color_body_linktext' => t('Default body linktext color'),
    'color_header_background' => t('Default header background color'),
    'color_header_text' => t('Default header text color'),
    'color_border' => t('Default border color'),
    'link_target_profile' => t('Default target attribute for profile links'),
    'link_target_sharing' => t('Default target attribute for sharing links'),
    'link_rel_profile' => t('Default rel attribute for profile links'),
    'link_rel_sharing' => t('Default rel attribute for sharing links'),
    'link_class_profile' => t('Default class attribute for profile links'),
    'link_class_sharing' => t('Default class attribute for sharing links'),
  );
  foreach ($defaults AS $key => $value) {
    $tokens['socialmedia']['sm-default-' . $key] = array(
      'name' => $value,
      'description' => $value . ' ' . t('values set in social media config.'),
    );    
  }
  
  $tokens['current-page']['title-plain'] = array(
    'name' => t('The plain title of the current page.'),
    'description' => t('The title of the current page stripped of HTML'),
  );

  
  $info =  array(
    'types' => array('socialmedia' => $type), 
    'tokens' => array(
      'socialmedia' => $tokens['socialmedia'],
      //'usersite' => $tokens['usersite'],
      'site' => $tokens['site'],
      'user' => $tokens['user'],
      'current-page' => $tokens['current-page'],
    ),
  );
  return $info;
}

function socialmedia_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  if ($type == 'current-page') {
    if (isset($tokens['title-plain'])) {
      $replacements[$tokens['title-plain']] = strip_tags(drupal_get_title());
    }
  }
  if (($type == 'site') || ($type == 'user') || ($type == 'socialmedia')) {

    //$uid = (($type == 'user') && isset($data['user']->uid)) ? $data['user']->uid : 0; 
    foreach ($tokens as $name => $original) {
      if (strpos($name, 'sm-') === 0) {
        list($platform_name, $hash) = explode('_', substr($name, 3), 2);
        // process icon paths
        if (strpos($platform_name, 'default') === 0) {
          $replacements[$original] = socialmedia_default_tokens($hash);
          continue;         
        }
        if ((strpos($hash, 'icon-path') === 0) || (strpos($hash, 'icon-markup') === 0)) {
          $mode = (strpos($hash, 'icon-path') === 0) ? 'path' : 'markup';
          $replacements[$original] = socialmedia_icon_tokens($platform_name, $hash, $mode);
          continue;
        }
        $profile = NULL;
        // if token has user context, load the profiles for the user        
        $profile_context = (isset($data['set']['data']['socialmedia']['profile_context'])) ? $data['set']['data']['socialmedia']['profile_context'] : 'usersite';
        if ((strpos($profile_context, 'user') !== FALSE)
          && (($type == 'user') || ($type == 'socialmedia'))) {
          $uid = socialmedia_get_context_author_uid($data);
          if (isset($uid) && $uid) {
            $profile = socialmedia_profile_load($platform_name, $uid);
          }
        }
        // if site context or user profile not found, load site profile
        $load_site_profile = 0;
        if ((strpos($profile_context, 'site') !== FALSE)
          && (($type == 'site') || ($type == 'socialmedia'))) {
          if ($profile_context == 'site') {
            $load_site_profile = 1;
          }
          // if profile_context = usersite and profile from user not set
          else if (($profile_context != 'user') && (!isset($profile['result']) || !$profile['result'])) {
            $load_site_profile = 1;
          }
        }
        if ($load_site_profile) {
          $profile = socialmedia_profile_load($platform_name, 0);
        }
        $platform = socialmedia_platform_definition_load($platform_name);

        $replacement = call_user_func($platform['tokens callback'], $hash, $profile);

        if (!isset($replacement) || $replacement === FALSE) {
          $msg = t('You are trying to use a social media %platform profile token but the profile has not been set. !link',
            array(
              '%platform' => $platform['title'],
              '!link' => l(t('Set @platform profile.', array('@platform' => $platform['title'])), 'admin/config/media/socialmedia')
            )
          );
          //_socialmedia_widgets_set_error($data['widgets']['element'], $msg);
          _socialmedia_widgets_set_error($data['set']['name'] . ':' . $data['widgets']['element']['name'], '');
        }
        else {
          $replacements[$original] = $replacement;
        }
      }
    }
  }

  return $replacements;
}

function socialmedia_default_tokens($hash) {
  return variable_get('socialmedia_default_' . $hash, constant('SOCIALMEDIA_' . strtoupper($hash) . '_DEFAULT'));
}

function socialmedia_icon_tokens($platform_name, $hash, $mode) {
  $b = explode(':', $hash);
  $iconsets = socialmedia_iconset_info();
  if (isset($b[1])) {
    $s = $b[1];
  }
  else {
    $s = variable_get('socialmedia_icon_default_style', SOCIALMEDIA_ICON_DEFAULT_STYLE_DEFAULT);
  }
  if (!$s) {
    $msg = t('You are trying to use a social media icon token but no default icon style has been set. !link',
      array(
        '!link' => l(t('Set default icon style.'), 'admin/config/media/socialmedia')
      )
    );
    _socialmedia_widgets_set_error('set', $msg);
    return '';
  }
  list($iconset, $style) = explode(':', $s);

  if (($mode == 'path') && isset($iconsets[$iconset]['path callback'])) {
    return base_path() . call_user_func($iconsets[$iconset]['path callback'], $platform_name, $style);
  }
  else if (($mode == 'markup') && isset($iconsets[$iconset]['markup callback'])) {
    return call_user_func($iconsets[$iconset]['markup callback'], $platform_name, $style);
  }
  return '<none>';
}




