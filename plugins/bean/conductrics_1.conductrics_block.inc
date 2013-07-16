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
				'selection_mode' => 'block-vis',
			),
			'experiences' => array(
				'experience-a' => array(
					'blocks' => array(
						array(
							'module' => 'bean',
							'delta' => 'block-a'
						),
					),
				),
				'experience-b' => array(
					'blocks' => array(
						array(
							'module' => 'bean',
							'delta' => 'block-b'
						),
					),
				),
			),
		);
		return $values;
	}

	/**
	* Builds extra settings for the block edit form.
	*/
	public function form($bean, $form, &$form_state) {
		$form = array();

		$ajax_for_block_list = array(
	    	'callback' => 'conductrics_1_element_callback',
	    	'wrapper' => 'conductrics_1_dynamic_div',
	    	'method' => 'replace',
	    	'effect' => 'fade',
	    );

		$form['settings'] = array(
			'#type' => 'fieldset',
			'#tree' => 1,
			'#title' => t('Conductrics'),
		);

		$form['settings']['agent_code'] = array(
			'#type' => 'textfield',
			'#title' => t('Agent Code'),
			'#size' => 30,
			'#default_value' => $bean->settings['agent_code'],
			'#description' => 'unique code/name',
			'#required' => TRUE,
		);

		$form['settings']['selection_mode'] = array(
			'#type' => 'radios',
			'#options' => array(
				'block-vis' => '&nbsp;' . t('<strong>Block Visibility</strong> - Show or hide one block'),
				'block-swap' => '&nbsp;' . t('<strong>Block Selection</strong> - Show one of several blocks'),
			),
			// '#default_value' => 'block-vis',
			'#title' => 'Type of Test',
		);

		// simple 'filter' to use to reduce the set of blocks down
		$form['settings']['autocomplete_blocks'] = array(
			'#type' => 'textfield',
			'#title' => t('Blocks'),
			'#autocomplete_path' => 'autocomplete/blocks',
		);

		return $form;
	}

	/**
	* Displays the bean.
	*/
	public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
		// Set markup index as empty.
		$content['#markup'] = '';
		// Loop through experiences and extract beans for display.
		foreach ($bean->experiences as $key => $experience) {
			if (isset($experience['blocks'])) {
				$content['#markup'] .= theme('agent_experience',
					array(
						'blocks' => $experience['blocks'],
						'view_mode' => $view_mode, # needed?
						'agent_code' => $bean->settings['agent_code'],
						'choice_code' => $key,
					)
				);
			}
		}
		return $content;
	}

}
