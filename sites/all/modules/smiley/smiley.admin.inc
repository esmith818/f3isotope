<?php
/**
 * @file
 * Smileys administrative pages.
 */

/**
 * Smileys overview form.
 */
function smiley_overview_form($form, &$form_state) {
  $header = array(
    array('data' => t('Picture')),
    array('data' => t('File'), 'field' => 'uri'),
    array('data' => t('Enabled'), 'field' => 'status', 'sort' => 'desc'),
    array('data' => t('Acronyms'), 'field' => 'acronyms'),
    array('data' => t('Description'), 'field' => 'description'),
    array('data' => t('Delete')),
  );
  $query = db_select('smiley', 'em')->extend('PagerDefault')->extend('TableSort');
  $query->fields('em');
  $result = $query->orderByHeader($header)->limit(15)->execute();

  $smiley_path = drupal_get_path('module', 'smiley');
  foreach ($result as $smiley) {
    $form += smiley_overview_item($smiley);
  }

  // Fields for a new smiley.
  $smiley = new stdClass();
  $smiley->sid = 'new';
  $smiley->uri = '';
  $smiley->status = 0;
  $smiley->acronyms = '';
  $smiley->description = '';
  $form += smiley_overview_item($smiley);

  $form['new']['delete'] = NULL;
  $form['new']['picture']['#markup'] = 'New';
  $form['new']['uri']['#required'] = FALSE;

  $form['pager'] = array('#theme' => 'pager');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['#header'] = $header;

  return $form;
}

/**
 * Helper function for smiley_overview_form().
 */
function smiley_overview_item($smiley) {
  static $smiley_path;
  if ($smiley_path == NULL) {
    $smiley_path = variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs');
  }
  $img_vars = array(
    'path' => $smiley_path . '/' . $smiley->uri,
    'alt' => 'smiley',
    'attributes' => array(),
  );
  $form[$smiley->sid]['picture']['#markup'] = theme_image($img_vars);
  $form[$smiley->sid]['uri'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->uri,
    '#required' => TRUE,
    '#size' => 25,
    '#title' => t('File'),
    '#title_display' => 'invisible',
  );
  $form[$smiley->sid]['status'] = array(
    '#type' => 'checkbox',
    '#default_value' => $smiley->status,
  );
  $form[$smiley->sid]['acronyms'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->acronyms,
    '#size' => 50,
  );
  $form[$smiley->sid]['description'] = array(
    '#type' => 'textfield',
    '#default_value' => $smiley->description,
    '#size' => 10,
  );
  $form[$smiley->sid]['delete'] = array(
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form[$smiley->sid]['#tree'] = TRUE;
  $form[$smiley->sid]['#smiley'] = TRUE;

  return $form;
}

/**
 * Smileys overview form validate.
 */
function smiley_overview_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  foreach ($values as $sid => $value) {
    if (is_array($value) && !empty($value['uri']) && empty($value['delete'])) {
      $exist = db_query('SELECT COUNT(*) FROM {smiley} WHERE uri = :url AND sid <> :sid', array(
        ':url' => $value['uri'],
        ':sid' => (is_numeric($sid) ? $sid : -1),
      ))->fetchColumn();
      if ($exist) {
        form_set_error($sid . '][uri', t('This uri already exists'));
      }
      elseif (!preg_match('#^[a-z0-9_\-/]*\.(gif|png|jpg)$#i', $value['uri'])) {
        form_set_error($sid . '][uri', t('Incorrect uri.'));
      }
    }
  }
}

/**
 * Smileys form submit.
 */
function smiley_overview_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  $delete = array();
  $update_cnt = 0;
  foreach ($values as $sid => $value) {
    if (is_array($value) && !empty($value['uri'])) {
      $smiley = (object) $value;
      $smiley->sid = $sid;
      $smiley->description = strip_tags($smiley->description);

      // Adding.
      if ($sid == 'new') {
        unset($smiley->sid);
        drupal_write_record('smiley', $smiley);
        drupal_set_message(t('New smiley has been added.'));
      }
      // Deleting.
      elseif ($value['delete']) {
        $delete[] = $sid;
      }
      // Updateting.
      else {
        $update_cnt++;
        drupal_write_record('smiley', $smiley, 'sid');
      }
    }
  }
  if ($update_cnt) {
    drupal_set_message(format_plural($update_cnt, 'Updated 1 smiley.', '@count smiley has been updated'));
  }
  // Deleting.
  if ($delete) {
    $num_deleted = db_delete('smiley')->condition('sid', $delete, 'IN')
      ->execute();
    drupal_set_message(format_plural($num_deleted, 'Deleted 1 smiley.', '@count smiley has been deleted'));
  }
}

/**
 * Returns HTML for the smiley overview form into a table.
 *
 * @ingroup themeable
 */
function theme_smiley_overview_form($vars) {
  $form = $vars['form'];
  foreach (element_children($form) as $smid) {
    $element = & $form[$smid];
    if (empty($element['#smiley'])) {
      continue;
    }
    $row = array();
    foreach ($element as $k => $v) {
      if ($k[0] != '#') {
        $row[] = array(
          'data' => drupal_render($element[$k]),
          'style' => 'text-align: center',
        );
      }
    }
    $row = array_merge(array('data' => $row), $element['#attributes']);
    $rows[] = $row;
  }
  $output = theme('table', array(
    'header' => $form['#header'],
    'rows' => $rows,
    'attributes' => array('id' => 'smiley-overview'),
  ));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Settings form.
 */
function smiley_settings_form($form, &$form_state) {
  $form['note'] = array(
    '#markup' => t("It is the best practice to hold your smiley in separate folder under website's public files or resources directory."),
  );
  $form['smiley_path'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => 'Path to smiley packs directory',
    '#maxlength' => 256,
    '#default_value' => variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs'),
    '#size' => 64,
  );
  $form['smiley_css_class'] = array(
    '#type' => 'textfield',
    '#title' => 'CSS class to add to smiley images',
    '#description' => 'Separate mulitple classes with a space',
    '#maxlength' => 256,
    '#default_value' => variable_get('smiley_css_class', ''),
    '#size' => 64,
  );

  return system_settings_form($form);
}

/**
 * Settings form validate.
 */
function smiley_settings_form_validate($form, &$form_state) {
  if (!is_dir($form_state['values']['smiley_path'])) {
    form_set_error('smiley_path', t('Directory does not exist'));
  }
}

/**
 * Smileys import form.
 */
function smiley_import_form($form, &$form_state) {
  $form['help'] = array(
    '#prefix' => '<p>',
    '#markup' => t('Before import new smiley, download smiley packs (!kolobok, !phpBB or other) and extract them in smiley folder (<strong>@folder</strong>).', array(
      '!kolobok' => l(t('kolobok'), 'http://www.en.kolobok.us/download.php?list.8', array('attributes' => array('title ' => 'Author\'s smiley Kolobok Style'))),
      '!phpBB' => l(t('phpBB'), 'http://www.phpbb.com/customise/db/styles/smilies-13/', array('attributes' => array('title ' => 'Smyles for phpBB'))),
      '@folder' => variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs'),
    )),
    '#suffix' => '</p>',
  );
  $form['status'] = array(
    '#title' => t('Enable all new smiley'),
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Start import'),
  );

  return $form;
}

/**
 * Smileys import form submit.
 */
function smiley_import_form_submit($form, &$form_state) {
  $num_added = smiley_import($form_state['values']['status']);
  if ($num_added) {
    drupal_set_message(format_plural($num_added, '1 smiley has been added', '@count smiley has been added'));
  }
  else {
    drupal_set_message(t('There is no new smiles in directory <em>@folder</em>', array(
      '@folder' => variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs'),
    )), 'warning');
  }
}

/**
 * Import smiley.
 *
 * @param int $status
 *   (optional) Status of new smiley.
 *
 * @return int
 *   Number of added smiley
 */
function smiley_import($status = 0) {
  $packs_path = variable_get('smiley_path', drupal_get_path('module', 'smiley') . '/packs');
  $files = file_scan_directory($packs_path, '#.\.(gif|png|jpg)$#i');
  $query = db_insert('smiley')->fields(array(
    'uri',
    'status',
    'acronyms',
    'description',
  ));
  $total = 0;
  foreach ($files as $file) {
    $values = array(
      'uri' => substr($file->uri, drupal_strlen($packs_path) + 1),
      'status' => $status,
      'acronyms' => '*' . $file->name . '*',
      // "yes3" will be "Yes 3"
      'description' => drupal_ucfirst(preg_replace(array('/_/', '/(\d+)$/'), array(' ', ' $1'), $file->name)),
    );
    $exist = db_query('SELECT COUNT(*) FROM {smiley} WHERE uri = :uri', array(':uri' => $values['uri']))->fetchColumn();
    if (!$exist) {
      $query->values($values);
      $total++;
    }
  }
  $query->execute();

  return $total;
}
