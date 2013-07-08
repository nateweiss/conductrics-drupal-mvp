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
			'experiences' => array(
				'experience-a' => array(
					'beans' => array(),
				),
				'experience-b' => array(
					'beans' => array(),
				),
			),
		);
		return $values;
		//return array_merge(parent::values(), $values);
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

		$form['settings']['nodes'] = array(
			'#type' => 'node_selection',
			'#title' => t('Agent Code'),
			'#size' => 30,
			'#default_value' => $bean->settings['agent_code'],
			'#description' => 'unique code/name',
			'#required' => TRUE,
		);

	    $form['experiences'] = array(
	      '#type' => 'vertical_tabs',
	      '#tree' => TRUE,
	    );

	    $all_beans = bean_get_all_beans();
	    $bean_list = array();
	    if (!empty($all_beans)) {
	      foreach ($all_beans as $loaded_bean) {
	        if ($loaded_bean->bundle() != 'Conductrics_1') {
	          $bean_list[$loaded_bean->internalIdentifier()] = $loaded_bean->label();
	        }
	      }
	    }

		$i = 0;
	    foreach ($bean->experiences as $choice_code => $experience) {
			if (isset($experience['beans'])) {
		        $form['experiences'][$choice_code] = array(
		          '#type' => 'fieldset',
		          '#tree' => TRUE,
		          '#title' => t(ucwords(str_replace('-', ' ', $choice_code))),
		          '#group' => 'experiences',
		          '#weight' => $i++,
		        );
		        $form['experiences'][$choice_code]['beans'] = array(
		          '#type' => 'select',
		          '#title' => t('Beans'),
		          '#options' => $bean_list,
		          '#default_value' => $experience['beans'],
		          '#required' => FALSE,
		          '#multiple' => TRUE,
		        );
		    };
	  	}

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
			if (isset($experience['beans'])) {
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
		}
		return $content;
	}



}
