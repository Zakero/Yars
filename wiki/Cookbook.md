Cookbook
========

Here you will find examples and explanations of how to use \yars to 
accomplish RPG-ish things that:
- demonstrate best practices
- may not be obvious

Damage Over Time [DOT]
----------------------

blah blah Affect blah blah

Only some items are equippable
------------------------------

more blah blah

How to create a backpack or bag
-------------------------------

do the magic

Insta-Kill Affect
-----------------

There is an NPC that has a vial of poison in them that will go off in 24 
hours.  The Player has the option to disable the timer to give the NPC time 
to extract the poison.  How can I do this with Affects?

First, set up the Affect, Item, and Skill.

\code
yars.skillAdd("Vial_of_Poison_Trigger",
{	.level =
	{	.value      = 200 // Set the level here so we don't have
	,	.value_base = 200 // to worry about it in the Character.
	}
});
yars.itemAdd("Vial of Poison",
{	.skill_attacker = { "Vial_of_Poison_Trigger" }
,	.affect_health_current = Yars::Health_Max;
});

Yars::Id poison_affect = yars.affectCreate();
\endcode

If it is poison, why is the current health being added to?  You will see soon 
enough.

Next, create the NPC with all the stuff.

\code
Yars::Id npc = yars.characterCreate();
yars.characterDataSet(npc, "Time Left", int32_t(24 * 60)); // Time in minutes
yars.characterSkillAdd(npc, "Vial_of_Poison_Trigger");
yars.characterItemAdd(npc, "Vial_of_Poison");
\endcode

Setup the Affect.

\code
yars.affectInit(npc, poison_affect
	, "Vial_of_Poison"         // The Item
	, "Vial_of_Poison_Trigger" // The Skill
	, -100                     // Ensure successful initialization
	);

yars.characterAffectApply(npc, poison_affect)
\endcode

At this point, the NPC has the Vial of Poison and the poison Affect has been 
applied.  Plus the npc knows how much time is left as long as the "Time Left" 
data is updated.

Back to "How does this poison work?".  If the Player is successful in 
disabling the timer, the Affect can be _removed_.

\code
// The Player Did It! NPC is saved!
yars.characterAffectRemove(npc, poison_affect);
\endcode

When the poison timer triggers, it will _revert_ the Affect.  When an Affect 
is removed, then all the bonus are deducted from the current values.  This 
means when the `poison_affect` Affect is reverted, Yars::Health_Max will be 
subtracted from the NPC, killing them.

\code
int32_t time_left = 24;
yars.characterDataGet("Time Left", time_left);
if(time_left == 0)
{
	// If the Affect is present, revert it to kill the npc.
	// If the Affect was removed, ignore the error.
	yars.characterAffectRevert(npc, poison_affect);
}
\endcode

And finally, you should seek help for coming up with such a sick idea...
