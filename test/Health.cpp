/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Health Health.cpp && ./Health
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


TEST_CASE("Health.Init", "[health]")
{
	Yars::Health health;

	REQUIRE(health.current == 0);
	REQUIRE(health.maximum == 0);
	REQUIRE(health.maximum_base == 0);
	REQUIRE(health.maximum_modifier == 0);
}


TEST_CASE("Health.Set", "[health]")
{
	Yars::Health health;

	SECTION("Good values 1")
	{
		Yars::healthSet(health, 0, 0);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Good values 2")
	{
		Yars::healthSet(health, 0, 5);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 5);
		REQUIRE(health.maximum_base == 5);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Good values 3")
	{
		Yars::healthSet(health, 3, 5);

		REQUIRE(health.current == 3);
		REQUIRE(health.maximum == 5);
		REQUIRE(health.maximum_base == 5);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Good values 4")
	{
		Yars::healthSet(health, 5, 5);

		REQUIRE(health.current == 5);
		REQUIRE(health.maximum == 5);
		REQUIRE(health.maximum_base == 5);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Good values 5")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Bad values 1")
	{
		Yars::healthSet(health, Yars::Health_Max, 0);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Bad values 2")
	{
		Yars::healthSet(health, Yars::Health_Max, 10);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Bad values 3")
	{
		uint32_t bad_value = Yars::Health_Max;
		bad_value *= 2;
		Yars::healthSet(health, bad_value, bad_value);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}
}


TEST_CASE("Health.Modifier.Set", "[health]")
{
	Yars::Health health;

	SECTION("Modifiers with 0 health - 1")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 0);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Modifiers with 0 health - 2")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 10);

		REQUIRE(health.current ==  0);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 10);
	}

	SECTION("Modifiers with 0 health - 3")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, -10);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == -10);
	}

	SECTION("Modifiers with 0 health - 4")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, Yars::Health_Max);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Modifiers with 0 health - 5")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == Yars::Health_Min);
	}

	SECTION("Modifiers with health - 1")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthModifierSet(health, 0);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Modifiers with health - 2")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthModifierSet(health, Yars::Health_Max);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Modifiers with health - 3")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthModifierSet(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == Yars::Health_Min);
	}

	SECTION("Modifiers with health - 4")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthModifierSet(health, 10);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 20);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 10);
	}

	SECTION("Modifiers with health - 5")
	{
		Yars::healthSet(health, 20, 20);
		Yars::healthModifierSet(health, -10);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 20);
		REQUIRE(health.maximum_modifier == -10);
	}

	SECTION("Modifiers with health - 6")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthModifierSet(health, 10);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 10);
	}

	SECTION("Modifiers with health - 7")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthModifierSet(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Modifiers with health - 8")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthModifierSet(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == Yars::Health_Min);
	}
}


TEST_CASE("Health.Adjust", "[health]")
{
	Yars::Health health;

	SECTION("Increase 0 Max of 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthAdjust(health, 0);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase 0 with Max of Max")
	{
		Yars::healthSet(health, 0, Yars::Health_Max);
		Yars::healthAdjust(health, 0);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase to Max")
	{
		Yars::healthSet(health, 0, Yars::Health_Max);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase beyond Max")
	{
		Yars::healthSet(health, 10, Yars::Health_Max);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase too much")
	{
		Yars::healthSet(health, 0, 10);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase a bit")
	{
		Yars::healthSet(health, 0, 20);
		Yars::healthAdjust(health, 15);

		REQUIRE(health.current == 15);
		REQUIRE(health.maximum == 20);
		REQUIRE(health.maximum_base == 20);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease from Max of 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthAdjust(health, -1);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base ==  0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease from Max of Max")
	{
		Yars::healthSet(health, 0, Yars::Health_Max);
		Yars::healthAdjust(health, -1);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base ==  Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease from Max 1")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthAdjust(health, -1);

		REQUIRE(health.current == (Yars::Health_Max - 1));
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease Good")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthAdjust(health, -6);

		REQUIRE(health.current == 4);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base ==  10);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease too much")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthAdjust(health, -9999);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 0);
	}
}


TEST_CASE("Health.Maximum.Adjust", "[health]")
{
	Yars::Health health;

	SECTION("Increase a bit")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthMaximumAdjust(health, 10);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == 20);
		REQUIRE(health.maximum_base == 20);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Max Increase from 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthMaximumAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Max Increase too much")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthMaximumAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 10);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Decrease a bit")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthMaximumAdjust(health, -5);

		REQUIRE(health.current == 5);
		REQUIRE(health.maximum == 5);
		REQUIRE(health.maximum_base == 5);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Max Decrease from Max")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthMaximumAdjust(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Max Decrease too much")
	{
		Yars::healthSet(health, 10, 10);
		Yars::healthMaximumAdjust(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 0);
	}
}


TEST_CASE("Health.Modifier.Adjust", "[health]")
{
	Yars::Health health;

	SECTION("Positive Adjustment with Max 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 0);
		Yars::healthModifierAdjust(health, 10);

		REQUIRE(health.current ==  0);
		REQUIRE(health.maximum == 10);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 10);
	}

	SECTION("Positive Adjustment")
	{
		Yars::healthSet(health, 0, 10);
		Yars::healthModifierSet(health, 0);
		Yars::healthModifierAdjust(health, 10);

		REQUIRE(health.current ==  0);
		REQUIRE(health.maximum == 20);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 10);
	}

	SECTION("Positive Adjustment too much with Max 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 10);
		Yars::healthModifierAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Positive Adjustment too much")
	{
		Yars::healthSet(health, 0, Yars::Health_Max);
		Yars::healthModifierSet(health, 10);
		Yars::healthModifierAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Negative Adjustment")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 0);
		Yars::healthModifierAdjust(health, -10);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == -10);
	}

	SECTION("Negative Adjustment too much with Max 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, -10);
		Yars::healthModifierAdjust(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == Yars::Health_Min);
	}
}


TEST_CASE("Health.combine.Adjust.with.Modifier.Set", "[health]")
{
	Yars::Health health;

	SECTION("Positive Adjustment: Increase a bit with base max of 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 100);
		Yars::healthAdjust(health, 50);

		REQUIRE(health.current ==  50);
		REQUIRE(health.maximum == 100);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase a bit")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierSet(health, 100);
		Yars::healthAdjust(health, 150);

		REQUIRE(health.current == 150);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase too much")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierSet(health, 200);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 300);
		REQUIRE(health.maximum == 300);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 200);
	}

	SECTION("Negative Adjustment: Decrease from Max of 0")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierSet(health, 100);
		Yars::healthAdjust(health, -10);

		REQUIRE(health.current ==   0);
		REQUIRE(health.maximum == 100);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Negative Adjustment: Decrease from Max")
	{
		Yars::healthModifierSet(health, 100);
		Yars::healthSet(health, 200, 10);
		Yars::healthAdjust(health, -20);

		REQUIRE(health.current ==  90);
		REQUIRE(health.maximum == 110);
		REQUIRE(health.maximum_base == 10);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase beyond Max")
	{
		Yars::healthSet(health, 0, 300);
		Yars::healthModifierSet(health, -100);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 200);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 300);
		REQUIRE(health.maximum_modifier == -100);
	}
}


TEST_CASE("Health.combine.Adjust.with.Modifier.Adjust", "[health]")
{
	Yars::Health health;

	SECTION("Positive Adjustment: Increase to Max")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health, 100);

		REQUIRE(health.current == 100);
		REQUIRE(health.maximum == 100);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase beyond Max")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 100);
		REQUIRE(health.maximum == 100);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase to Max 2")
	{
		Yars::healthSet(health, 0, 0);
		Yars::healthModifierAdjust(health, Yars::Health_Max);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == 0);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Positive Adjustment: Increase to Max 3")
	{
		Yars::healthSet(health, 0, Yars::Health_Max);
		Yars::healthModifierAdjust(health, Yars::Health_Max);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Positive Adjustment: Increase to Max 4")
	{
		Yars::healthSet(health, Yars::Health_Max, Yars::Health_Max);
		Yars::healthModifierAdjust(health, Yars::Health_Max);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == Yars::Health_Max);
		REQUIRE(health.maximum == Yars::Health_Max);
		REQUIRE(health.maximum_base == Yars::Health_Max);
		REQUIRE(health.maximum_modifier == Yars::Health_Max);
	}

	SECTION("Positive Adjustment: Increase a bit")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health, 150);

		REQUIRE(health.current == 150);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase a bit again")
	{
		Yars::healthModifierAdjust(health, 100);
		Yars::healthSet(health, 0, 100);
		Yars::healthAdjust(health, 150);

		REQUIRE(health.current == 150);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase a bit more")
	{
		Yars::healthSet(health, 0, 300);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health, 350);

		REQUIRE(health.current == 350);
		REQUIRE(health.maximum == 400);
		REQUIRE(health.maximum_base == 300);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Negative Adjustment: Decrease from 0 with Max")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health, -10);

		REQUIRE(health.current ==   0);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Negative Adjustment: Decrease from Max")
	{
		Yars::healthSet(health, 100, 100);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthAdjust(health,   50);
		Yars::healthAdjust(health, -100);

		REQUIRE(health.current ==  50);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Positive Adjustment: Increase beyond Max")
	{
		Yars::healthSet(health, 0, 300);
		Yars::healthModifierAdjust(health, -100);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 200);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 300);
		REQUIRE(health.maximum_modifier == -100);
	}

	SECTION("Positive Adjustment: Increase but still negative")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, -200);
		Yars::healthAdjust(health, Yars::Health_Max);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == -200);
	}

	SECTION("Negative Adjustment: Decrease to Min")
	{
		Yars::healthSet(health, -100, 200);
		Yars::healthModifierAdjust(health, Yars::Health_Min);
		Yars::healthAdjust(health, Yars::Health_Min);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 200);
		REQUIRE(health.maximum_modifier == Yars::Health_Min);
	}
}


TEST_CASE("Health.IncreaseToMaximum", "[health]")
{
	Yars::Health health;

	SECTION("Increase to Maxumum 1")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthIncreaseToMaximum(health);

		REQUIRE(health.current == 100);
		REQUIRE(health.maximum == 100);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 0);
	}

	SECTION("Increase to Maxumum 2")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierSet(health, 100);
		Yars::healthIncreaseToMaximum(health);

		REQUIRE(health.current == 200);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Increase to Maxumum 3")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, 100);
		Yars::healthIncreaseToMaximum(health);

		REQUIRE(health.current == 200);
		REQUIRE(health.maximum == 200);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == 100);
	}

	SECTION("Increase to Maxumum 4")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, -50);
		Yars::healthIncreaseToMaximum(health);

		REQUIRE(health.current == 50);
		REQUIRE(health.maximum == 50);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == -50);
	}

	SECTION("Increase to Maxumum 5")
	{
		Yars::healthSet(health, 0, 100);
		Yars::healthModifierAdjust(health, -100);
		Yars::healthIncreaseToMaximum(health);

		REQUIRE(health.current == 0);
		REQUIRE(health.maximum == 0);
		REQUIRE(health.maximum_base == 100);
		REQUIRE(health.maximum_modifier == -100);
	}
}

