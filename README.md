# Conductrics MVP
## Drupal Module

This is an early take on what an integrated implementation of the Conductrics service for Drupal would look like.

The basic idea at this point is to introduce a new bean type which is a "Conductrics Block". You can place an instance of the block
in a page region, with multiple "normal" content blocks within it. Think of the content blocks as "versions" or "experiences". When the page is viewed, Conductrics will select the most appropriate one (or just select one randomly, if prefered) to be shown to the visitor. Conductrics will keep track of how well each block performs in terms of conversions or whatever the site's goals are.

The current implementation stores a small amount of information within Drupal, and provides a couple of Drupal-specific forms for setting up the experiences, but is conceptually a wrapper around the "autowire" functionality provided by the Conductrics jQuery plugin (https://github.com/conductrics/conductrics-jquery). Under the covers, the Conductrics API is used to actually run the test or optimation (see http://console.conductrics.com/docs/home for a conceptual overview of the API).

### Getting Set Up

1. Install this module. Note that the module is called "conductrics_mvp" at this time (actual module name tbd).
2. Get a Conductrics account via "Get Access" link at www.conductrics.com
3. Go to Admin > Configuration > Conductrics (under Content Authoring).
4. Provide your Owner Code, Runtime API Key, and Admin API key from your Conductrics account.
   (the Admin Key is not actually used at this time, but will in later versions of this plugin)
5. Specify one or more goal pages.
   By default, there will be one set up that considers any page view to be a "success event". You can use this at first while getting familiar with the module, but at some point you should make sure that only those pages that are important to you as conceptual "conversions" are considered to be goals.
   In the future, there will be support for click, hover, and other interaction events as well as page-load type events.

### Creating A Test Or Optimization

1. Create blocks that display the content variations that you want Conductrics to test out or select dynamically.
2. Create a new Conductrics block via Admin > Content > Blocks > Add Block > Conductrics Selection.
3. Choose the "Block Selection" option, then specify the blocks with your content variations as Block 1, Block 2, etc. Start typing the name of the block, then select it from the autocomplete list (better UI for this step will be forthcoming).
4. For Conductrics Agent Code, make up a "machine name" style name for your test or optimization, such as 'hero-image-test' or 'call-to-action' or whatever makes sense. You do not need to have created the agent first in Conductrics.
5. Select one or more of the goal events you created while getting set up initially.

### Targeting

#### Targeting with information you have in Drupal

You also pass certain profile data about the visitor that you have in Drupal, to Conductrics.

Let's say you have a custom profile field called "sport", with values such as "baseball", "football" and so on. You can tell Drupal to pass the value of the field (when available for the visitor) to Conductric as targeting "features". Conductrics will keep track of which content options work best for the people with the "football" feature versus the "baseball" feature and so on. As it learns which options perform best for the different visitor features, it is able to target more and more accurately in real time.

To pass profile data to Conductrics:
1. Go to Admin > Configuration > Conductrics (under Content Authoring).
2. Under Tokens for Targeting, provide the token for the field you want to pass. User profile fields are available using the format `[current-user:field-xxx]`, where xxx is the machine name for the field (so, something like `[current-user:field-sport]`). In the future there will likely be a better way to provide the token for the field you want--for now just type it manually.
3. Provide a prefix for the token, which will appear before the value to keep things organized. For instance, if you provide "sport" as the prefix, then on the Conductrics side your features might be called "sport-baseball", "sport-football" and so on.

IMPORTANT - It doesn't make sense to use tokens that would pass information that isn't generalizable from visitor to visitor. So, don't provide tokens for visitor information such as the visitor's email address or unique id, or the current time and so on. If a value isn't likely to be present on many different user profiles over time, it isn't of much use to Conductrics and shouldn't be passed along.

#### Targeting with information supplied by Conductrics

You can set up targeting rules in the Conductrics Console to automatically "tag" your visitors with features based on their geo-location, type of browser, time of day (morning versus evening, or weekday versus weekend, etc), and more.

You don't have to do anything in the Drupal Admin to enable the Conductrics-supplied targeting. Just go straight to your Conductrics account to enable the targeting that makes sense for you. See http://console.conductrics.com/docs/home/targeting-rules for details.

### Viewing Reports

At this time, reports are available only by logging into your Conductrics account.
Later versions of this module will likely include embedded reporting.

### Creating Tests Or Optimizations That Show Up In Different Places

Sometimes you may want to have one conceptual selection "manifest itself" in multiple ways throughout your site. As an example, consider a test or
optimization about whether to offer a 10% discount code, or a 20% discount code. If a visitor is selected for the 10% code, you may want to call it out in a big "hero" on the home page, and also mention it in a smaller sidebar callout on all other pages and also perhaps in the footer. One test, multiple places.

To handle such a use case, follow the instructions for creating the test or optimization above for the first "manifestation" (so, the hero callout in our ficticious example). Then follow the same steps for the other "manifestations" (the sidebar callout and footer reminder), using the same Conductrics Agent Code each time, and the same NUMBER of blocks each time (so Block 1 is always the 10% version and Block 2 is always the 20% version or whatever). Conductrics will make the selection once (the first time each visitor hits any of the pages involved), and present the first or second version as appropriate throughout.

### TODOs, Questions, and Areas for Improvement

- At this time, the relationship between each Conductrics "Agent" and its goals is stored as Drupal variables, but could be moved to a relational table.
- Support N number of blocks in the bean's form (via "add another" button or similar), rather than a fixed limit of four.
- Use the Admin Key to retrieve list of current agent codes, etc., from Conductrics server so that existing agents can be easily reused, etc.

### Planned In Future Revisions Of This Module

- Support for Multivariate (MVT) and multi-step selections
- Support for selections at different granularities (other than block-level)
- Consider integration with the "shortcode" module (https://drupal.org/project/shortcode) to allow for easy testing of ad-hoc text and article content
- Automatically generate agent code based on block label ('machine name' style)
- Improved UI / workflow and validation
- Documentation and guidance around passing of user tokens to Conductrics as targeting features (so user doesn't unwittingly provide token data that is inherently to "unique" to be helpful to Conductrics)
- "Pick tree" for selecting user tokens for passing as targeting features
- Embedded reports
- Documentation and guidance for themers

### Notes on how the data is stored

#### Block instances
When a new instance of the Conductrics Block bean is created, its data is stored like so:

```php
array(
	'settings' => array(
		'agent_code' => 'my-agent',
		'selection_mode' => 'block-swap',
	),
	'experiences' => array(
		'block-1' => array(
			'module' => 'my-module',
			'delta' => 'my-block-a',
		),
		'block-2' => array(
			'module' => 'my-module',
			'delta' => 'my-block-b',
		),
	),
)
```

At page view time, the theme function renders this data like so in the page markup:

```html
<div data-conductrics-agent="my-agent" data-conductrics-choice="block-1">
	...first block's content...
</div>
<div data-conductrics-agent="my-agent" data-conductrics-choice="block-2">
	...second block's content...
</div>
```

...at that point, the Conductrics jQuery Plugin's 'autowire' method takes over (see the plugin's readme for details).

#### Goals / Reward events

When a goal is added in the global-level configuration page, it is added to variable "conductrics_goals", which looks like:

```php
array(
	'any-page' => array(
		'goal_mode' => 'page-load', // currently always 'page-load'
		'urls' => ['*'], // one or more patterns
		'agents' => array(
			'agent-1', 'agent-2', // the agents that should listen for this goal event
		),
	),
);
```

The 'agents' array is edited at the Conductrics Block level (in the bean's form); the other data is maintained via the module's admin form.
