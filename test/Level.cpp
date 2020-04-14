/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Level Level.cpp && ./Level
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

TEST_CASE("Level.Init", "[level]")
{
	Yars::Level level;

	REQUIRE(level.value == 0);
}

TEST_CASE("Level.Set", "[level]")
{
	Yars::Level level;

	SECTION("Value")
	{
		Yars::levelSet(level, 1);
		REQUIRE(level.value == 1);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("Value and Partial")
	{
		Yars::levelSet(level, 2, 0.5);
		REQUIRE(level.value == 2);
		REQUIRE(level.partial == Approx(0.5f));
	}

	SECTION("Value and Partial, with increase")
	{
		Yars::levelSet(level, 3, 5.5);
		REQUIRE(level.value == 4);
		REQUIRE(level.partial == Approx(2.5f));

		//std::cout << Yars::to_string(level) << std::endl;
	}

	SECTION("Set and Set")
	{
		Yars::levelSet(level, 3, 5.5);
		REQUIRE(level.value == 4);
		REQUIRE(level.partial == Approx(2.5f));

		Yars::levelSet(level, 1, 0);
		REQUIRE(level.value == 1);
		REQUIRE(level.partial == Approx(0.0f));
		//std::cout << Yars::to_string(level) << std::endl;
	}

	SECTION("Increase then Set")
	{
		Yars::levelIncrease(level, 0.5);
		REQUIRE(level.value == 0);
		REQUIRE(level.partial == Approx(0.5f));

		Yars::levelSet(level, 3, 5.5);
		REQUIRE(level.value == 4);
		REQUIRE(level.partial == Approx(2.5f));

		//std::cout << Yars::to_string(level) << std::endl;
	}
}

TEST_CASE("Level.Increase", "[level]")
{
	Yars::Level level;

	SECTION("Partial level")
	{
		Yars::levelIncrease(level, 0.5);
		REQUIRE(level.value == 0);
		REQUIRE(level.partial == Approx(0.5f));
	}

	SECTION("1 full level, exact")
	{
		Yars::levelIncrease(level, 1);
		REQUIRE(level.value == 1);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("1 full level, with partial")
	{
		Yars::levelIncrease(level, 1.5);
		REQUIRE(level.value == 1);
		REQUIRE(level.partial == Approx(0.5f));
	}

	SECTION("2 levels")
	{
		Yars::levelIncrease(level, 2.0);
		REQUIRE(level.value == 2);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("3 levels, with partial")
	{
		Yars::levelIncrease(level, 5.5);
		REQUIRE(level.value == 3);
		REQUIRE(level.partial == Approx(1.5f));
	}

	SECTION("Accumulate")
	{
		Yars::levelIncrease(level, 1.0);
		Yars::levelIncrease(level, 2.8);
		Yars::levelIncrease(level, 3.6);
		REQUIRE(level.value == 4);
		REQUIRE(level.partial == Approx(0.4f));
	}

	SECTION("Negative Level, not allowed")
	{
		Yars::levelIncrease(level, -1.0);
		REQUIRE(level.value == 0);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("Negative Increase, not allowed")
	{
		Yars::levelIncrease(level, 4.0);
		Yars::levelIncrease(level, -1.0);
		REQUIRE(level.value == 3);
		REQUIRE(level.partial == Approx(0.0f));
	}
}

TEST_CASE("Level.Modifier", "[level]")
{
	Yars::Level level;

	SECTION("Modifier Ignored for Level 0")
	{
		Yars::levelModifierAdjust(level, 1);
		REQUIRE(level.value == 0);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("Modifier increases Level")
	{
		Yars::levelSet(level, 1);
		Yars::levelModifierAdjust(level, 1);
		REQUIRE(level.value == 2);
		REQUIRE(level.partial == Approx(0.0f));
	}

	SECTION("Modifier does not change partial")
	{
		Yars::levelSet(level, 3, 0.1);
		Yars::levelModifierAdjust(level, 1);
		REQUIRE(level.value == 4);
		REQUIRE(level.partial == Approx(0.1f));
	}

	SECTION("Modifier does not effect leveling")
	{
		Yars::levelSet(level, 3, 0.1);
		Yars::levelModifierAdjust(level, 1);
		Yars::levelIncrease(level, 3.1);
		REQUIRE(level.value == 5);
		REQUIRE(level.partial == Approx(0.2f));
	}

	SECTION("Modifiers stack")
	{
		Yars::levelSet(level, 5);
		Yars::levelModifierAdjust(level, 1);
		REQUIRE(level.value == 6);

		Yars::levelModifierAdjust(level, 2);
		REQUIRE(level.value == 8);

		Yars::levelModifierAdjust(level, -1);
		REQUIRE(level.value == 7);
	}

	SECTION("Modifiers can't make Levels negative")
	{
		Yars::levelModifierAdjust(level, -1); // Lvl: 0, Mod: -1
		REQUIRE(level.value == 0);

		Yars::levelIncrease(level, 2.0f);     // Lvl: 1, Mod: -1
		REQUIRE(level.value == 1);

		Yars::levelModifierAdjust(level, -2); // Lvl: 1, Mod: -3
		REQUIRE(level.value == 0);
	}
}
