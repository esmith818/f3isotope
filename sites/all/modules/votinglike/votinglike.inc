<?php

function votinglike_action(){
    global $user;
    
    $type 	=$_REQUEST['type'];
    $eid 	=$_REQUEST['eid'];
    $tag  	=$_REQUEST['tag'];
    $value 	=$_REQUEST['val'];
    $vtype 	=$_REQUEST['value_type'];
    
    $voted =votinglike_get_vote($type, $eid, $user ->uid);
    votinglike_unvote($type, $eid, $user ->uid);
    
    if(!is_object($voted) || $voted ->tag !=$tag){
    	votinglike_vote($type, $eid, $user ->uid, $tag, $value, $vtype);
    }
    
    $vars =array();
	$vars['value_vote']  	=$value;
	$vars['value_type']  	=$vtype;
	$vars['button_type'] 	=$_REQUEST['button_type'];
	$vars['label_like'] 	=$_REQUEST['label_like'];
	$vars['label_dislike'] 	=$_REQUEST['label_dislike'];
        
    $dislike_show 	=$_REQUEST['dislike_show'];
    $tips_show		=$_REQUEST['tips_show'];
    $newButton 	=votinglike_output($type, $eid, $dislike_show, $vars, $show_tips);  
      
	$output = array('success' =>1, 'newButton' =>$newButton);
    
    if (isset($_REQUEST['ajax'])) {
    	drupal_add_http_header('Content-Type', 'text/javascript; charset=utf-8');
    	print drupal_json_encode($output);
    	drupal_exit();
    }else{
    	drupal_goto();
    }
}

function votinglike_settings_form(){
	$form = array();
	
	// Get all available content types.
	$types = node_type_get_types();
	$options = array();
	foreach ($types as $type) {
		$options[$type->type] = $type->name;
	}
	
	$weight =array();
	for($i =-20; $i <=20; $i++) $weight[$i] =$i;

	$button_type =array(
		"white" 	=>"white",
		"lightgray" =>"lightgray",
		"gray" 		=>"gray",
		"black" 	=>"black",
		"padded" 	=>"padded",
		"drop" 		=>"drop",
		"line" 		=>"line",
		"youtube" 	=>"youtube",
		"google" 	=>"google",
	);
    	
	$form['votinglike_nodetypes'] = array(
		'#type'          => 'checkboxes',
		'#title'         => t('Enable for the following content types'),
		//'#description'   => t('Select the content types for which you want to activate voting like button.'),
		'#default_value' => variable_get('votinglike_nodetypes', array()),
		'#options'       => $options,
	);
	
	$form['comment'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('Enable for comments to the following content types'),
		'#collapsible' => 1,
		'#collapsed'   => 1,
	);	
	$form['comment']['votinglike_comments_nodetypes'] = array(
		'#type'          => 'checkboxes',
		'#title'         => t('Enable for the following content types'),
		'#description'   => t('Select the content types for comments to which you want to activate voting like button.'),
		'#default_value' => variable_get('votinglike_comments_nodetypes', array()),
		'#options'       => $options,
	);

	$form['advanced'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('Advanced settings'),
		'#collapsible' => 1,
		'#collapsed'   => 1,
	);	
	$form['advanced']['votinglike_weight'] = array(
		'#type'          => 'select',
		'#title'         => t('Position'),
		'#description'   => t('Position of button.'),
		'#default_value' => variable_get('votinglike_weight', 0),
		'#options'       => $weight,
	);
	$form['advanced']['votinglike_button_type'] = array(
		'#type'          => 'select',
		'#title'         => t('Style'),
		'#default_value' => variable_get('votinglike_button_type', 'youtube'),
		'#options'       => $button_type,
	);
	$form['advanced']['votinglike_counter_type'] = array(
		'#type'          => 'select',
		'#title'         => t('Counter type'),
		'#default_value' => variable_get('votinglike_counter_type', 'number'),
		'#options'       => array('number' =>'number', 'percent' =>'percent'),
	);
	$form['advanced']['votinglike_user_enabled'] = array(
		'#type'          => 'checkbox',
		'#title'         => t('Enable for user page'),
		'#default_value' => variable_get('votinglike_user_enabled', TRUE),
	);
	$form['advanced']['votinglike_dislike_show'] = array(
		'#type'          => 'checkbox',
		'#title'         => t('Show Dislike Button'),
		'#default_value' => variable_get('votinglike_dislike_show', TRUE),
	);
	$form['advanced']['votinglike_tips_show'] = array(
		'#type'          => 'checkbox',
		'#title'         => t('Show Button Tooltip'),
		'#default_value' => variable_get('votinglike_tips_show', TRUE),
	);
	$form['advanced']['votinglike_label_like'] = array(
		'#type'          => 'textfield',
		'#title'         => t('Like Button label'),
		'#default_value' => variable_get('votinglike_label_like', 'Like'),
	);
	$form['advanced']['votinglike_label_dislike'] = array(
		'#type'          => 'textfield',
		'#title'         => t('Dislike Button label'),
		'#default_value' => variable_get('votinglike_label_dislike', 'Dislike'),
	);
	
	return system_settings_form($form);
}

