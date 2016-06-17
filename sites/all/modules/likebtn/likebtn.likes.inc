<?php

/**
 * @file
 * Function to generate likes page.
 */

/**
 * Generate the likes page.
 */
function likebtn_likes_page($entity, $entity_type) {

  $output = '';

  $rows = likebtn_get_count($entity, $entity_type);

  $total_likes_minus_dislikes = 0;
  foreach ($rows as $row) {
    $total_likes_minus_dislikes += $row['likes_minus_dislikes'];
  }

  $header = array(
    t('Button'),
    t('Likes'),
    t('Dislikes'),
    t('Likes minus dislikes'),
  );
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  $output .= '<p>' . t('Total likes minus dislikes (vote results):') . ' <strong> ' . $total_likes_minus_dislikes . '</strong></p>';
  $output .= '<p>' . t("If you don't see information on likes:") . '</p>';
  $output .= '<ul>';
  $output .= '<li>' . t('Make sure you have entered information correctly in') . ' <a href="/admin/config/services/likebtn">' . t('Auto-synching likes into local database') . '(PRO, VIP, ULTRA)</a></li>';
  $output .= '<li>' . t('Make sure that PHP curl extension is enabled.') . '</a></li>';
  $output .= '<li>' . t('Maybe nobody voted for this content type yet.') . '</a></li>';
  $output .= '<li>' . t('Perhaps synchronization has not been launched yet.') . '</a></li>';
  $output .= '</ul>';

  return $output;
}
