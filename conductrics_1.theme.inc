<?php
/**
 * @file
 * Bean theme templates for Conductrics "Agent Experience" children.
 */

/**
 * Define theme templates.
 */
function theme_agent_experience(&$variables) {
	$content = '<div class="conductrics-experience" data-conductrics-agent="'. $variables['agent_code'] .'" data-conductrics-choice="' . $variables['choice_code'] . '">';
	foreach ( $variables['blocks'] as $display_block) {
		$content .= theme('agent_experience_block',
			array(
				'block' => $display_block,
			)
		);
	}
	$content .= '</div>';
	return $content;
}

function theme_agent_experience_block(&$variables) {
	$block_info = block_load($variables['block']['module'], $variables['block']['delta']);
	$block = _block_get_renderable_array(_block_render_blocks(array($block_info)));
	return render($block);
}

function theme_preprocess_page(&$vars, $hook) {
	drupal_add_js(drupal_get_path('theme', 'mytheme') . '/mytheme.js');
}
