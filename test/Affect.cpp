/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Affect Affect.cpp && ./Affect
 */

#ifdef CATCH_CONFIG_MAIN
#define ZAKERO_YARS_IMPLEMENTATION
#define ZAKERO_YARS_ENABLE_DEBUG
#define ZAKERO_YARS_ENABLE_ID_SHUFFLE
#define ZAKERO_YARS_ENABLE_SAFE_MODE
#endif

#include "../src/Zakero_Yars.h"
#include "catch.hpp"

using namespace zakero;

namespace
{
}


TEST_CASE("Affect.Create_and_Delete", "[affect]")
{
	Yars yars;

	SECTION("Create")
	{
		REQUIRE(yars.affectExists(42) == false);

		Yars::Id affect_id = yars.affectCreate();
		REQUIRE(yars.affectExists(affect_id) == true);
	}

	SECTION("Get")
	{
		const Yars& const_yars = yars;

		{
			Yars::Affect& affect = yars.affect(1234);
			REQUIRE(&affect == nullptr);
		}
		{
			const Yars::Affect& affect = const_yars.affect(1234);
			REQUIRE(&affect == nullptr);
		}

		Yars::Id affect_id = yars.affectCreate();

		{
			Yars::Affect& affect = yars.affect(affect_id);
			REQUIRE(&affect != nullptr);
		}
		{
			const Yars::Affect& affect = const_yars.affect(affect_id);
			REQUIRE(&affect != nullptr);
		}
	}

	SECTION("Delete")
	{
		REQUIRE(yars.affectExists(42) == false);

		bool retval = yars.affectDelete(42);
		REQUIRE(retval == false);

		Yars::Id affect_id = yars.affectCreate();

		retval = yars.affectDelete(affect_id);
		REQUIRE(retval == true);
		REQUIRE(yars.affectExists(affect_id) == false);
	}

	SECTION("Multiple")
	{
		Yars::Id affect_id[2];

		affect_id[0] = yars.affectCreate();
		affect_id[1] = yars.affectCreate();

		REQUIRE(yars.affectExists(affect_id[0]) == true);
		REQUIRE(yars.affectExists(affect_id[1]) == true);
	}

	SECTION("Reuse")
	{
		Yars::Id affect_id[3];

		affect_id[0] = yars.affectCreate();
		affect_id[1] = yars.affectCreate();

		Yars::Id id = affect_id[0];
		{
			Yars::Affect& affect = yars.affect(id);
			affect.level = 1;
			affect.origin_character = 1;
			affect.origin_skill = "foo";
		}

		REQUIRE(yars.affect(id).level == 1);
		REQUIRE(yars.affect(id).origin_character == 1);
		REQUIRE(yars.affect(id).origin_skill == "foo");

		yars.affectDelete(affect_id[0]);

		affect_id[2] = yars.affectCreate();
		affect_id[0] = yars.affectCreate();

		REQUIRE(affect_id[2] == id);

		REQUIRE(affect_id[0] != affect_id[1]);
		REQUIRE(affect_id[0] != affect_id[2]);
		REQUIRE(affect_id[1] != affect_id[2]);

		REQUIRE(yars.affect(id).level == 0);
		REQUIRE(yars.affect(id).origin_character == 0);
		REQUIRE(yars.affect(id).origin_skill.empty());
	}
}


TEST_CASE("Affect.Init", "[affect]")
{
	Yars yars;

	const std::string SKILL_NAME = "skill";
	Yars::Skill skill =
	{	.attribute_contribution =
		{	{ "attr_a", 2 }
		,	{ "attr_b", 4 }
		}
	};
	yars.skillAdd(SKILL_NAME, skill);

	const std::string SKILL_B_NAME = "skill_b";
	yars.skillAdd(SKILL_B_NAME, {});

	const std::string ITEM_NAME = "item";
	Yars::Item item =
	{	.affect_attribute      = { { "a", 0 } }
	,	.affect_skill          = { { SKILL_NAME, 1 } }
	,	.modify_attribute      = { { "attr_a", 1 } }
	,	.modify_skill          = { { SKILL_NAME, 1 } }
	,	.skill_attacker        = { SKILL_NAME, SKILL_B_NAME }
	,	.count                 = 1
	,	.affect_health_current = 10
	,	.affect_health_maximum = 2
	};
	yars.itemAdd(ITEM_NAME, item);
//{ std::cout << ITEM_NAME << ": " << Yars::to_string(item) << std::endl; }

	SECTION("Bad Affect")
	{
		bool retval = yars.affectInit(1, 2, item, "", 0);

		REQUIRE(retval == false);
	}

	SECTION("Bad Character")
	{
		Yars::Id affect_id = yars.affectCreate();

		bool retval = yars.affectInit(affect_id, 1, item, "", 0);

		REQUIRE(retval == false);
	}

	SECTION("Unusable Item")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, SKILL_NAME);

		Yars::Item foo = {};
		yars.itemAdd("foo", foo);

		bool retval = yars.affectInit(affect_id
			, character_id
			, foo
			, SKILL_NAME
			, 0
			);

		REQUIRE(retval == false);
	}

	SECTION("Bad Skill")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, SKILL_B_NAME);

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, SKILL_NAME
			, 0
			);

		REQUIRE(retval == false);
	}

	SECTION("Unsupported Skill")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		const std::string DO_NOTHING = "Do Nothing";
		yars.skillAdd(DO_NOTHING, {});
		yars.characterSkillAdd(character_id, DO_NOTHING);

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, DO_NOTHING
			, 0
			);

		REQUIRE(retval == false);
	}

	SECTION("Init: Automatic Failure - Too Much Negative")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, SKILL_NAME);
		yars.characterSkillIncrease(character_id, SKILL_NAME, 20);
		yars.characterLevelIncrease(character_id, SKILL_NAME, 20);
		// Skill Level: 6

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, SKILL_NAME
			, -10
			);

		REQUIRE(retval == false);
	}

	SECTION("Init: Automatic Failure")
	{
		yars.characterCreate(); // Use Character ID 0

		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, SKILL_NAME);
		yars.characterSkillIncrease(character_id, SKILL_NAME, 20);
		yars.characterLevelIncrease(character_id, SKILL_NAME, 20);
		// Skill Level: 6

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, SKILL_NAME
			, -4
			);

		REQUIRE(retval == false);

		Yars::Affect& affect = yars.affect(affect_id);
		REQUIRE(affect.level                 == 0);
		REQUIRE(affect.origin_character      == 0);
		REQUIRE(affect.modify_health_current == 0);
		REQUIRE(affect.modify_health_maximum == 0);

		REQUIRE(affect.experience == Approx(0));

		REQUIRE(affect.origin_skill.empty());
		REQUIRE(affect.modify_skill.empty());
		REQUIRE(affect.modify_attribute.empty());
		REQUIRE(affect.defense_skill.empty());
	}

	SECTION("Init: Automatic Success")
	{
		yars.characterCreate(); // Use Character ID 0

		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, SKILL_NAME);
		yars.characterSkillIncrease(character_id, SKILL_NAME, 20);
		yars.characterLevelIncrease(character_id, SKILL_NAME, 20);
		// Skill Level: 6

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, SKILL_NAME
			, 4
			);

		REQUIRE(retval == true);

		Yars::Affect& affect = yars.affect(affect_id);
		REQUIRE(affect.level                   == 10); // 6 + 4
		REQUIRE(affect.origin_character        == character_id);
		REQUIRE(affect.origin_skill            == SKILL_NAME);
		REQUIRE(affect.modify_health_current   == 10);
		REQUIRE(affect.modify_health_maximum   == 2);

		// No expereince with automatic success  
		REQUIRE(affect.experience == Approx(0));

		REQUIRE(affect.modify_skill.size()     == 1);
		REQUIRE(affect.modify_attribute.size() == 1);
		REQUIRE(affect.defense_skill.empty()   == true);
	}

	SECTION("Init: Automatic Success - Too Much Positive")
	{
		yars.characterCreate(); // Use Character ID 0

		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, SKILL_NAME);
		yars.characterSkillIncrease(character_id, SKILL_NAME, 20);
		yars.characterLevelIncrease(character_id, SKILL_NAME, 20);
		// Skill Level: 6

//{ Yars::Item&  item  = yars.characterItem( character_id, ITEM_NAME);  std::cout << ITEM_NAME  << ": " << Yars::to_string(item)  << std::endl; }
//{ Yars::Skill& skill = yars.characterSkill(character_id, SKILL_NAME); std::cout << SKILL_NAME << ": " << Yars::to_string(skill) << std::endl; }

		bool retval = yars.affectInit(affect_id
			, character_id
			, item
			, SKILL_NAME
			, 10
			);

//{ Yars::Affect& affect = yars.affect(affect_id); std::cout << Yars::to_string(affect) << std::endl; }

		REQUIRE(retval == true);

		Yars::Affect& affect = yars.affect(affect_id);

		// No expereince with automatic success
		REQUIRE(affect.experience              == Approx(0));
		REQUIRE(affect.level                   == 16); // 6 + 10
		REQUIRE(affect.origin_character        == character_id);
		REQUIRE(affect.origin_skill            == SKILL_NAME);
		REQUIRE(affect.modify_health_current   == 10);
		REQUIRE(affect.modify_health_maximum   == 2);
		REQUIRE(affect.modify_skill.size()     == 1);
		REQUIRE(affect.modify_attribute.size() == 1);
		REQUIRE(affect.defense_skill.empty()   == true);
	}
}

