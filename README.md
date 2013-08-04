# conductrics-drupal-mvp

## GETTING SET UP

1. Install this module. Note that the module is called "conductrics_1" at this time (actual module name tbd).
2. Get a Conductrics account via "Get Access" link at www.conductrics.com
3. Goto Admin > Configuration > Conductrics (under Content Authoring)
4. Provide your Owner Code, Runtime API Key, and Admin API key from your Conductrics account.
   (the Admin Key is not actually used at this time, but will in later versions of this plugin)
5. Specify one or more goal pages.
   By default, there will be one set up that considers any page view to be a "success event". You can use this at first while getting familiar with the module, but at some point you should make sure that only those pages that are important to you as conceptual "conversions" are considered to be goals.
   In the future, there will be support for click, hover, and other interaction events as well as page-load type events.

## CREATING A TEST or OPTIMIZATION

1. Create blocks that display the content variations that you want Conductrics to test out or select dynamically.
2. Create a new Conductrics block via Admin > Content > Blocks > Add Block > Conductrics Selection.
3. Choose the "Block Selection" option, then specify the blocks with your content variations as Block 1, Block 2, etc. Start typing the name of the block, then select it from the autocomplete list (better UI for this step will be forthcoming).
4. For Conductrics Agent Code, make up a "machine name" style name for your test or optimization, such as 'hero-image-test' or 'call-to-action' or whatever makes sense. You do not need to have created the agent first in Conductrics.
5. Select one or more of the goal events you created while getting set up initially.

## VIEWING REPORTS

At this time, reports are available only by logging into your Conductrics account.
Later versions of this module will likely include embedded reporting.

## CREATING TESTS or OPTIMIZATIONS THAT SHOW UP IN DIFFERENT PLACES

Sometimes you may want to have one conceptual selection "manifest itself" in multiple ways throughout your site. As an example, consider a test or
optimization about whether to offer a 10% discount code, or a 20% discount code. If a visitor is selected for the 10% code, you may want to call it out in a big "hero" on the home page, and also mention it in a smaller sidebar callout on all other pages and also perhaps in the footer. One test, multiple places.

To handle such a use case, follow the instructions for creating the test or optimization above for the first "manifestation" (so, the hero callout in our ficticious example). Then follow the same steps for the other "manifestations" (the sidebar callout and footer reminder), using the same Conductrics Agent Code each time, and the same NUMBER of blocks each time (so Block 1 is always the 10% version and Block 2 is always the 20% version or whatever). Conductrics will make the selection once (the first time each visitor hits any of the pages involved), and present the first or second version as appropriate throughout.

## PLANNED IN FUTURE REVISIONS OF THIS MODULE

- Support for Multivariate (MVT) and multi-step selections
- Support for selections at different granularities (other than block-level)
- Automatically generate agent code based on block label ('machine name' style)
- Improved UI / workflow and validation
- Documentation and guidance around passing of user tokens to Conductrics as targeting features
- "Pick tree" for selecting user tokens for passing as targeting features
- Embedded reports
- Documentation and guidance for themers
