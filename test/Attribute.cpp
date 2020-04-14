/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Attribute Attribute.cpp && ./Attribute
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

TEST_CASE("Attribute.Init", "[attribute]")
{
	Yars::Attribute attribute;

	REQUIRE(attribute.level.value == 0);
	REQUIRE(attribute.level.partial == 0);
}


TEST_CASE("Attribute.Add", "[attribute]")
{
	Yars yars;

	SECTION("Does not exist")
	{
		REQUIRE(yars.attributeExists("") == false);
	}

	SECTION("Add invalid attribute")
	{
		REQUIRE(yars.attributeAdd("") == false);
		REQUIRE(yars.attributeExists("") == false);
	}

	SECTION("Add valid attribute")
	{
		REQUIRE(yars.attributeExists("A") == false);
		REQUIRE(yars.attributeAdd("A") == true);
		REQUIRE(yars.attributeExists("A") == true);
	}

	SECTION("Add valid attributes")
	{
		REQUIRE(yars.attributeAdd("A") == true);
		REQUIRE(yars.attributeExists("A") == true);

		REQUIRE(yars.attributeAdd("AA") == true);
		REQUIRE(yars.attributeExists("AA") == true);

		REQUIRE(yars.attributeAdd("B") == true);
		REQUIRE(yars.attributeExists("B") == true);
	}

	SECTION("Don't add duplicate attributes")
	{
		yars.attributeAdd("A");

		REQUIRE(yars.attributeAdd("A") == false);
		REQUIRE(yars.attributeExists("A") == true);
		//std::cout << Yars::to_string(yars.attribute("A")) << std::endl;
	}
}


TEST_CASE("Attribute.Get", "[attribute]")
{
	Yars yars;

	yars.attributeAdd("A");

	SECTION("Non-Const")
	{
		Yars::Attribute& attribute = yars.attribute("A");

		REQUIRE(attribute.level.value == 0);


		Yars::Attribute& bad_name = yars.attribute("");
		REQUIRE(&bad_name == nullptr);

		Yars::Attribute& does_not_exist = yars.attribute("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}

	SECTION("Const")
	{
		const Yars& const_yars = yars;

		const Yars::Attribute& attribute = const_yars.attribute("A");

		REQUIRE(attribute.level.value == 0);


		const Yars::Attribute& bad_name = const_yars.attribute("");
		REQUIRE(&bad_name == nullptr);

		const Yars::Attribute& does_not_exist = const_yars.attribute("Does Not Exist");
		REQUIRE(&does_not_exist == nullptr);
	}
}


TEST_CASE("Attribute.Level", "[attribute][level]")
{
	using namespace zakero;

	Yars yars;

	SECTION("Attribute: Level")
	{
		yars.attributeAdd("A");
		yars.levelSet(yars.attribute("A").level, 5);

		REQUIRE(yars.attribute("A").level.value == 5);
	}
}
