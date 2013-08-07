<?php
/**
* @file
* Global-level configuration form and supporting logic for the Conductrics module
*/

module_load_include('inc', 'conductrics_mvp', 'conductrics_mvp_data_access'); // helper functions for getting/setting data about agents, goals, etc

function conductrics_mvp_admin($form, &$form_state) {
  $account_info = variable_get('conductrics_account_info');
  $account_info_provided = !empty($account_info['owner_code']) && !empty($account_info['api_key']);

  $path = drupal_get_path('module', 'conductrics_mvp') . '/assets';
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $path . '/conductrics-admin.css',
    ),
  );

  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => 'Conductrics Account Settings',
    '#collapsible' => TRUE,
    '#collapsed' => $account_info_provided
  );

  $form['account']['msg'] = array(
    '#markup' => t("<p>This information is used to 'link' your Conductrics account to Drupal.</p><p>You can find these codes by logging into your Conductrics account and going to the Account > Password and Keys page.</p><p>If you don't have a Conductrics account, you can get one now by signing up at <a href='http://conductrics.com' target='_blank'>http://conductrics.com</a> (free signup).</p>"),
  );

  $form['account']['conductrics_owner_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Owner Code'),
    '#default_value' => $account_info['owner_code'],
    '#size' => 35,
    '#maxlength' => 50,
    '#description' => t("Paste in from the Account > Keys page in Conductrics Console (starts with 'owner')"),
    '#required' => TRUE,
  );

  $form['account']['conductrics_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Runtime API Key'),
    '#default_value' => $account_info['api_key'],
    '#size' => 35,
    '#maxlength' => 50,
    '#description' => t("Paste in from the Account > Keys page in Conductrics Console (starts with 'api')"),
    '#required' => TRUE,
  );

  $form['account']['conductrics_admin_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Admin API Key'),
    '#default_value' => $account_info['admin_key'],
    '#size' => 35,
    '#maxlength' => 50,
    '#description' => t("Paste in from the Account > Keys page in Conductrics Console (starts with 'admin')"),
    '#required' => TRUE,
  );

  $form['account']['servers'] = array(
    '#type' => 'fieldset',
    '#title' => 'Conductrics Server Settings',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['account']['servers']['msg'] = array(
    '#markup' => t('Normally these fields should be left blank, unless you have specific instructions from Conductrics to use a private server or particular beta/update server.'),
  );

  $form['account']['servers']['conductrics_api_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API Server URL (optional)'),
    '#default_value' => $account_info['api_url'],
    '#size' => 35,
    '#maxlength' => 50,
    '#description' => t("Adjust this URL only if using a custom server from Conductrics."),
    '#required' => FALSE,
  );

  $form['account']['servers']['conductrics_console_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Console URL (optional)'),
    '#default_value' => $account_info['console_url'],
    '#size' => 35,
    '#maxlength' => 50,
    '#description' => t("Adjust this URL only if using a custom server from Conductrics."),
    '#required' => FALSE,
  );

  $goals = conductrics_mvp_goals_get();
  $form['goals'] = array(
    '#type' => 'fieldset',
    '#title' => 'Goals',
    '#prefix' => '<div id="conductrics_ajax_goals_wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  $form['goals']['msg'] = array(
    '#markup' => t("<p>You should specify at least one page that should be considered a 'Goal' for your site.</p>"),
  );
  if (!isset($form_state['goals'])) {
    $form_state['goals'] = conductrics_mvp_goals_get();
  }
  $form['goals']['add'] = array(
    '#type' => 'fieldset',
    '#attributes' => array(
      'class' => array('container-inline container-inline-inset'),
    ),
  );
  $form['goals']['add']['goal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Goal'),
    '#default_value' => '',
    '#size' => 8,
    '#maxlength' => 50,
  );
  $form['goals']['add']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('valued at'),
    '#default_value' => 1,
    '#size' => 2,
  );
  $form['goals']['add']['goal_mode'] = array(
    '#type' => 'select',
    '#title' => 'occurs',
    '#options' => array(
      'page-load' => "on page load",
    ),
    '#default_value' => 'page-load',
  );
  $form['goals']['add']['urls'] = array(
    '#type' => 'textfield',
    '#title' => t('if alias matches'),
    '#default_value' => '*', // TODO better to provide multiple patterns - for now assume commas separate patterns
    '#size' => 20,
    '#maxlength' => 500,
  );
  $form['goals']['add']['btn'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#attributes' => array('style' => 'margin-bottom:10px'),
    '#submit' => array('conductrics_mvp_ajax_add_goal'),
    '#ajax' => array(
      'callback' => 'conductrics_mvp_ajax_add_goal_callback',
      'wrapper' => 'conductrics_ajax_goals_wrapper',
    ),
  );

  $form['goals']['msg_2'] = array(
    '#markup' => t("<p>You can use <code>*</code> as a wildcard, such as <code>/content/special/*</code> if the business goal for your site were to get users to view pages in the 'special' section.</p>"),
  );

  $options = array_merge(array(), $form_state['goals']);
  foreach ($options as $code => &$value) {
    $value['goal_code'] = $code;
    $value['urls'] = implode(',', $value['urls']);
  }
  $form['goals']['existing']['goals'] = array(
    '#type' => 'tableselect',
    '#js_select' => FALSE,
    '#options' => $options,
    '#attributes' => array('style' => array('max-width:700px')),
    '#hidden' => count($options) == 0,
    '#header' => array(
      'goal_code' => array( 'data' => t('Goal'), 'width' => '30%' ),
      'value' => array( 'data' => t('Value'), 'width' => '30%' ),
      'goal_mode' => array( 'data' => t('Occurs'), 'width' => '30%' ),
      'urls' => array( 'data' => t('URL'), 'width' => '30%' ),
    ),
  );
  $form['goals']['existing']['btn'] = array(
    '#type' => 'submit',
    '#name' => 'goal_remove',
    '#value' => t('Remove Selected'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#attributes' => array('style' => 'margin-bottom:10px'),
    '#submit' => array('conductrics_mvp_ajax_remove_goal'),
    '#ajax' => array(
      'callback' => 'conductrics_mvp_ajax_remove_goal_callback',
      'wrapper' => 'conductrics_ajax_goals_wrapper',
    ),
  );

  /* TOKENS */
  if ( !isset($form_state['tokens']) ) {
    $form_state['tokens'] = isset($account_info['tokens']) ? $account_info['tokens'] : array();
  }
  $form['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => 'Tokens for Targeting',
    '#prefix' => '<div id="conductrics_ajax_tokens_wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  $form['tokens']['msg'] = array(
    '#markup' => "<p>You may pass one or more values from the user's profile to your Conductrics agents, which they will use as targeting features to inform reporting and optimization.</p>",
  );

  $form['tokens']['add'] = array(
    '#type' => 'fieldset',
    '#attributes' => array(
      'class' => array('container-inline'),
      'style' => 'border: thin solid #efefef; border-radius:4px; display:inline; background-color: #efefef; padding-top:10px;',
    ),
  );

  $form['tokens']['add']['token'] = array(
    '#type' => 'textfield',
    '#title' => t('Pass value of token'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => FALSE,
  );
  $form['tokens']['add']['prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('with prefix'),
    '#default_value' => '',
    '#size' => 10,
    '#maxlength' => 20,
    '#required' => FALSE,
  );
  $form['tokens']['add']['btn'] = array(
    '#type' => 'submit',
    '#name' => 'token_add',
    '#value' => t('Add'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#attributes' => array('style' => 'margin-bottom:10px'),
    '#submit' => array('conductrics_mvp_ajax_add_token'),
    '#ajax' => array(
      'callback' => 'conductrics_mvp_ajax_add_token_callback',
      'wrapper' => 'conductrics_ajax_tokens_wrapper',
    ),
  );

  if ( count($form_state['tokens']) > 0) {
    $options = array_merge(array(), $form_state['tokens']);
    foreach ($options as $code => &$value) {
      $value['token'] = $code;
    }
    $form['tokens']['existing']['tokens'] = array(
      '#type' => 'tableselect',
      '#js_select' => FALSE,
      '#options' => $options,
      '#attributes' => array('style' => array('max-width:700px')),
      '#hidden' => count($options) == 0,
      '#header' => array(
        'token' => array( 'data' => t('Existing Token'), 'width' => '70%' ),
        'prefix' => array( 'data' => t('Prefix'), 'width' => '30%' ),
      ),
    );

    $form['tokens']['existing']['btn'] = array(
      '#type' => 'submit',
      '#name' => 'token_remove',
      '#value' => t('Remove Selected'),
      // See the examples in ajax_example.module for more details on the
      // properties of #ajax.
      '#attributes' => array('style' => 'margin-bottom:10px'),
      '#submit' => array('conductrics_mvp_ajax_remove_token'),
      '#ajax' => array(
        'callback' => 'conductrics_mvp_ajax_remove_token_callback',
        'wrapper' => 'conductrics_ajax_tokens_wrapper',
      ),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save Configuration',
    '#prefix' => '</p>', // wrap with <p> to fix lack of spacing under button
    '#suffix' => '</p>',
  );

  return $form;
}
function conductrics_mvp_admin_validate($form, &$form_state) {
  if (!conductrics_mvp_code_valid($form_state['values']['conductrics_owner_code'])) {
    form_set_error('conductrics_owner_code', "Invalid owner code");
  }
}
function conductrics_mvp_admin_submit($form, &$form_state) {
  $account_info = array(
    'owner_code' => $form_state['values']['conductrics_owner_code'],
    'api_key' => $form_state['values']['conductrics_api_key'],
    'admin_key' => $form_state['values']['conductrics_admin_key'],
    'api_url' => $form_state['values']['conductrics_api_url'],
    'console_url' => $form_state['values']['conductrics_console_url'],
  );
  $account_info['tokens'] = $form_state['tokens'];
  variable_set('conductrics_account_info', $account_info);

  foreach ($form_state['goals'] as $goal_code => $value) {
    conductrics_mvp_goal_update($goal_code, $value);
  }
  foreach (conductrics_mvp_goals_get() as $goal_code => $value) {
    if (!isset($form_state['goals'][$goal_code])) {
      conductrics_mvp_goal_remove($goal_code);
    }
  }
}
// Process form gesture to add a goal.
function conductrics_mvp_ajax_add_goal($form, &$form_state) {
  $goal_code = $form_state['values']['goals']['add']['goal_code'];
  $goal_mode = $form_state['values']['goals']['add']['goal_mode'];
  $value = $form_state['values']['goals']['add']['value'];
  $urls = $form_state['values']['goals']['add']['urls'];
  $valid = !empty($goal_code) && conductrics_mvp_code_valid($goal_code) && !empty($goal_mode) && is_numeric($value) && !empty($urls);
  if ($valid) {
    $form_state['goals'][$goal_code] = array(
      'goal_mode' => $goal_mode,
      'value' => $value,
      'urls' => explode(',', $urls),
    );
    $form_state['rebuild'] = TRUE;
  }
}
function conductrics_mvp_ajax_add_goal_callback($form, $form_state) {
  return $form['goals'];
}
// Process form gesture to remove a goal.
function conductrics_mvp_ajax_remove_goal($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  foreach ($form_state['values']['goals']['existing']['goals'] as $code => $value) {
    if ($value == TRUE) {
      unset($form_state['goals'][$code]);
      $form_state['rebuild'] = TRUE;
    }
  }
}
function conductrics_mvp_ajax_remove_goal_callback($form, $form_state) {
  return $form['goals'];
}
// Process form gesture to add a token.
function conductrics_mvp_ajax_add_token($form, &$form_state) {
  $token_code = $form_state['values']['tokens']['add']['token'];
  $prefix = $form_state['values']['tokens']['add']['prefix'];
  $valid = !empty($token_code) && conductrics_mvp_code_valid($prefix); // TODO - validate token_code properly
  if ($valid) {
    $form_state['tokens'][$token_code] = array(
      'prefix' => $prefix,
    );
    $form_state['rebuild'] = TRUE;
  }
}
function conductrics_mvp_ajax_add_token_callback($form, $form_state) {
  return $form['tokens'];
}
// Process form gesture to remove a token.
function conductrics_mvp_ajax_remove_token($form, &$form_state) {
  foreach ($form_state['values']['tokens']['existing']['tokens'] as $token_code => $value) {
    if ($token_code == $value) {
      unset($form_state['tokens'][$value]);
      $form_state['rebuild'] = TRUE;
    }
  }
}
function conductrics_mvp_ajax_remove_token_callback($form, $form_state) {
  return $form['tokens'];
}


