(function( $ ) {

	// look at special data attributes ('data-conductrics-agent' etc) and show/hide accordingly
	$.conductrics(Drupal.settings['conductrics-jquery']); // passed via drupal_add_js
	$('.conductrics-experience').conductrics('autowire'); // show appropriate content

})( jQuery );
