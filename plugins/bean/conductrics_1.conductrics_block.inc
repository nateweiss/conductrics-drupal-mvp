<?php
/**
* @file
* Tweet block bean plugin.
*/
class ConductricsBlockBean extends BeanPlugin {
	/**
	* Declares default block settings.
	*/
	public function values() {
		$values = array(
			'settings' => array(
				'agent_code' => '',
			),
		);
		return array_merge(parent::values(), $values);
	}

	/**
	* Builds extra settings for the block edit form.
	*/
	public function form($bean, $form, &$form_state) {
		$form = array();

		$form['settings'] = array(
			'#type' => 'fieldset',
			'#tree' => 1,
			'#title' => t('Conductrics Agent'),
		);

		$form['settings']['agent_code'] = array(
			'#type' => 'textfield',
			'#title' => t('Agent Code'),
			'#size' => 30,
			'#default_value' => $bean->settings['agent_code'],
			'#description' => 'unique code/name',
			'#required' => TRUE,
		);

		return $form;
	}

	/**
	* Displays the bean.
	*/
	public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {

		// ***** TODO - only should do this once, not every view
		$pass = array(
			'conductrics-jquery' => array(
				'owner' => variable_get('conductrics_owner_code', ''),
				'apiKey' => variable_get('conductrics_api_key', ''),
				'baseUrl' => variable_get('conductrics_api_url', ''),
			)
		);
		drupal_add_js($pass, 'setting');
		// *****

		// Set markup index as empty.
		$content['#markup'] = '';
		$bean->data['experiences'] = array(
			'experience-a' => array(
				'beans' => array('6'),
			),
			'experience-b' => array(
				'beans' => array('7'),
			),
		);
		// Loop through experiences and extract beans for display.
		foreach ($bean->data['experiences'] as $key => $values) {
			$experience = $bean->data['experiences'][$key];
			$display_beans = bean_load_multiple($experience['beans']);
			$content['#markup'] .= theme('agent_experience',
				array(
					'entities' => $display_beans,
					'view_mode' => $view_mode, # needed?
					'agent_code' => $bean->settings['agent_code'],
					'choice_code' => $key,
				)
			);
		}
		return $content;
	}
}
