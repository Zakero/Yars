/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Skill Skill.cpp && ./Skill
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
	const std::string Skill_Mentalist = "Mentalist";
}


TEST_CASE("Skill.Init", "[skill]")
{
	Yars::Skill mentalist;

	REQUIRE(mentalist.attribute_contribution.empty() == true);
	REQUIRE(mentalist.level.value == 0);
}


TEST_CASE("Skill.Add", "[skill]")
{
	Yars yars;

	Yars::Skill mentalist =
	{	.attribute_contribution =
		{	{	Attr_Vision, 1
			}
		}
	};

	SECTION("Does not exist")
	{
		REQUIRE(yars.skillExists("") == false);
		REQUIRE(yars.skillExists(Skill_Mentalist) == false);
	}

	SECTION("Add invalid Skill")
	{
		REQUIRE(yars.skillAdd("", mentalist) == false);
		REQUIRE(yars.skillExists("") == false);
	}

	SECTION("Add valid Skill")
	{
		REQUIRE(yars.skillAdd(Skill_Mentalist, mentalist) == true);
		REQUIRE(yars.skillExists(Skill_Mentalist) == true);
		REQUIRE(yars.skillAdd(Skill_Mentalist, mentalist) == false);
		REQUIRE(yars.skill(Skill_Mentalist).attribute_contribution[Attr_Vision] == 1);

		//std::cout << Yars::to_string(yars.skill(Skill_Mentalist)) << std::endl;
	}

	SECTION("Don't allow duplicate Skills")
	{
		yars.skillAdd(Skill_Mentalist, mentalist);
		
		REQUIRE(yars.skillAdd(Skill_Mentalist, mentalist) == false);
		REQUIRE(yars.skillExists(Skill_Mentalist) == true);
	}
}



TEST_CASE("Skill.Get", "[skill]")
{
	Yars yars;

	yars.skillAdd("A", {});

	SECTION("Non-Const")
	{
		Yars::Skill& skill = yars.skill("A");

		REQUIRE(skill.level.value == 0);


		Yars::Skill& bad_name = yars.skill("");
		REQUIRE(&bad_name == nullptr);

		Yars::Skill& does_not_exist = yars.skill("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::Skill& skill = const_yars.skill("A");

		REQUIRE(skill.level.value == 0);


		const Yars::Skill& bad_name = const_yars.skill("");
		REQUIRE(&bad_name == nullptr);

		const Yars::Skill& does_not_exist = const_yars.skill("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}
}


TEST_CASE("Skill.Remove", "[skill]")
{
	Yars yars;

	Yars::Skill mentalist =
	{	.attribute_contribution =
		{	{	Attr_Vision, 1
			}
		}
	};

	SECTION("Remove invalid Skill")
	{
		REQUIRE(yars.skillRemove("") == false);
		REQUIRE(yars.skillRemove(Attr_Vision) == false);
		REQUIRE(yars.skillRemove(Skill_Mentalist) == false);
	}

	SECTION("Remove valid Skill")
	{
		yars.skillAdd(Skill_Mentalist, mentalist);

		REQUIRE(yars.skillRemove(Skill_Mentalist) == true);
		REQUIRE(yars.skillExists(Skill_Mentalist) == false);
	}
}


TEST_CASE("Skill.Level", "[level][skill]")
{
	Yars::Skill mentalist =
	{	.attribute_contribution =
		{	{	Attr_Vision, 1
			}
		}
	};

	Yars::levelIncrease(mentalist.level, 3.0f);

	REQUIRE(mentalist.attribute_contribution.empty() == false);
	REQUIRE(mentalist.attribute_contribution[Attr_Vision] == 1);
	REQUIRE(mentalist.level.value == 2);
}


TEST_CASE("Skill.List", "[skill]")
{
	Yars yars;

	Yars::VectorString skill_list;

	skill_list = yars.skillList();

	REQUIRE(skill_list.empty() == true);

	yars.skillAdd("A", {});
	yars.skillAdd("B", {});
	yars.skillAdd("C", {});

	skill_list = yars.skillList();

	REQUIRE(skill_list.size() == 3);
	REQUIRE(Yars::vectorContains<std::string>(skill_list, "A") == true);
	REQUIRE(Yars::vectorContains<std::string>(skill_list, "B") == true);
	REQUIRE(Yars::vectorContains<std::string>(skill_list, "C") == true);
}
