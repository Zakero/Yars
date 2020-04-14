/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Item Item.cpp && ./Item
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
	const std::string Attr_Vision     = "Vision";
	const std::string Skill_Acting    = "Acting";
	const std::string Skill_Medic     = "Medic";
	const std::string Skill_Mentalist = "Mentalist";
	const std::string Item_Costume    = "Costume";
	const std::string Item_Glasses    = "Body Vitials [AR]";
	const std::string Item_Stuff      = "Useless Stuff";

	Yars::Item glasses =
	{	.modify_attribute =
		{	{	Attr_Vision, 5
			}
		}
	,	.skill_attacker =
		{	Skill_Mentalist
		,	Skill_Medic
		}
	,	.skill_defender =
		{	Skill_Acting
		}
	};

	Yars::Item costume =
	{	.skill_defender =
		{	Skill_Acting
		}
	};

	Yars::Item stuff = {};
};


TEST_CASE("Item.Init", "[item]")
{
	Yars::Item item;

	REQUIRE(item.modify_attribute.empty() == true);
	REQUIRE(item.modify_skill.empty()     == true);
	REQUIRE(item.skill_attacker.empty()   == true);
	REQUIRE(item.skill_defender.empty()   == true);
	REQUIRE(item.count                    == 0);
	REQUIRE(item.affect_health_current    == 0);
	REQUIRE(item.affect_health_maximum    == 0);
}

TEST_CASE("Item.Add", "[item]")
{
	Yars yars;

	SECTION("Does not exist")
	{
		REQUIRE(yars.itemExists("") == false);
		REQUIRE(yars.itemExists(Item_Glasses) == false);
	}

	SECTION("Add invalid Item")
	{
		REQUIRE(yars.itemAdd("", glasses) == false);
		REQUIRE(yars.itemExists("") == false);
	}

	SECTION("Add valid Item")
	{
		REQUIRE(yars.itemAdd(Item_Glasses, glasses) == true);
		REQUIRE(yars.itemExists(Item_Glasses) == true);
	}

	SECTION("Don't allow duplicate Items")
	{
		yars.itemAdd(Item_Glasses, glasses);

		REQUIRE(yars.itemExists(Item_Glasses) == true);
		REQUIRE(yars.itemAdd(Item_Glasses, glasses) == false);
		REQUIRE(yars.itemExists(Item_Glasses) == true);

		//std::cout << Yars::to_string(yars.item(Item_Glasses)) << std::endl;
	}
}


TEST_CASE("Item.Get", "[item]")
{
	Yars yars;

	yars.itemAdd("A", {});

	SECTION("Non-Const")
	{
		Yars::Item& item = yars.item("A");

		REQUIRE(item.count == 0);


		Yars::Item& bad_name = yars.item("");
		REQUIRE(&bad_name == nullptr);

		Yars::Item& does_not_exist = yars.item("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::Item& item = const_yars.item("A");

		REQUIRE(item.count == 0);


		const Yars::Item& bad_name = const_yars.item("");
		REQUIRE(&bad_name == nullptr);

		const Yars::Item& does_not_exist = const_yars.item("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}
}


TEST_CASE("Item.Remove", "[item]")
{
	Yars yars;

	SECTION("Remove item using an empty name")
	{
		REQUIRE(yars.itemRemove("") == false);
	}

	SECTION("Remove item that does not exist")
	{
		REQUIRE(yars.itemRemove("Does not exist") == false);
	}

	SECTION("Remove item")
	{
		yars.itemAdd(Item_Glasses, glasses);

		REQUIRE(yars.itemRemove(Item_Glasses) == true);
		REQUIRE(yars.itemExists(Item_Glasses) == false);
	}

	SECTION("Remove items")
	{
		yars.itemAdd(Item_Glasses, glasses);
		yars.itemAdd(Item_Stuff,   stuff);

		REQUIRE(yars.itemRemove(Item_Glasses) == true);
		REQUIRE(yars.itemRemove(Item_Stuff) == true);

		REQUIRE(yars.itemExists(Item_Glasses) == false);
		REQUIRE(yars.itemExists(Item_Stuff) == false);
	}
}


TEST_CASE("Item.isUseable", "[item]")
{
	Yars yars;

	SECTION("Use an item with an empty name")
	{
		REQUIRE(yars.itemIsUsable("") == false);
	}

	SECTION("Use an item that does not exist")
	{
		REQUIRE(yars.itemIsUsable("Does not exist") == false);
	}

	SECTION("Is Usable by Attacker")
	{
		yars.itemAdd(Item_Glasses, glasses);

		REQUIRE(yars.itemIsUsable(Item_Glasses) == true);
	}

	SECTION("Is Usable by Defender")
	{
		yars.itemAdd(Item_Costume, costume);

		REQUIRE(yars.itemIsUsable(Item_Costume) == true);
	}

	SECTION("Not Usable")
	{
		yars.itemAdd(Item_Stuff, stuff);

		REQUIRE(yars.itemIsUsable(Item_Stuff) == false);
	}
}


TEST_CASE("Item.isUseableByAttacker", "[item]")
{
	Yars yars;

	SECTION("Use an item with an empty name")
	{
		REQUIRE(yars.itemIsUsableByAttacker("") == false);
	}

	SECTION("Use an item that does not exist")
	{
		REQUIRE(yars.itemIsUsableByAttacker("Does not exist") == false);
	}

	SECTION("Is Usable")
	{
		yars.itemAdd(Item_Glasses, glasses);

		REQUIRE(yars.itemIsUsableByAttacker(Item_Glasses) == true);
	}

	SECTION("Not Usable")
	{
		yars.itemAdd(Item_Stuff, stuff);

		REQUIRE(yars.itemIsUsableByAttacker(Item_Stuff) == false);
	}
}


TEST_CASE("Item.isUseableByDefender", "[item]")
{
	Yars yars;

	SECTION("Use an item with an empty name")
	{
		REQUIRE(yars.itemIsUsableByDefender("") == false);
	}

	SECTION("Use an item that does not exist")
	{
		REQUIRE(yars.itemIsUsableByDefender("Does not exist") == false);
	}

	SECTION("Is Usable")
	{
		yars.itemAdd(Item_Glasses, glasses);

		REQUIRE(yars.itemIsUsableByDefender(Item_Glasses) == true);
	}

	SECTION("Not Usable")
	{
		yars.itemAdd(Item_Stuff, stuff);

		REQUIRE(yars.itemIsUsableByDefender(Item_Stuff) == false);
	}
}


TEST_CASE("Item.List", "[item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.itemAdd(Item_Stuff, stuff);

	Yars::VectorString list = yars.itemList();

	REQUIRE(Yars::vectorContains<std::string>(list, Item_Glasses) == true);
	REQUIRE(Yars::vectorContains<std::string>(list, Item_Stuff) == true);
}


TEST_CASE("Item.Operator==", "[item]")
{
	Yars yars;

	Yars::Item orig;

	SECTION("Equal")
	{
		Yars::Item copy = orig;

		REQUIRE((orig == copy));
	}

	SECTION("Not Equal - count")
	{
		// Item Count is not checked
	}

	// Item Count is not checked
	SECTION("Not Equal - affect_attribute")
	{
		Yars::Item copy = orig;
		copy.affect_attribute = { { "foo", 42 } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - affect_skill")
	{
		Yars::Item copy = orig;
		copy.affect_skill = { { "foo", 42 } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - modify_attribute")
	{
		Yars::Item copy = orig;
		copy.modify_attribute = { { "foo", 42 } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - modify_skill")
	{
		Yars::Item copy = orig;
		copy.modify_skill = { { "foo", 42 } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - skill_attacker")
	{
		Yars::Item copy = orig;
		copy.skill_attacker = { { "foo" } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - skill_defender")
	{
		Yars::Item copy = orig;
		copy.skill_defender = { { "foo" } };

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - affect_health_current")
	{
		Yars::Item copy = orig;
		copy.affect_health_current = 42;

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - affect_health_maximum")
	{
		Yars::Item copy = orig;
		copy.affect_health_maximum = 42;

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - modify_health_current")
	{
		Yars::Item copy = orig;
		copy.modify_health_current = 42;

		REQUIRE((orig != copy));
	}

	SECTION("Not Equal - modify_health_maximum")
	{
		Yars::Item copy = orig;
		copy.modify_health_maximum = 42;

		REQUIRE((orig != copy));
	}
}
