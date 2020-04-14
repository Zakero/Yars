/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o CharacterEquipment CharacterEquipment.cpp && ./CharacterEquipment
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
	const std::string Skill_Name = "skill";

	const std::string Item_Name = "item";
	Yars::Item item =
	{	.skill_attacker =
		{	Skill_Name
		}
	};

	const std::string Equip_Chest = "chest";

}

TEST_CASE("Character.Equipment.Item", "[character][equipment][item]")
{
	Yars yars;
	const Yars& const_yars = yars;

	SECTION("Get Item : Invalid Character")
	{
		Yars::Item& item = yars.characterEquipmentItem(1234, "location");
		REQUIRE(&item == nullptr);
	}

	SECTION("Get Item : Invalid Equipment")
	{
		Yars::Id character_id = yars.characterCreate();

		Yars::Item& item = yars.characterEquipmentItem(character_id, "location");
		REQUIRE(&item == nullptr);
	}

	SECTION("Get Item")
	{
		yars.skillAdd(Skill_Name, {});
		yars.itemAdd(Item_Name, item);

		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Name);
		yars.characterItemAdd(character_id, Item_Name);
		yars.characterEquipmentAdd(character_id, Equip_Chest, Item_Name);

		Yars::Item& equip = yars.characterEquipmentItem(character_id, Equip_Chest);

		REQUIRE(&equip != nullptr);
		REQUIRE((equip == item));
	}

	SECTION("Get Item (const) : Invalid Character")
	{
		const Yars::Item& item = const_yars.characterEquipmentItem(1234, "location");
		REQUIRE(&item == nullptr);
	}

	SECTION("Get Item (const) : Invalid Equipment")
	{
		Yars::Id character_id = yars.characterCreate();

		const Yars::Item& item = const_yars.characterEquipmentItem(character_id, "location");
		REQUIRE(&item == nullptr);
	}

	SECTION("Get Item (const)")
	{
		yars.skillAdd(Skill_Name, {});
		yars.itemAdd(Item_Name, item);

		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Name);
		yars.characterItemAdd(character_id, Item_Name);
		yars.characterEquipmentAdd(character_id, Equip_Chest, Item_Name);

		const Yars::Item& equip = const_yars.characterEquipmentItem(character_id, Equip_Chest);

		REQUIRE(&equip != nullptr);
		REQUIRE((equip == item));
	}
}

TEST_CASE("Character.Equipment.Name", "[character][equipment][item]")
{
	Yars yars;

	SECTION("Name : Invalid Character")
	{
		std::string name = yars.characterEquipmentName(1234, "invalid location");
		REQUIRE(name.empty() == true);
	}

	SECTION("Name : Invalid Equipment")
	{
		Yars::Id character_id = yars.characterCreate();

		std::string name = yars.characterEquipmentName(character_id, "invalid location");
		REQUIRE(name.empty() == true);
	}

	SECTION("Name")
	{
		yars.skillAdd(Skill_Name, {});
		yars.itemAdd(Item_Name, item);

		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Name);
		yars.characterItemAdd(character_id, Item_Name);
		yars.characterEquipmentAdd(character_id, Equip_Chest, Item_Name);

		std::string name = yars.characterEquipmentName(character_id, Equip_Chest);

		REQUIRE(name == Item_Name);
	}
}
