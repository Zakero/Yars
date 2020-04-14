/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Character Character.cpp && ./Character
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
	const std::string Attr_Hearing    = "Hearing";
	const std::string Attr_Vision     = "Vision";
	const std::string Equip_Face      = "Head.Face";
	const std::string Item_Glasses    = "Body Vitials [AR]";
	const std::string Item_MagGlas    = "Magnification Lenses";
	const std::string Item_Stuff      = "Useless Stuff";
	const std::string Item_Costume    = "Costume";
	const std::string Skill_Acting    = "Acting";
	const std::string Skill_Medic     = "Medic";
	const std::string Skill_Mentalist = "Mentalist";
	const std::string Skill_Reading   = "Reading";

	Yars::Skill medic =
	{	.attribute_contribution =
		{	{	Attr_Hearing, 1 }
		,	{	Attr_Vision,  1 }
		}
	};

	Yars::Skill mentalist =
	{	.attribute_contribution =
		{	{	Attr_Vision, 1
			}
		}
	};

	constexpr int32_t Hearing_Level = 10;

	const std::string Skill_Lockpicking = "Lockpicking";
	Yars::Skill lockpicking =
	{	.attribute_contribution =
		{	{	Attr_Hearing, 3 }
		,	{	Attr_Vision,  1 }
		}
	};

	Yars::Item glasses =
	{	.modify_attribute =
		{	{	Attr_Vision, 5
			}
		}
	,	.modify_skill =
		{	{	Skill_Mentalist, 5
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
	{	.modify_skill =
		{	{	Skill_Acting, 10
			}
		}
	,	.skill_defender =
		{	Skill_Acting
		}
	};

	Yars::Item mag_glas =
	{	.modify_attribute =
		{	{	Attr_Vision, 5
			}
		}
	,	.skill_attacker =
		{	Skill_Reading
		}
	,	.skill_defender = {}
	};

	Yars::Item stuff = {};
}


TEST_CASE("Character.Create_And_Delete", "[character]")
{
	Yars yars;

	SECTION("Does not exist")
	{
		Yars::Id character_id = 0;

		REQUIRE(yars.characterExists(character_id) == false);
	}

	SECTION("Create 1")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterExists(character_id) == true);

		//std::cout << Yars::to_string(yars.character(character_id)) << std::endl;
	}

	SECTION("Create 2")
	{
		Yars::Id id_1 = yars.characterCreate();
		Yars::Id id_2 = yars.characterCreate();

		REQUIRE(yars.characterExists(id_1) == true);
		REQUIRE(yars.characterExists(id_2) == true);
	}

	SECTION("Delete 0 - Does not exist")
	{
		Yars::Id character_id = 0;

		REQUIRE(yars.characterDelete(character_id) == false);
	}

	SECTION("Delete 1")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterDelete(character_id) == true);
		REQUIRE(yars.characterExists(character_id) == false);
	}

	SECTION("Delete 2.0")
	{
		Yars::Id id_1 = yars.characterCreate();
		Yars::Id id_2 = yars.characterCreate();

		REQUIRE(yars.characterDelete(id_1) == true);
		REQUIRE(yars.characterExists(id_1) == false);
		REQUIRE(yars.characterExists(id_2) == true);

		REQUIRE(yars.characterDelete(id_2) == true);
		REQUIRE(yars.characterExists(id_1) == false);
		REQUIRE(yars.characterExists(id_2) == false);
	}

	SECTION("Delete 2.1")
	{
		Yars::Id id_1 = yars.characterCreate();
		Yars::Id id_2 = yars.characterCreate();

		REQUIRE(yars.characterDelete(id_2) == true);
		REQUIRE(yars.characterExists(id_1) == true);
		REQUIRE(yars.characterExists(id_2) == false);

		REQUIRE(yars.characterDelete(id_1) == true);
		REQUIRE(yars.characterExists(id_1) == false);
		REQUIRE(yars.characterExists(id_2) == false);
	}

	SECTION("Id recycle")
	{
		Yars::Id id_1 = yars.characterCreate();
		yars.characterCreate();

		yars.characterDelete(id_1);

		Yars::Id id_3 = yars.characterCreate();

		REQUIRE(id_1 == id_3);
	}
}


TEST_CASE("Character.Config", "[character]")
{
	Yars yars;

	SECTION("Max Stack Size")
	{
		REQUIRE(yars.characterConfigItemStackMax(1234, 5678) == false);

		Yars::Id character_id = yars.characterCreate();
		REQUIRE(yars.characterConfigItemStackMax(character_id, 0) == false);
		REQUIRE(yars.characterConfigItemStackMax(character_id, 10) == true);
	}
}


TEST_CASE("Character.Clear_With_Affect", "[character][affect]")
{
	Yars yars;

	Yars::Id character_id = yars.characterCreate();
	Yars::Id affect_id    = yars.affectCreate();

	REQUIRE(yars.characterAffectApply(character_id, affect_id) == true);
	REQUIRE(yars.characterAffectExists(character_id, affect_id) == true);

	yars.characterDelete(character_id);

	Yars::Id next_id = yars.characterCreate();
	REQUIRE(character_id == next_id);
	REQUIRE(yars.characterAffectExists(character_id, affect_id) == false);
}


TEST_CASE("Character.Health", "[character][health]")
{
	Yars yars;

	Yars::Id character = yars.characterCreate();

	SECTION("Non-Const")
	{
		Yars::Health& health = yars.characterHealth(character);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);

	
		Yars::Health& bad_id = yars.characterHealth(character ^ 0xff);
		REQUIRE(&bad_id == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::Health& health = const_yars.characterHealth(character);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);


		const Yars::Health& bad_id = const_yars.characterHealth(character ^ 0xff);
		REQUIRE(&bad_id == nullptr);
	}
}


TEST_CASE("Character.Attribute", "[attribute][character]")
{
	Yars yars;

	SECTION("Default Attribute")
	{
		Yars::Id character_id = yars.characterCreate();

		const Yars::MapStringAttribute& attr_map = yars.characterAttributeMap(character_id);
		REQUIRE(attr_map.size() == 0);
	}

	yars.skillAdd("A", { .attribute_contribution = { { "attr", 100 } } });

	Yars::Id character_id = yars.characterCreate();
	yars.characterSkillAdd(character_id, "A");

	SECTION("Non-Const")
	{
		Yars::Attribute& attribute = yars.characterAttribute(character_id, "attr");
		REQUIRE(attribute.level.value == 0);


		Yars::Attribute& bad_id = yars.characterAttribute(character_id ^ 0xff, "attr");
		REQUIRE(&bad_id == nullptr);

		Yars::Attribute& bad_name = yars.characterAttribute(character_id, "");
		REQUIRE(&bad_name == nullptr);

		Yars::Attribute& does_not_exist = yars.characterAttribute(character_id, "Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::Attribute& attribute = const_yars.characterAttribute(character_id, "attr");
		REQUIRE(attribute.level.value == 0);


		const Yars::Attribute& bad_id = const_yars.characterAttribute(character_id ^ 0xff, "attr");
		REQUIRE(&bad_id == nullptr);

		const Yars::Attribute& bad_name = const_yars.characterAttribute(character_id, "");
		REQUIRE(&bad_name == nullptr);

		const Yars::Attribute& does_not_exist = const_yars.characterAttribute(character_id, "Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}

	SECTION("Exists")
	{
		REQUIRE(yars.characterAttributeExists(character_id ^ 0xff, "") == false);
		REQUIRE(yars.characterAttributeExists(character_id, "attr") == true);
		REQUIRE(yars.characterAttributeExists(character_id, "nope") == false);
	}
}


TEST_CASE("Character.Attribute_Map", "[attribute][character]")
{
	Yars yars;

	yars.skillAdd("A", { .attribute_contribution = { { "attr", 100 } } });

	Yars::Id character_id = yars.characterCreate();
	yars.characterSkillAdd(character_id, "A");

	SECTION("Non-Const")
	{
		Yars::MapStringAttribute& map = yars.characterAttributeMap(character_id);
		REQUIRE(map.size() == 1);
		REQUIRE(map.contains("attr") == true);


		Yars::MapStringAttribute& bad_id = yars.characterAttributeMap(character_id ^ 0xff);
		REQUIRE(&bad_id == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::MapStringAttribute& map = const_yars.characterAttributeMap(character_id);
		REQUIRE(map.size() == 1);
		REQUIRE(map.contains("attr") == true);


		const Yars::MapStringAttribute& bad_id = const_yars.characterAttributeMap(character_id ^ 0xff);
		REQUIRE(&bad_id == nullptr);
	}
}


TEST_CASE("Character.Attributes_From_Skill", "[attribute][character][skill]")
{
	Yars yars;

	yars.attributeAdd(Attr_Hearing);
	yars.levelSet(yars.attribute(Attr_Hearing).level, Hearing_Level);

	yars.skillAdd(Skill_Lockpicking, lockpicking);
	yars.skillAdd(Skill_Mentalist,   mentalist);

	SECTION("Inherit unknown Attribute from Skill")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterAttributeExists(character_id, Attr_Vision) == false);

		yars.characterSkillAdd(character_id, Skill_Mentalist);
	
		REQUIRE(yars.characterAttributeExists(character_id, Attr_Vision) == true);

		Yars::Attribute& attribute = yars.characterAttribute(character_id, Attr_Vision);
		REQUIRE(attribute.level.value == 0);
	}

	SECTION("Inherit known Attribute from Skill")
	{
		Yars::Id character_id = yars.characterCreate();
	
		REQUIRE(yars.characterAttributeExists(character_id, Attr_Hearing) == false);

		yars.characterSkillAdd(character_id, Skill_Lockpicking);

		REQUIRE(yars.characterAttributeExists(character_id, Attr_Hearing) == true);

		Yars::Attribute& attribute = yars.characterAttribute(character_id, Attr_Hearing);
		REQUIRE(attribute.level.value == Hearing_Level);
	}

	SECTION("Attribute persistance")
	{
		Yars::Id character_id = yars.characterCreate();
	
		yars.characterSkillAdd(character_id, Skill_Mentalist);
		yars.characterSkillRemove(character_id, Skill_Mentalist);

		REQUIRE(yars.characterAttributeExists(character_id, Attr_Vision) == true);
	}
}


TEST_CASE("Character.Data", "[character][data]")
{
	Yars yars;

	const bool        test_bool   = true;
	const float       test_float  = 4.2f;
	const int32_t     test_int32  = 1234;
	const std::string test_string = "foo";

	bool        value_bool   = false;
	float       value_float  = 0.0f;
	int32_t     value_int32  = 0;
	std::string value_string = "";

	SECTION("Get Invalid Character Id")
	{
		REQUIRE(yars.characterDataGet(0, "nope", value_bool)   == false);
		REQUIRE(yars.characterDataGet(0, "nope", value_float)  == false);
		REQUIRE(yars.characterDataGet(0, "nope", value_int32)  == false);
		REQUIRE(yars.characterDataGet(0, "nope", value_string) == false);
	}

	SECTION("Set Invalid Character Id")
	{
		REQUIRE(yars.characterDataSet(0, "nope", value_bool)   == false);
		REQUIRE(yars.characterDataSet(0, "nope", value_float)  == false);
		REQUIRE(yars.characterDataSet(0, "nope", value_int32)  == false);
		REQUIRE(yars.characterDataSet(0, "nope", value_string) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Get Invalid Data Name")
	{
		value_bool   = test_bool;
		value_float  = test_float;
		value_int32  = test_int32;
		value_string = test_string;

		REQUIRE(yars.characterDataGet(character_id, "nope", value_bool)   == false);
		REQUIRE(yars.characterDataGet(character_id, "nope", value_float)  == false);
		REQUIRE(yars.characterDataGet(character_id, "nope", value_int32)  == false);
		REQUIRE(yars.characterDataGet(character_id, "nope", value_string) == false);

		// If not found, values should not be changed
		REQUIRE(value_bool   == test_bool);
		REQUIRE(value_float  == test_float);
		REQUIRE(value_int32  == test_int32);
		REQUIRE(value_string == test_string);
	}

	SECTION("Set Data");
	{
		REQUIRE(yars.characterDataSet(character_id, "test", test_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test", test_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test", test_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test", test_string) == true);
	}

	SECTION("Get Data")
	{
		REQUIRE(yars.characterDataGet(character_id, "test", value_bool)   == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_float)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_int32)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_string) == true);

		REQUIRE(value_bool   == test_bool);
		REQUIRE(value_float  == test_float);
		REQUIRE(value_int32  == test_int32);
		REQUIRE(value_string == test_string);
	}

	SECTION("Over-Write Data")
	{
		const bool        new_bool   = false;
		const float       new_float  = 6.9f;
		const int32_t     new_int32  = 5678;
		const std::string new_string = "bar";

		REQUIRE(yars.characterDataSet(character_id, "test", new_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test", new_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test", new_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test", new_string) == true);

		REQUIRE(yars.characterDataGet(character_id, "test", value_bool)   == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_float)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_int32)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test", value_string) == true);

		REQUIRE(value_bool   == new_bool);
		REQUIRE(value_float  == new_float);
		REQUIRE(value_int32  == new_int32);
		REQUIRE(value_string == new_string);
	}

	SECTION("Erase Data")
	{
		REQUIRE(yars.characterDataErase(character_id + 1, "test_1") == false);
		REQUIRE(yars.characterDataErase(character_id,     "test_1") == false);

		REQUIRE(yars.characterDataSet(character_id, "test_1", test_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test_1", test_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_1", test_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_1", test_string) == true);
		REQUIRE(yars.characterDataSet(character_id, "test_2", test_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test_2", test_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_2", test_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_2", test_string) == true);

		REQUIRE(yars.characterDataErase(character_id, "test_1") == true);

		// All "test_1" values should be gone
		REQUIRE(yars.characterDataGet(character_id, "test_1", value_bool)   == false);
		REQUIRE(yars.characterDataGet(character_id, "test_1", value_float)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_1", value_int32)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_1", value_string) == false);

		// All "test_2" values should still be there
		REQUIRE(yars.characterDataGet(character_id, "test_2", value_bool)   == true);
		REQUIRE(yars.characterDataGet(character_id, "test_2", value_float)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test_2", value_int32)  == true);
		REQUIRE(yars.characterDataGet(character_id, "test_2", value_string) == true);
	}

	SECTION("Clear Data")
	{
		REQUIRE(yars.characterDataClear(character_id + 1) == false);

		REQUIRE(yars.characterDataSet(character_id, "test_3", test_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test_3", test_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_3", test_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_3", test_string) == true);
		REQUIRE(yars.characterDataSet(character_id, "test_4", test_bool)   == true);
		REQUIRE(yars.characterDataSet(character_id, "test_4", test_float)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_4", test_int32)  == true);
		REQUIRE(yars.characterDataSet(character_id, "test_4", test_string) == true);

		REQUIRE(yars.characterDataClear(character_id) == true);

		// All values should be gone
		REQUIRE(yars.characterDataGet(character_id, "test_3", value_bool)   == false);
		REQUIRE(yars.characterDataGet(character_id, "test_3", value_float)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_3", value_int32)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_3", value_string) == false);
		REQUIRE(yars.characterDataGet(character_id, "test_4", value_bool)   == false);
		REQUIRE(yars.characterDataGet(character_id, "test_4", value_float)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_4", value_int32)  == false);
		REQUIRE(yars.characterDataGet(character_id, "test_4", value_string) == false);
	}
}


TEST_CASE("Character.Skill", "[character][skill]")
{
	Yars yars;

	SECTION("Invalid Character")
	{
		Yars::Skill& skill = yars.characterSkill(0, "foo");

		REQUIRE(&skill == nullptr);
	}

	SECTION("Invalid Skill")
	{
		Yars::Id character_id = yars.characterCreate();

		Yars::Skill& skill = yars.characterSkill(character_id, "foo");

		REQUIRE(&skill == nullptr);
	}

	const Yars& const_yars = yars;

	SECTION("Const - Invalid Character")
	{
		const Yars::Skill& skill = const_yars.characterSkill(0, "foo");

		REQUIRE(&skill == nullptr);
	}

	SECTION("Const - Invalid Skill")
	{
		const Yars::Id character_id = yars.characterCreate();

		const Yars::Skill& skill = const_yars.characterSkill(character_id, "foo");

		REQUIRE(&skill == nullptr);
	}
}


TEST_CASE("Character.Skill.Add", "[character][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Mentalist, mentalist);

	SECTION("Add Skill to invalid character")
	{
		REQUIRE(yars.characterSkillAdd(0, "") == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Add invalid Skill")
	{
		REQUIRE(yars.characterSkillExists(character_id, "") == false);
		REQUIRE(yars.characterSkillAdd(   character_id, "") == false);
		REQUIRE(yars.characterSkillExists(character_id, "") == false);

		// Lockpicking was not added to Yars
		REQUIRE(yars.characterSkillExists(character_id, Skill_Lockpicking) == false);
		REQUIRE(yars.characterSkillAdd(   character_id, Skill_Lockpicking) == false);
		REQUIRE(yars.characterSkillExists(character_id, Skill_Lockpicking) == false);
	}

	SECTION("Add valid Skill")
	{
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == false);
		REQUIRE(yars.characterSkillAdd(   character_id, Skill_Mentalist) == true);
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == true);

		// Already added
		REQUIRE(yars.characterSkillAdd(character_id, Skill_Mentalist) == false);
	}

	SECTION("Add valid Pre-Leveled Skill")
	{
		yars.levelSet(yars.skill(Skill_Mentalist).level, 3);
		yars.characterSkillAdd(character_id, Skill_Mentalist);

		REQUIRE(yars.characterSkill(character_id, Skill_Mentalist).level.value == 3);
	}
}


TEST_CASE("Character.Skill.Exists", "[character][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Mentalist, mentalist);

	SECTION("Check Skill in invalid character")
	{
		REQUIRE(yars.characterSkillExists(0, Skill_Mentalist) == false);
	}

	SECTION("Check Skill in valid character")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == false);
		REQUIRE(yars.characterSkillAdd(   character_id, Skill_Mentalist) == true);
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == true);
	}
}


TEST_CASE("Character.Skill.List", "[character][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Mentalist,   mentalist);
	yars.skillAdd(Skill_Lockpicking, lockpicking);

	SECTION("Skill List from invalid character")
	{
		Yars::VectorString vector = yars.characterSkillList(0);
		REQUIRE(vector.empty() == true);
	}

	SECTION("Skill List from valid character with no skills")
	{
		Yars::Id character_id = yars.characterCreate();

		Yars::VectorString vector = yars.characterSkillList(character_id);
		REQUIRE(vector.empty() == true);
	}

	SECTION("Skill List from valid character with skills")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterSkillAdd(character_id, Skill_Lockpicking) == true);
		REQUIRE(yars.characterSkillAdd(character_id, Skill_Mentalist)   == true);

		Yars::VectorString vector = yars.characterSkillList(character_id);

		REQUIRE(vector.size() == 2);
		REQUIRE(Yars::vectorContains(vector, Skill_Lockpicking) == true);
		REQUIRE(Yars::vectorContains(vector, Skill_Mentalist)   == true);
	}
}


TEST_CASE("Character.Skill.Map", "[character][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Mentalist,   mentalist);
	yars.skillAdd(Skill_Lockpicking, lockpicking);

	SECTION("Skill Map from invalid character")
	{
		Yars::MapStringSkill& map = yars.characterSkillMap(0);
		REQUIRE(&map == nullptr);
	}

	SECTION("Skill Map from valid character with no skills")
	{
		Yars::Id character_id = yars.characterCreate();

		Yars::MapStringSkill& map = yars.characterSkillMap(character_id);
		REQUIRE(map.empty() == true);
	}

	SECTION("Skill Map from valid character with skills")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterSkillAdd(character_id, Skill_Lockpicking) == true);
		REQUIRE(yars.characterSkillAdd(character_id, Skill_Mentalist)   == true);

		Yars::MapStringSkill& map = yars.characterSkillMap(character_id);

		REQUIRE(map.size() == 2);
		REQUIRE(map.contains(Skill_Lockpicking) == true);
		REQUIRE(map.contains(Skill_Mentalist)   == true);
	}
}


TEST_CASE("Character.Skill.Map.Const", "[character][skill]")
{
	Yars yars;
	const Yars& const_yars = yars;

	yars.skillAdd(Skill_Mentalist,   mentalist);
	yars.skillAdd(Skill_Lockpicking, lockpicking);

	SECTION("Skill Map from invalid character")
	{
		const Yars::MapStringSkill& map = const_yars.characterSkillMap(0);
		REQUIRE(&map == nullptr);
	}

	SECTION("Skill Map from valid character with no skills")
	{
		Yars::Id character_id = yars.characterCreate();

		const Yars::MapStringSkill& map = const_yars.characterSkillMap(character_id);
		REQUIRE(map.empty() == true);
	}

	SECTION("Skill Map from valid character with skills")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterSkillAdd(character_id, Skill_Lockpicking) == true);
		REQUIRE(yars.characterSkillAdd(character_id, Skill_Mentalist)   == true);

		const Yars::MapStringSkill& map = const_yars.characterSkillMap(character_id);

		REQUIRE(map.size() == 2);
		REQUIRE(map.contains(Skill_Lockpicking) == true);
		REQUIRE(map.contains(Skill_Mentalist)   == true);
	}
}


TEST_CASE("Character.Skill.Remove", "[character][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Mentalist, mentalist);

	SECTION("Remove Skill from invalid character")
	{
		REQUIRE(yars.characterSkillRemove(0, "") == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Remove invalid Skill")
	{
		REQUIRE(yars.characterSkillExists(character_id, "") == false);
		REQUIRE(yars.characterSkillRemove(character_id, "") == false);
		REQUIRE(yars.characterSkillExists(character_id, "") == false);
	}

	SECTION("Remove valid Skill")
	{
		yars.characterSkillAdd(character_id, Skill_Mentalist);

		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == true);
		REQUIRE(yars.characterSkillRemove(character_id, Skill_Mentalist) == true);
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == false);
	}
}


TEST_CASE("Character.Skill.Increase", "[character][skill]")
{
	Yars yars;

	SECTION("Increase Skill for an invalid character")
	{
		REQUIRE(yars.characterSkillIncrease(0, "", 1) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Increase invalid Skill")
	{
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == false);
		REQUIRE(yars.characterSkillIncrease(character_id, Skill_Mentalist, 1) == false);
		REQUIRE(yars.characterSkillExists(character_id, Skill_Mentalist) == false);
	}

	yars.skillAdd(Skill_Mentalist, mentalist);

	yars.characterSkillAdd(character_id, Skill_Mentalist);

	REQUIRE(yars.characterSkill(character_id, Skill_Mentalist).level.value == 0);

	yars.levelIncrease(yars.characterSkill(character_id, Skill_Mentalist).level, 3.0f);

	REQUIRE(yars.characterSkill(character_id, Skill_Mentalist).level.value == 2);
}


TEST_CASE("Character.Item", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	SECTION("Invalid Character")
	{
		Yars::Item& skill = yars.characterItem(0, "foo");

		REQUIRE(&skill == nullptr);
	}

	SECTION("Invalid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		Yars::Item& item = yars.characterItem(character_id, "foo");

		REQUIRE(&item == nullptr);
	}

	SECTION("Invalid Item Offset")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		Yars::Item& item = yars.characterItem(character_id, Item_Glasses, 2);

		REQUIRE(&item == nullptr);
	}

	SECTION("Valid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		Yars::Item& item = yars.characterItem(character_id, Item_Glasses);

		REQUIRE(&item != nullptr);
	}
}


TEST_CASE("Character.Item.Const", "[character][item]")
{
	Yars yars;
	yars.itemAdd(Item_Glasses, glasses);

	const Yars& const_yars = yars;

	SECTION("Const - Invalid Character")
	{
		const Yars::Item& item = const_yars.characterItem(0, "foo");

		REQUIRE(&item == nullptr);
	}

	SECTION("Const - Invalid Item")
	{
		const Yars::Id character_id = yars.characterCreate();

		const Yars::Item& item = const_yars.characterItem(character_id, "foo");

		REQUIRE(&item == nullptr);
	}

	SECTION("Const - Invalid Item Offset")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		const Yars::Item& item = const_yars.characterItem(character_id, Item_Glasses, 2);

		REQUIRE(&item == nullptr);
	}

	SECTION("Const - Valid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		const Yars::Item& item = const_yars.characterItem(character_id, Item_Glasses);

		REQUIRE(&item != nullptr);
	}
}


TEST_CASE("Character.Item.Add", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	SECTION("Add Item to invalid Character")
	{
		REQUIRE(yars.characterExists(0) == false);
		REQUIRE(yars.characterItemAdd(0,    Item_Glasses) == false);
		REQUIRE(yars.characterItemExists(0, Item_Glasses) == false);
		REQUIRE(yars.characterExists(0) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Add invalid Item")
	{
		REQUIRE(yars.characterItemExists(character_id, "") == false);
		REQUIRE(yars.characterItemAdd(character_id,    "") == false);
		REQUIRE(yars.characterItemExists(character_id, "") == false);
	}

	SECTION("Add valid Item")
	{
		//std::cout << Yars::to_string(yars.character(character_id)) << std::endl;
		REQUIRE(yars.characterItemAdd(character_id,    Item_Glasses, 0) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses)    == false);

		REQUIRE(yars.characterItemAdd(character_id,    Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);
	}

	SECTION("Add Items: Validate from Character")
	{
		yars.characterItemAdd(character_id, Item_Glasses);

		Yars::Item& char_item = yars.characterItem(character_id, Item_Glasses);
		Yars::Item& yars_item = yars.item(Item_Glasses);

		REQUIRE((char_item == yars_item));
	}

	SECTION("Add Items: Stack 1")
	{
		// Stack of 1 is the default

		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses) == true);

		{
			Yars::Item& item = yars.characterItem(character_id, Item_Glasses);
			REQUIRE(item.count == 1);
		}

		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses) == true);

		{
			Yars::Item& item = yars.characterItem(character_id, Item_Glasses, 1);
			REQUIRE(item.count == 1);
		}

		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses, 2) == true);

		{
			Yars::Item& item = yars.characterItem(character_id, Item_Glasses, 2);
			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id, Item_Glasses, 3);
			REQUIRE(item.count == 1);
		}
	}

	SECTION("Add Items: Stack 2")
	{
		yars.characterConfigItemStackMax(character_id, 2);

		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);

		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses) == true);

		Yars::Item& item_0 = yars.characterItem(character_id, Item_Glasses, 0);
		Yars::Item& item_1 = yars.characterItem(character_id, Item_Glasses, 1);

		if(item_0.count == 1)
		{
			REQUIRE(item_1.count == 2);
		}
		else
		{
			REQUIRE(item_1.count == 1);
		}
	}

	SECTION("Add Items: Stack 3")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses, 5) == true);

		Yars::Item& item_0 = yars.characterItem(character_id, Item_Glasses, 0);
		Yars::Item& item_1 = yars.characterItem(character_id, Item_Glasses, 1);

		if(item_0.count == 3)
		{
			REQUIRE(item_1.count == 2);
		}
		else
		{
			REQUIRE(item_1.count == 3);
		}
	}

	SECTION("Add Items: Stack 4")
	{
		yars.characterConfigItemStackMax(character_id, 1);
		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses, 2) == true);

		yars.characterConfigItemStackMax(character_id, 2);
		REQUIRE(yars.characterItemAdd(character_id, Item_Glasses, 2) == true);

		Yars::Item& item_0 = yars.characterItem(character_id, Item_Glasses, 0);
		Yars::Item& item_1 = yars.characterItem(character_id, Item_Glasses, 1);

		REQUIRE(item_0.count == 2);
		REQUIRE(item_1.count == 2);
	}
}


TEST_CASE("Character.Item.IsUsable", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.itemAdd(Item_Costume, costume);

	yars.skillAdd(Skill_Medic,  {});
	yars.skillAdd(Skill_Acting, {});

	bool is_usable = false;

	SECTION("Is Usable - Invalid Character")
	{
		Yars::Id character_id = 1234;

		is_usable = yars.characterItemIsUsable(character_id, Item_Glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable - Invalid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		is_usable = yars.characterItemIsUsable(character_id, Item_Glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable - Valid but not usable")
	{
		Yars::Id character_id = yars.characterCreate();

		const std::string Item_Junk = "Junk";
		yars.itemAdd(Item_Junk, {});

		yars.characterItemAdd(character_id, Item_Junk);

		is_usable = yars.characterItemIsUsable(character_id, Item_Junk);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		yars.characterSkillAdd(character_id, Skill_Medic);
		is_usable = yars.characterItemIsUsable(character_id, Item_Glasses);
		REQUIRE(is_usable == true);
		yars.characterSkillRemove(character_id, Skill_Medic);

		yars.characterSkillAdd(character_id, Skill_Acting);
		is_usable = yars.characterItemIsUsable(character_id, Item_Glasses);
		REQUIRE(is_usable == true);
		yars.characterSkillRemove(character_id, Skill_Acting);
	}
}


TEST_CASE("Character.Item.IsUsableByAttacker", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.skillAdd(Skill_Medic, medic);

	bool is_usable = false;

	SECTION("Is Usable By Attacker- Invalid Character")
	{
		Yars::Id character_id = 1234;

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses, Skill_Medic);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Attacker- Invalid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses, Skill_Medic);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Attacker - Item does not have Skill")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		const std::string Invalid_Skill = "Invalid Skill";
		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses, Invalid_Skill);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Attacker")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterSkillAdd(character_id, Skill_Medic);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses);
		REQUIRE(is_usable == true);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, Item_Glasses, Skill_Medic);
		REQUIRE(is_usable == true);

		is_usable = yars.characterItemIsUsableByAttacker(character_id, glasses);
		REQUIRE(is_usable == true);
	}
}


TEST_CASE("Character.Item.IsUsableByDefender", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses,  glasses);
	yars.skillAdd(Skill_Acting, {});

	bool is_usable = false;

	SECTION("Is Usable By Defender- Invalid Character")
	{
		Yars::Id character_id = 1234;

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses, Skill_Acting);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByDefender(character_id, glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Defender- Invalid Item")
	{
		Yars::Id character_id = yars.characterCreate();

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses, Skill_Acting);
		REQUIRE(is_usable == false);

		is_usable = yars.characterItemIsUsableByDefender(character_id, glasses);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Defender - Item does not have Skill")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);

		const std::string Invalid_Skill = "Invalid Skill";
		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses, Invalid_Skill);
		REQUIRE(is_usable == false);
	}

	SECTION("Is Usable By Defender")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterSkillAdd(character_id, Skill_Acting);

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses);
		REQUIRE(is_usable == true);

		is_usable = yars.characterItemIsUsableByDefender(character_id, Item_Glasses, Skill_Acting);
		REQUIRE(is_usable == true);

		is_usable = yars.characterItemIsUsableByDefender(character_id, glasses);
		REQUIRE(is_usable == true);
	}
}


TEST_CASE("Character.Item.List", "[character][item]")
{
	Yars yars;

	const std::string Apple  = "Apple";
	const std::string Biscut = "Biscut";
	const std::string Carrot = "Carrot";

	yars.itemAdd(Apple, {});
	yars.itemAdd(Biscut, {});
	yars.itemAdd(Carrot, {});

	SECTION("Invalid Character")
	{
		Yars::VectorString list = yars.characterItemList(0);
		REQUIRE(list.empty() == true);
	}

	SECTION("Invalid Character")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterItemAdd(character_id, Apple);
		yars.characterItemAdd(character_id, Biscut);
		yars.characterItemAdd(character_id, Carrot);

		Yars::VectorString list = yars.characterItemList(character_id);

		REQUIRE(list.size() == 3);
		REQUIRE(Yars::vectorContains(list, Apple)  == true);
		REQUIRE(Yars::vectorContains(list, Biscut) == true);
		REQUIRE(Yars::vectorContains(list, Carrot) == true);
	}
}


TEST_CASE("Character.Item.Restack", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.itemAdd(Item_Stuff,   stuff);

	SECTION("Restack All Items: Invalid Character")
	{
		yars.characterItemRestack(1234);
		yars.characterItemRestack(1234, Item_Stuff);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Restack All Items: Increase")
	{
		yars.characterConfigItemStackMax(character_id, 1);

		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);

		uint32_t stacks = 0;
		uint32_t count  = yars.characterItemCount(character_id, Item_Glasses, stacks);

		REQUIRE(count  == 3);
		REQUIRE(stacks == 3);

		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);

		stacks = 0;
		count  = yars.characterItemCount(character_id, Item_Stuff, stacks);

		REQUIRE(count  == 6);
		REQUIRE(stacks == 6);

		yars.characterConfigItemStackMax(character_id, 3);
		yars.characterItemRestack(character_id);

		stacks = 0;
		count  = yars.characterItemCount(character_id, Item_Glasses, stacks);

		REQUIRE(count  == 3);
		REQUIRE(stacks == 1);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Glasses
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 3);
		}

		stacks = 0;
		count  = yars.characterItemCount(character_id, Item_Stuff, stacks);

		REQUIRE(count  == 6);
		REQUIRE(stacks == 2);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 3);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 1 // Stack Offset
				);

			REQUIRE(item.count == 3);
		}
	}

	SECTION("Restack All Items: Decrease")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);

		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);

		yars.characterConfigItemStackMax(character_id, 1);
		yars.characterItemRestack(character_id);

		uint32_t stacks = 0;
		uint32_t count  = yars.characterItemCount(character_id, Item_Glasses, stacks);

		REQUIRE(count  == 3);
		REQUIRE(stacks == 3);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Glasses
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Glasses
				, 1 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Glasses
				, 2 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}

		count = yars.characterItemCount(character_id, Item_Stuff, stacks);

		REQUIRE(count  == 2);
		REQUIRE(stacks == 2);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 1 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
	}

	SECTION("Restack Item")
	{
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);

		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);
		yars.characterItemAdd(character_id, Item_Stuff);

		yars.characterConfigItemStackMax(character_id, 3);
		yars.characterItemRestack(character_id, Item_Glasses);

		uint32_t stacks = 0;
		uint32_t count  = yars.characterItemCount(character_id, Item_Glasses, stacks);

		REQUIRE(count  == 3);
		REQUIRE(stacks == 1);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Glasses
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 3);
		}

		stacks = 0;
		count  = yars.characterItemCount(character_id, Item_Stuff, stacks);

		REQUIRE(count  == 3);
		REQUIRE(stacks == 3);

		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 0 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 1 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
		{
			Yars::Item& item = yars.characterItem(character_id
				, Item_Stuff
				, 2 // Stack Offset
				);

			REQUIRE(item.count == 1);
		}
	}

	SECTION("Restack invalid Item")
	{
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
		yars.characterItemRestack(character_id, Item_Glasses);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}
}


TEST_CASE("Character.Item.Skill", "[character][item][skill]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	yars.skillAdd(Skill_Acting, {});
	yars.skillAdd(Skill_Medic, {});
	yars.skillAdd(Skill_Mentalist, {});

	Yars::Id character = yars.characterCreate();
	yars.characterItemAdd(character, Item_Glasses);

	yars.characterSkillAdd(character, Skill_Acting);
	yars.characterSkillIncrease(character, Skill_Acting, 20);
	yars.characterLevelIncrease(character, Skill_Acting, 20);

	yars.characterSkillAdd(character, Skill_Medic);
	yars.characterSkillIncrease(character, Skill_Medic, 30);
	yars.characterLevelIncrease(character, Skill_Medic, 30);

	yars.characterSkillAdd(character, Skill_Mentalist);
	yars.characterSkillIncrease(character, Skill_Mentalist, 10);
	yars.characterLevelIncrease(character, Skill_Mentalist, 10);

	std::string skill_name = "";
	Yars::VectorString skill_list;

	SECTION("Attacker: Invalid Character")
	{
		skill_name = yars.characterItemSkillAttacker(character ^ 1234, Item_Glasses);
		REQUIRE(skill_name.empty());
	}

	SECTION("Attacker: No Skill")
	{
		Yars::Id character = yars.characterCreate();

		skill_name = yars.characterItemSkillAttacker(character, Item_Glasses);
		REQUIRE(skill_name.empty());
	}

	SECTION("Attacker: No Skill List")
	{
		const std::string Skill_1 = "Skill_1";
		const std::string Skill_2 = "Skill_2";
		const std::string Skill_3 = "Skill_3";
		const std::string Skill_4 = "Skill_4";

		yars.skillAdd(Skill_1, {});
		yars.skillAdd(Skill_2, {});
		yars.skillAdd(Skill_3, {});
		yars.skillAdd(Skill_4, {});

		const std::string Item_Thing = "Thing";
		Yars::Item thing =
		{	.skill_attacker =
			{	Skill_1
			,	Skill_Mentalist
			,	Skill_2
			,	Skill_Medic
			,	Skill_3
			}
		};

		yars.itemAdd(Item_Thing, thing);

		Yars::Id character = yars.characterCreate();

		skill_list = yars.characterItemSkillAttackerList(character, Item_Thing);
		REQUIRE(skill_list.empty());

		skill_list = yars.characterItemSkillAttackerList(character ^ 124, Item_Thing);
		REQUIRE(skill_list.empty() == true);

		yars.characterItemAdd(character, Item_Thing);
		skill_list = yars.characterItemSkillAttackerList(character, Item_Thing);
		REQUIRE(skill_list.empty() == true);
	}

	SECTION("Attacker: Best Skill 1")
	{
		skill_name = yars.characterItemSkillAttacker(character, Item_Glasses);
		REQUIRE(skill_name == Skill_Medic);
	}

	SECTION("Attacker: Best Skill 2")
	{
		const std::string Skill_1 = "Skill_1";
		const std::string Skill_2 = "Skill_2";
		const std::string Skill_3 = "Skill_3";
		const std::string Skill_4 = "Skill_4";

		yars.skillAdd(Skill_1, {});
		yars.skillAdd(Skill_2, {});
		yars.skillAdd(Skill_3, {});
		yars.skillAdd(Skill_4, {});

		const std::string Item_Thing = "Thing";
		Yars::Item thing =
		{	.skill_attacker =
			{	Skill_1
			,	Skill_Mentalist
			,	Skill_2
			,	Skill_Medic
			,	Skill_3
			}
		};

		yars.itemAdd(Item_Thing, thing);

		Yars::Id character_id = yars.characterCreate();
		yars.characterItemAdd(character_id, Item_Thing);
		yars.characterSkillAdd(character_id, Skill_Medic);
		yars.characterSkillAdd(character_id, Skill_Mentalist);

		yars.levelSet(
			yars.characterSkill(character_id, Skill_Medic).level, 4
			);

		yars.levelSet(
			yars.characterSkill(character_id, Skill_Mentalist).level, 6
			);

		skill_name = yars.characterItemSkillAttacker(character_id, Item_Thing);
		REQUIRE(skill_name == Skill_Mentalist);
	}

	SECTION("Attacker: All Skills")
	{
		skill_list = yars.characterItemSkillAttackerList(character, Item_Glasses);
		REQUIRE(skill_list.size() == 2);
		REQUIRE(Yars::vectorContains(skill_list, Skill_Medic) == true);
		REQUIRE(Yars::vectorContains(skill_list, Skill_Mentalist) == true);
	}

	SECTION("Defender: Invalid Character")
	{
		skill_name = yars.characterItemSkillDefender(character ^ 1234, Item_Glasses);
		REQUIRE(skill_name.empty());
	}

	SECTION("Defender: No Skill")
	{
		Yars::Id character = yars.characterCreate();

		skill_name = yars.characterItemSkillDefender(character, Item_Glasses);
		REQUIRE(skill_name.empty());
	}

	SECTION("Defender: No Skill List")
	{
		const std::string Skill_1 = "Skill_1";
		const std::string Skill_2 = "Skill_2";
		const std::string Skill_3 = "Skill_3";
		const std::string Skill_4 = "Skill_4";

		yars.skillAdd(Skill_1, {});
		yars.skillAdd(Skill_2, {});
		yars.skillAdd(Skill_3, {});
		yars.skillAdd(Skill_4, {});

		const std::string Item_Thing = "Thing";
		Yars::Item thing =
		{	.skill_defender =
			{	Skill_1
			,	Skill_Mentalist
			,	Skill_2
			,	Skill_Medic
			,	Skill_3
			}
		};

		yars.itemAdd(Item_Thing, thing);

		Yars::Id character = yars.characterCreate();

		skill_list = yars.characterItemSkillDefenderList(character, Item_Thing);
		REQUIRE(skill_list.empty());

		skill_list = yars.characterItemSkillDefenderList(character ^ 124, Item_Thing);
		REQUIRE(skill_list.empty() == true);

		yars.characterItemAdd(character, Item_Thing);
		skill_list = yars.characterItemSkillDefenderList(character, Item_Thing);
		REQUIRE(skill_list.empty() == true);
	}

	SECTION("Defender: Best Skill 1")
	{
		skill_name = yars.characterItemSkillDefender(character, Item_Glasses);
		REQUIRE(skill_name == Skill_Acting);
	}

	SECTION("Defender: Best Skill 2")
	{
		const std::string Skill_1 = "Skill_1";
		const std::string Skill_2 = "Skill_2";
		const std::string Skill_3 = "Skill_3";
		const std::string Skill_4 = "Skill_4";

		yars.skillAdd(Skill_1, {});
		yars.skillAdd(Skill_2, {});
		yars.skillAdd(Skill_3, {});
		yars.skillAdd(Skill_4, {});

		const std::string Item_Thing = "Thing";
		Yars::Item thing =
		{	.skill_defender =
			{	Skill_1
			,	Skill_Mentalist
			,	Skill_2
			,	Skill_Medic
			,	Skill_3
			}
		};

		yars.itemAdd(Item_Thing, thing);

		Yars::Id character_id = yars.characterCreate();
		yars.characterItemAdd(character_id, Item_Thing);
		yars.characterSkillAdd(character_id, Skill_Medic);
		yars.characterSkillAdd(character_id, Skill_Mentalist);

		yars.levelSet(
			yars.characterSkill(character_id, Skill_Medic).level, 4
			);

		yars.levelSet(
			yars.characterSkill(character_id, Skill_Mentalist).level, 6
			);

		skill_name = yars.characterItemSkillDefender(character_id, Item_Thing);
		REQUIRE(skill_name == Skill_Mentalist);
	}

	SECTION("Defender: All Skills")
	{
		skill_list = yars.characterItemSkillDefenderList(character, Item_Glasses);
		REQUIRE(skill_list.size() == 1);
		REQUIRE(Yars::vectorContains(skill_list, Skill_Acting) == true);
	}
}


TEST_CASE("Character.Item.Count", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	uint32_t stack_count = 0;

	SECTION("Invalid Character")
	{
		REQUIRE(yars.characterItemCount(0, "") == 0);
	}

	SECTION("Invalid Character - Stack Count")
	{
		REQUIRE(yars.characterItemCount(0, "", stack_count) == 0);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Invalid Item")
	{
		REQUIRE(yars.characterItemCount(character_id, "") == 0);
	}

	SECTION("Invalid Item - Stack Count")
	{
		REQUIRE(yars.characterItemCount(character_id, "", stack_count) == 0);
	}

	SECTION("Count individule items")
	{
		yars.characterItemAdd(character_id, Item_Glasses, 3);
		REQUIRE(yars.characterItemCount(character_id, Item_Glasses) == 3);
		yars.characterItemRemove(character_id, Item_Glasses, 3);

		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Count stacks items")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		yars.characterItemAdd(character_id, Item_Glasses, 8);
		REQUIRE(yars.characterItemCount(character_id, Item_Glasses) == 8);
		yars.characterItemRemove(character_id, Item_Glasses, 8);

		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}
}


TEST_CASE("Character.Item.Remove", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	SECTION("Remove Item from invalid character")
	{
		REQUIRE(yars.characterExists(0) == false);
		REQUIRE(yars.characterItemRemove(0, Item_Glasses) == false);
		REQUIRE(yars.characterItemExists(0, Item_Glasses) == false);
		REQUIRE(yars.characterExists(0) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Remove invalid Item")
	{
		REQUIRE(yars.characterItemRemove(character_id, "") == false);
		REQUIRE(yars.characterItemExists(character_id, "") == false);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Remove valid Item")
	{
		yars.characterItemAdd(character_id, Item_Glasses);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Remove Items from collection")
	{
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_Glasses);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Remove Items from stack")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		yars.characterItemAdd(character_id, Item_Glasses, 2);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Remove Items from collections and stack")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		yars.characterItemAdd(character_id, Item_Glasses, 5);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses, 4) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);

		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Remove more Items than what exists")
	{
		yars.characterItemAdd(character_id, Item_Glasses, 1);
		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses, 10) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);

		yars.characterConfigItemStackMax(character_id, 3);
		yars.characterItemAdd(character_id, Item_Glasses, 5);
		REQUIRE(yars.characterItemRemove(character_id, Item_Glasses, 10) == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}
}


TEST_CASE("Character.Item.Take", "[character][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	SECTION("Take Item from invalid character")
	{
		REQUIRE(yars.characterExists(0) == false);

		Yars::Item item = yars.characterItemTake(0, Item_Glasses);
		REQUIRE((item != yars.item(Item_Glasses)));

		REQUIRE(yars.characterItemExists(0, Item_Glasses) == false);
		REQUIRE(yars.characterExists(0) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Take invalid Item")
	{
		yars.characterItemTake(character_id, "");
		REQUIRE(yars.characterItemExists(character_id, "") == false);

		Yars::Item item = yars.characterItemTake(character_id, Item_Glasses);
		REQUIRE((item != yars.item(Item_Glasses)));
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Take valid Item")
	{
		yars.characterItemAdd(character_id, Item_Glasses);

		Yars::Item item = yars.characterItemTake(character_id, Item_Glasses);
		REQUIRE((item == yars.item(Item_Glasses)));
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Take valid Items")
	{
		yars.characterItemAdd(character_id, Item_Glasses, 2);

		Yars::Item item;
		item = yars.characterItemTake(character_id, Item_Glasses);
		REQUIRE((item == yars.item(Item_Glasses)));
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);

		item = yars.characterItemTake(character_id, Item_Glasses);
		REQUIRE((item == yars.item(Item_Glasses)));
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}
}


TEST_CASE("Character.Equipment.Add", "[character][equipment][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.itemAdd(Item_Stuff,   stuff);

	SECTION("Equip Item from invalid character")
	{
		REQUIRE(yars.characterExists(0) == false);
		REQUIRE(yars.characterItemExists(0, Item_Glasses) == false);
		REQUIRE(yars.characterEquipmentExists(0, Equip_Face)   == false);
		REQUIRE(yars.characterEquipmentAdd(0,    Equip_Face, Item_Glasses) == false);
		REQUIRE(yars.characterEquipmentExists(0, Equip_Face)   == false);
		REQUIRE(yars.characterItemExists(0, Item_Glasses) == false);
		REQUIRE(yars.characterExists(0) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Equip invalid Item")
	{
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face)   == false);
		REQUIRE(yars.characterEquipmentAdd(character_id,    Equip_Face, Item_Glasses) == false);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face)   == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);
	}

	SECTION("Equip valid Item")
	{
		yars.characterItemAdd(character_id, Item_Glasses);

		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face)   == false);
		REQUIRE(yars.characterEquipmentAdd(character_id,    Equip_Face, Item_Glasses) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face)   == true);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == false);

		yars.characterItemAdd(character_id, Item_Glasses);
		REQUIRE(yars.characterEquipmentAdd(character_id,    Equip_Face, Item_Glasses) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);
	}

	SECTION("Equip unusable Item")
	{
		yars.characterItemAdd(character_id, Item_Stuff);

		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);
		REQUIRE(yars.characterEquipmentAdd(character_id,    Equip_Face, Item_Stuff) == false);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Stuff) == true);
	}
}


TEST_CASE("Character.Equipment.Remove", "[character][equipment][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);

	SECTION("Remove equipment from invalid character")
	{
		REQUIRE(yars.characterEquipmentRemove(0, "") == false);
		REQUIRE(yars.characterEquipmentExists(0, "") == false);

		REQUIRE(yars.characterEquipmentRemove(0, Equip_Face) == false);
		REQUIRE(yars.characterEquipmentExists(0, Equip_Face) == false);
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Remove invalid equipment")
	{
		REQUIRE(yars.characterEquipmentRemove(character_id, "") == false);
		REQUIRE(yars.characterEquipmentExists(character_id, "") == false);

		REQUIRE(yars.characterEquipmentRemove(character_id, Equip_Face) == false);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);
	}

	SECTION("Remove valid equipment")
	{
		yars.characterConfigItemStackMax(character_id, 1);

		yars.characterItemAdd(character_id, Item_Glasses, 2);
		yars.characterEquipmentAdd(character_id, Equip_Face, Item_Glasses);

		REQUIRE(yars.characterEquipmentRemove(character_id, Equip_Face) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);
	}

	SECTION("Remove valid equipment with restack")
	{
		yars.characterConfigItemStackMax(character_id, 3);

		yars.characterItemAdd(character_id, Item_Glasses, 2);
		Yars::Item& item = yars.characterItem(character_id, Item_Glasses);
		REQUIRE(item.count == 2);

		yars.characterEquipmentAdd(character_id, Equip_Face, Item_Glasses);
		REQUIRE(item.count == 1);

		REQUIRE(yars.characterEquipmentRemove(character_id, Equip_Face) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);

		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);
		REQUIRE(item.count == 2);
	}

	SECTION("Remove equipment when item doesn't exist")
	{
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterEquipmentAdd(character_id, Equip_Face, Item_Glasses);
		yars.itemRemove(Item_Glasses);

		REQUIRE(yars.characterEquipmentRemove(character_id, Equip_Face) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, Equip_Face) == false);
		REQUIRE(yars.characterItemExists(character_id, Item_Glasses) == true);
	}

	/**
	 * \todo Add test to unequip item when the inventory is full
	 *       - Item needs to know when it is full first.
	 *       - Requires sometype of management system.
	 *       - Save for a future version.
	 */
}


TEST_CASE("Character.Equipment.with.Modifiers.Attribute", "[attribute][character][equipment][item]")
{
	Yars yars;

	SECTION("Equip Item with Attribute Modifiers")
	{
		yars.skillAdd(Skill_Mentalist, mentalist);
		yars.itemAdd(Item_Glasses, glasses);

		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, Skill_Mentalist);
		Yars::levelSet(yars.characterAttribute(character_id, Attr_Vision).level, 1);

		yars.characterItemAdd(character_id, Item_Glasses);

		int32_t attr_before = yars.characterAttribute(character_id, Attr_Vision).level.value;

		REQUIRE(attr_before == 1);

		yars.characterEquipmentAdd(character_id, Equip_Face, Item_Glasses);

		int32_t attr_after = yars.characterAttribute(character_id, Attr_Vision).level.value;

		REQUIRE(attr_before != attr_after);
		REQUIRE(attr_after == 6);

		yars.characterEquipmentRemove(character_id, Equip_Face);

		attr_after = yars.characterAttribute(character_id, Attr_Vision).level.value;

		REQUIRE(attr_before == attr_after);
		REQUIRE(attr_after == 1);
	}
}


TEST_CASE("Character.Equipment.with.Modifiers.Skill", "[character][equipment][item][skill]")
{
	Yars yars;

	SECTION("Equip Item with Skill Modifiers")
	{
		yars.skillAdd(Skill_Mentalist, mentalist);
		yars.itemAdd(Item_Glasses, glasses);

		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, Skill_Mentalist);
		yars.characterSkillIncrease(character_id, Skill_Mentalist, 7);

		yars.characterItemAdd(character_id, Item_Glasses);

		int32_t skill_before = yars.characterSkill(character_id, Skill_Mentalist).level.value;

		REQUIRE(skill_before == 4);

		yars.characterEquipmentAdd(character_id, Equip_Face, Item_Glasses);

		int32_t skill_after = yars.characterSkill(character_id, Skill_Mentalist).level.value;

		REQUIRE(skill_before != skill_after);
		REQUIRE(skill_after == 9);

		yars.characterEquipmentRemove(character_id, Equip_Face);

		skill_after = yars.characterSkill(character_id, Skill_Mentalist).level.value;

		REQUIRE(skill_before == skill_after);
		REQUIRE(skill_after == 4);
	}
}


TEST_CASE("Character.Equipment.Change_Example", "[character][data][equipment][item]")
{
	Yars yars;

	yars.itemAdd(Item_Glasses, glasses);
	yars.itemAdd(Item_MagGlas, mag_glas);

	Yars::Id character_id = yars.characterCreate();

	SECTION("Change Glasses")
	{
		yars.characterItemAdd(character_id, Item_Glasses);
		yars.characterItemAdd(character_id, Item_MagGlas);

		REQUIRE(yars.characterEquipmentAdd(character_id, "Face", Item_Glasses) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, "Face") == true);

		if(yars.characterEquipmentExists(character_id, "Face") == true)
		{
			REQUIRE(yars.characterEquipmentRemove(character_id, "Face") == true);
		}

		REQUIRE(yars.characterEquipmentAdd(character_id, "Face", Item_MagGlas) == true);
		REQUIRE(yars.characterEquipmentExists(character_id, "Face") == true);
	}
}


TEST_CASE("Character.Level.Increase.Health", "[character][health][level]")
{
	Yars yars;

	SECTION("Invalid Character")
	{
		bool success = yars.characterLevelIncrease(1234, 10);
		REQUIRE(success == false);
	}

	SECTION("Level Up")
	{
		Yars::Id character_id = yars.characterCreate();

		yars.characterLevelIncrease(character_id, 10);

		Yars::Health& health = yars.characterHealth(character_id);
		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 0);
	}
}


TEST_CASE("Character.Level.Increase.Skill", "[character][level][skill]")
{
	Yars yars;

	yars.skillAdd(Skill_Lockpicking, lockpicking);

	SECTION("Invalid Character")
	{
		bool success = yars.characterLevelIncrease(1234, Skill_Lockpicking, 8);
		REQUIRE(success == false);
	}

	SECTION("Level Up")
	{
		bool success = false;

		Yars::Id character_id = yars.characterCreate();

		success = yars.characterLevelIncrease(character_id, "", 8);
		REQUIRE(success == true);

		success = yars.characterLevelIncrease(character_id, Skill_Lockpicking, 8);
		REQUIRE(success == false);

		yars.characterSkillAdd(character_id, Skill_Lockpicking);
		success = yars.characterLevelIncrease(character_id, Skill_Lockpicking, 8);
		REQUIRE(success == true);

		auto& attr = yars.characterAttributeMap(character_id);
		REQUIRE(attr[Attr_Hearing].level.value == 3);
		REQUIRE(attr[Attr_Vision].level.value  == 2);
	}
}


TEST_CASE("Character.Level.Set", "[character][level][skill]")
{
	Yars yars;

	SECTION("Level Set")
	{
		yars.skillAdd(Skill_Lockpicking, lockpicking);

		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Lockpicking);
		yars.characterLevelIncrease(character_id, Skill_Lockpicking, 16);

		yars.characterLevelSet(character_id, Skill_Lockpicking, 8);

		auto& attr = yars.characterAttributeMap(character_id);
		REQUIRE(attr[Attr_Hearing].level.value == 3);
		REQUIRE(attr[Attr_Vision].level.value  == 2);

		bool success = false;
		success = yars.characterLevelSet(character_id, "", 32);
		REQUIRE(success == true);

		success = yars.characterLevelSet(character_id, "invalid skill", 32);
		REQUIRE(success == false);
	}
}


TEST_CASE("Character.Affect.Apply", "[character][level][skill]")
{
	Yars yars;

	SECTION("Bad Character Id")
	{
		REQUIRE(yars.characterAffectApply(0, 0) == false);
	}

	// Use the first character id
	yars.characterCreate();

	SECTION("Bad Affect Id")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterAffectApply(character_id, 0) == false);
	}

	Yars::Affect affect_data =
	{	.level                 = 0
	,	.origin_character      = 0
	,	.origin_skill          = ""
	,	.experience            = 10.0
	,	.modify_health_current = 10
	,	.modify_health_maximum = 2
	,	.modify_skill =
		{	{ "s1", +1 }
		,	{ "s2", +2 }
		,	{ "s3", +3 }
		}
	,	.modify_attribute =
		{	{ "a1", +4 }
		,	{ "a2", +5 }
		}
	,	.defense_skill = { }
	};

	Yars::Id affect_id = yars.affectCreate();
	yars.affect(affect_id) = affect_data;

	yars.skillAdd("s1", { .attribute_contribution = {{"a1", 100}}});
	yars.skillAdd("s2", { .attribute_contribution = {{"a2", 100}}});
	yars.skillAdd("s3", { .attribute_contribution = {{"a1", 50},{"a2", 50}}});

	SECTION("Character with no skills")
	{
		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, "s1");
		yars.healthSet(yars.characterHealth(character_id), 5, 10);

//std::cout << "health: " << Yars::to_string(yars.characterHealth(character_id)) << "\n";
//std::cout << "skill: " << Yars::to_string(yars.characterSkillMap(character_id)) << "\n";
//std::cout << "attribute: " << Yars::to_string(yars.characterAttributeMap(character_id)) << "\n";

		REQUIRE(yars.characterAffectApply(character_id, affect_id) == true);

		// Can't apply twice
		REQUIRE(yars.characterAffectApply(character_id, affect_id) == false);

		Yars::Health& health = yars.characterHealth(character_id);
		REQUIRE(health.current == 12);

		REQUIRE(yars.characterSkillExists(character_id, "s1") == true);
		{
			Yars::Skill& skill = yars.characterSkill(character_id, "s1");
			REQUIRE(skill.level.modifier == 1);
			REQUIRE(skill.attribute_contribution["a1"] == 100);
		}

		REQUIRE(yars.characterSkillExists(character_id, "s2") == true);
		{
			Yars::Skill& skill = yars.characterSkill(character_id, "s2");
			REQUIRE(skill.level.modifier == 2);
			REQUIRE(skill.attribute_contribution["a2"] == 100);
		}

		REQUIRE(yars.characterSkillExists(character_id, "s3") == true);
		{
			Yars::Skill& skill = yars.characterSkill(character_id, "s3");
			REQUIRE(skill.level.modifier == 3);
			REQUIRE(skill.attribute_contribution["a1"] == 50);
			REQUIRE(skill.attribute_contribution["a2"] == 50);
		}

		REQUIRE(yars.characterAttributeExists(character_id, "a1") == true);
		{
			Yars::Attribute& attribute = yars.characterAttribute(character_id, "a1");
			REQUIRE(attribute.level.modifier == 4);
		}

		REQUIRE(yars.characterAttributeExists(character_id, "a2") == true);
		{
			Yars::Attribute& attribute = yars.characterAttribute(character_id, "a2");
			REQUIRE(attribute.level.modifier == 5);
		}
	}
}


TEST_CASE("Character.Affect.Defend", "[character][level][skill]")
{
	Yars yars;

	SECTION("Bad Character Id")
	{
		REQUIRE(yars.characterAffectDefend(0, 0, "") == false);
	}

	SECTION("Bad Affect Id")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterAffectDefend(character_id, 0, "") == false);
	}

	SECTION("Bad Skill")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterAffectDefend(character_id, affect_id, "") == false);
	}

	yars.skillAdd(Skill_Acting, { .attribute_contribution = {{"Projecting", 100}}});
	yars.skillAdd(Skill_Mentalist, mentalist);

	Yars::Affect affect_data =
	{	.level                 = 10
	,	.origin_character      = 0
	,	.origin_skill          = ""
	,	.experience            = 0
	,	.modify_health_current = -10
	,	.modify_health_maximum = -2
	,	.modify_skill     =
		{	{ "s1", +1 }
		,	{ "s2", +2 }
		,	{ "s3", +3 }
		}
	,	.modify_attribute =
		{	{ "a1", +4 }
		,	{ "a2", +5 }
		}
	,	.defense_skill      =
		{	Skill_Acting
		}
	};

	Yars::Id affect_id = yars.affectCreate();
	yars.affect(affect_id) = affect_data;

	yars.skillAdd("s1", { .attribute_contribution = {{"a1", 100}}});
	yars.skillAdd("s2", { .attribute_contribution = {{"a2", 100}}});
	yars.skillAdd("s3", { .attribute_contribution = {{"a1", 50},{"a2", 50}}});

	SECTION("Invalid Skill")
	{
		Yars::Id affect_id    = yars.affectCreate();
		Yars::Id character_id = yars.characterCreate();

		yars.characterSkillAdd(character_id, Skill_Mentalist);

		REQUIRE(yars.characterAffectDefend(character_id
			, affect_id
			, Skill_Mentalist
			) == true);
	}

	SECTION("Skill Check: Auto Fail")
	{
		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Acting);

		yars.levelSet(yars.characterSkill(character_id, Skill_Acting).level, 1);

		// A level 1 acting skill can not defend against a level 10 affect
		REQUIRE(yars.characterAffectDefend(character_id
			, affect_id
			, Skill_Acting
			) == true
			);
	}

	SECTION("Skill Check: Auto Success")
	{
		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, Skill_Acting);

		yars.levelSet(yars.characterSkill(character_id, Skill_Acting).level, 100);

		// A level 100 acting skill can will defend against a level 10 affect
		REQUIRE(yars.characterAffectDefend(character_id
			, affect_id
			, Skill_Acting
			) == false
			);
	}
}


TEST_CASE("Character.Affect.Exists", "[affect][character]")
{
	Yars yars;

	SECTION("Invalid Character")
	{
		REQUIRE(yars.characterAffectExists(1234, 5678) == false);
	}

	SECTION("Does not exist")
	{
		Yars::Id character_id = yars.characterCreate();

		REQUIRE(yars.characterAffectExists(character_id, 1234) == false);
	}
}


TEST_CASE("Character.Affect.List", "[affect][character]")
{
	Yars yars;

	Yars::VectorId affect_list;

	SECTION("Invalid Character")
	{
		affect_list = yars.characterAffectList(1234);
		REQUIRE(affect_list.empty() == true);
	}

	SECTION("Empty List")
	{
		Yars::Id character_id = yars.characterCreate();

		affect_list = yars.characterAffectList(character_id);
		REQUIRE(affect_list.empty() == true);
	}

	SECTION("List")
	{
		Yars::Id character_id = yars.characterCreate();
		Yars::Id affect_id    = yars.affectCreate();

		bool success = false;

		success = yars.characterAffectApply(character_id, affect_id);
		REQUIRE(success == true);

		affect_list = yars.characterAffectList(character_id);
		REQUIRE(Yars::vectorContains(affect_list, affect_id) == true);
	}
}


TEST_CASE("Character.Affect.Remove", "[affect][character]")
{
	Yars yars;

	Yars::VectorId affect_list;
	Yars::Id character_id = yars.characterCreate();
	Yars::Id affect_id    = yars.affectCreate();

	SECTION("Invalid")
	{
		yars.characterAffectRemove(character_id ^ 1234, affect_id);
		yars.characterAffectRemove(character_id, affect_id ^ 1234);
	}

	SECTION("Valid")
	{
		yars.characterAffectApply(character_id, affect_id);
		REQUIRE(yars.characterAffectExists(character_id, affect_id) == true);

		yars.characterAffectRemove(character_id, affect_id);
		REQUIRE(yars.characterAffectExists(character_id, affect_id) == false);
	}
}


TEST_CASE("Character.Affect.Revert", "[affect][character]")
{
	Yars yars;

	const std::string Attr_Name = "attribute";

	const std::string Skill_A = "skill_a";
	yars.skillAdd(Skill_A,
	{	.attribute_contribution =
		{	{ Attr_Name, 10 }
		}
	});
	yars.levelSet(yars.skill(Skill_A).level, 0);

	const std::string Skill_B = "skill_b";
	yars.skillAdd(Skill_B,
	{	.attribute_contribution =
		{	{ Attr_Name, 10 }
		}
	});
	yars.levelSet(yars.skill(Skill_B).level, 0);

	const std::string Item_Name = "item";
	Yars::Item item =
	{	.affect_attribute =
		{	{ Attr_Name, 4 }
		}
	,	.affect_skill =
		{	{ Skill_A,  8 }
		,	{ Skill_B, 16 }
		}
	,	.skill_attacker =
		{	Skill_A
		}
	};
	yars.itemAdd(Item_Name, item);

	Yars::Id character_id = yars.characterCreate();
	yars.characterSkillAdd(character_id, Skill_A);
	yars.levelSet(yars.characterSkill(character_id, Skill_A).level, 100);

	Yars::Id affect_id = yars.affectCreate();
	yars.affectInit(affect_id
		, character_id
		, item
		, Skill_A
		, 100
		);

	SECTION("Invalid")
	{
		yars.characterAffectRevert(character_id ^ 1234, affect_id);
		yars.characterAffectRevert(character_id, affect_id ^ 1234);
	}

	SECTION("Valid 1")
	{
		yars.characterAffectApply(character_id, affect_id);
		yars.characterAffectRevert(character_id, affect_id);
	}

	SECTION("Valid 1")
	{
		yars.characterAffectApply(character_id, affect_id);
		yars.characterSkillRemove(character_id, Skill_B);
		yars.characterAffectRevert(character_id, affect_id);
	}
}

