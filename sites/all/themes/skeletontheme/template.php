<?php
/**
 * Added to customize registration form
 * https://www.drupal.org/node/350634
 */
function skeletontheme_form_alter(&$form, &$form_state, $form_id)
{
    if ($form_id == 'user_register_form' || $form_id == 'user_profile_form') {
        $form['account']['name']['#title'] = t('F3 Nickname');
    }
}

/**
 * Override or insert variables into the page template for HTML output.
 */
function skeletontheme_process_html(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_html_alter($variables);
  }
}

/**
 * Override or insert variables into the page template.
 */
function skeletontheme_process_page(&$variables) {
  // Always print the site name and slogan, but if they are toggled off, we'll
  // just hide them visually.
  $variables['hide_site_name']   = theme_get_setting('toggle_name') ? FALSE : TRUE;
  $variables['hide_site_slogan'] = theme_get_setting('toggle_slogan') ? FALSE : TRUE;
  if ($variables['hide_site_name']) {
    // If toggle_name is FALSE, the site_name will be empty, so we rebuild it.
    $variables['site_name'] = filter_xss_admin(variable_get('site_name', 'Drupal'));
  }
  if ($variables['hide_site_slogan']) {
    // If toggle_site_slogan is FALSE, the site_slogan will be empty, so we rebuild it.
    $variables['site_slogan'] = filter_xss_admin(variable_get('site_slogan', ''));
  }
  // Since the title and the shortcut link are both block level elements,
  // positioning them next to each other is much simpler with a wrapper div.
  if (!empty($variables['title_suffix']['add_or_remove_shortcut']) && $variables['title']) {
    // Add a wrapper div using the title_prefix and title_suffix render elements.
    $variables['title_prefix']['shortcut_wrapper'] = array(
      '#markup' => '<div class="shortcut-wrapper clearfix">',
      '#weight' => 100,
    );
    $variables['title_suffix']['shortcut_wrapper'] = array(
      '#markup' => '</div>',
      '#weight' => -99,
    );
    // Make sure the shortcut link is the first item in title_suffix.
    $variables['title_suffix']['add_or_remove_shortcut']['#weight'] = -100;
  }
}

function skeletontheme_page_alter($page) {

		$mobileoptimized = array(
			'#type' => 'html_tag',
			'#tag' => 'meta',
			'#attributes' => array(
			'name' =>  'MobileOptimized',
			'content' =>  'width'
			)
		);

		$handheldfriendly = array(
			'#type' => 'html_tag',
			'#tag' => 'meta',
			'#attributes' => array(
			'name' =>  'HandheldFriendly',
			'content' =>  'true'
			)
		);

		$viewport = array(
			'#type' => 'html_tag',
			'#tag' => 'meta',
			'#attributes' => array(
			'name' =>  'viewport',
			'content' =>  'width=device-width, initial-scale=1'
			)
		);

		drupal_add_html_head($mobileoptimized, 'MobileOptimized');
		drupal_add_html_head($handheldfriendly, 'HandheldFriendly');
		drupal_add_html_head($viewport, 'viewport');

}

function skeletontheme_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Use CSS to hide titile .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    // comment below line to hide current page to breadcrumb
	$breadcrumb[] = drupal_get_title();
    $output .= '<div class="breadcrumb">' . implode('<span class="sep">»</span>', $breadcrumb) . '</div>';
    return $output;
  }
}

/**
 * Add Javascript for responsive mobile menu
 */
if (theme_get_setting('responsive_menu_state')) {

	drupal_add_js(drupal_get_path('theme', 'skeletontheme') .'/js/jquery.mobilemenu.js');

    $responsive_menu_switchwidth = (int) theme_get_setting('responsive_menu_switchwidth','skeletontheme');
    $responsive_menu_topoptiontext = theme_get_setting('responsive_menu_topoptiontext','skeletontheme');
    drupal_add_js(array('skeletontheme' => array('topoptiontext' => $responsive_menu_topoptiontext)), 'setting');

	drupal_add_js('jQuery(document).ready(function($) {

	$("#navigation .content > ul").mobileMenu({
		prependTo: "#navigation",
		combine: false,
        switchWidth: '.$responsive_menu_switchwidth.',
        topOptionText: Drupal.settings.skeletontheme[\'topoptiontext\']
	});

	});',
	array('type' => 'inline', 'scope' => 'header'));

}

function skeletontheme_feed_icon($variables) {
  $text = t('Subscribe to @feed-title', array('@feed-title' => $variables['title']));
  if ($image = theme('image', array('path' => drupal_get_path('theme', 'skeletontheme') . '/images/feed.png', 'width' => 65, 'height' => 65, 'alt' => $text))) {
    return l($image, $variables['url'], array('html' => TRUE, 'attributes' => array('class' => array('xml-icon'), 'title' => $text)));
  }
}

function skeletontheme_date_nav_title($params) {
	$granularity = $params['granularity'];
	$view = $params['view'];
	$date_info = $view->date_info;
	$link = !empty($params['link']) ? $params['link'] : FALSE;
	$format = !empty($params['format']) ? $params['format'] : NULL;
	switch ($granularity) {
		case 'year':
		$title = $date_info->year;
		$date_arg = $date_info->year;
		break;

		case 'month':
		$format = !empty($format) ? $format : (empty($date_info->mini) ? 'F Y' : 'F Y');
		$title = date_format_date($date_info->min_date, 'custom', $format);
		$date_arg = $date_info->year .'-'. date_pad($date_info->month);
		break;

		case 'day':
		// 'l, F j Y' = Sunday, August 2 2015
		// 'D, M j, Y' = Sun, Aug 2, 2015
		$format = !empty($format) ? $format : (empty($date_info->mini) ? 'D, M j, Y' : 'M j, Y');
		$title = date_format_date($date_info->min_date, 'custom', $format);
		$date_arg = $date_info->year .'-'. date_pad($date_info->month) .'-'. date_pad($date_info->day);
		break;

		case 'week':
		$format = !empty($format) ? $format : (empty($date_info->mini) ? 'F j Y' : 'F j');
		$title = t('Week of @date', array('@date' => date_format_date($date_info->min_date, 'custom', $format)));
		$date_arg = $date_info->year .'-W'. date_pad($date_info->week);
		break;
	}
	if (!empty($date_info->mini) || $link) {
		// Month navigation titles are used as links in the mini view.
		$attributes = array('title' => t('View full page month'));
		$url = date_pager_url($view, $granularity, $date_arg, TRUE);
		return l($title, $url, array('attributes' => $attributes));
	}
	else {
		return $title;
	}
}

/**
 * Output customized node preview on node edit and add forms.
 */
function skeletontheme_node_preview($variables) {
  $node = $variables['node'];
  $elements = node_view($node, 'full');
  $full = drupal_render($elements);
  $output = '<div class="preview">';
  $output .= '<h3 class="post-preview" >' . t('Preview of your posting') . '</h3>';
  $output .= $full;
  $output .= "</div>\n";
  return $output;
}
//EOF:Javascript
