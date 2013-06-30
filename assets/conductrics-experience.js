(function( $ ) {

	// initialize Conductrics plugin for jQuery
	$.conductrics(Drupal.settings['conductrics-jquery']);
	// look at special data attributes ('data-agent' and 'data-choice') and show/hide accordingly
	$('.conductrics-experience').conductrics('autowire');

})( jQuery );
