/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Calculate Calculate.cpp && ./Calculate
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


TEST_CASE("Calculate.Difficulty", "[calculate][skill]")
{
	Yars yars;

	SECTION("Base and Target")
	{
		REQUIRE(std::isnan(yars.calculateDifficulty(  0, 0)));
		REQUIRE(std::isinf(yars.calculateDifficulty(100, 0)));

		REQUIRE(yars.calculateDifficulty(200, 100) == Approx(-2.0));
		REQUIRE(yars.calculateDifficulty(150, 100) == Approx(-1.0));
		REQUIRE(yars.calculateDifficulty(125, 100) == Approx(-0.5));
		REQUIRE(yars.calculateDifficulty(100, 100) == Approx( 0.0));
		REQUIRE(yars.calculateDifficulty( 75, 100) == Approx( 0.5));
		REQUIRE(yars.calculateDifficulty( 50, 100) == Approx( 1.0));
		REQUIRE(yars.calculateDifficulty(  0, 100) == Approx( 2.0));
	}

	SECTION("Skill vs Target")
	{
		Yars::Skill skill;

		Yars::levelSet(skill.level, 0);
		REQUIRE(std::isnan(yars.calculateDifficulty(skill, 0)));

		Yars::levelSet(skill.level, 100);
		REQUIRE(std::isinf(yars.calculateDifficulty(skill, 0)));

		Yars::levelSet(skill.level, 200);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx(-2.0));

		Yars::levelSet(skill.level, 150);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx(-1.0));

		Yars::levelSet(skill.level, 125);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx(-0.5));

		Yars::levelSet(skill.level, 100);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx( 0.0));

		Yars::levelSet(skill.level,  75);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx( 0.5));

		Yars::levelSet(skill.level,  50);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx( 1.0));

		Yars::levelSet(skill.level,   0);
		REQUIRE(yars.calculateDifficulty(skill, 100) == Approx( 2.0));
	}

	SECTION("Skill vs Skill")
	{
		Yars::Skill attacker;
		Yars::Skill defender;

		Yars::levelSet(attacker.level, 0);
		Yars::levelSet(defender.level, 0);
		REQUIRE(std::isnan(yars.calculateDifficulty(attacker, defender)));

		Yars::levelSet(attacker.level, 100);
		Yars::levelSet(defender.level,   0);
		REQUIRE(std::isinf(yars.calculateDifficulty(attacker, defender)));

		Yars::levelSet(attacker.level, 200);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx(-2.0));

		Yars::levelSet(attacker.level, 150);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx(-1.0));

		Yars::levelSet(attacker.level, 125);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx(-0.5));

		Yars::levelSet(attacker.level, 100);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx( 0.0));

		Yars::levelSet(attacker.level,  75);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx( 0.5));

		Yars::levelSet(attacker.level,  50);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx( 1.0));

		Yars::levelSet(attacker.level,   0);
		Yars::levelSet(defender.level, 100);
		REQUIRE(yars.calculateDifficulty(attacker, defender) == Approx( 2.0));
	}

	SECTION("Is Success")
	{
		const std::string SKILL_NAME = "skill";
		yars.skillAdd(SKILL_NAME, {});

		Yars::Id character_id = yars.characterCreate();
		yars.characterSkillAdd(character_id, SKILL_NAME);
		yars.levelSet(yars.characterSkill(character_id, SKILL_NAME).level, 10);

		bool success = false;

		success = yars.calculateIsSuccess(character_id
			, SKILL_NAME
			, -8
			);
		REQUIRE(success == true);

		success = yars.calculateIsSuccess(character_id
			, SKILL_NAME
			, 8
			);
		REQUIRE(success == false);
	}
}


TEST_CASE("Calculate.Modifier", "[calculate][character][skill]")
{
	Yars yars;

	SECTION("Invalid Character")
	{
		float modifier = yars.calculateModifier(1234, "skill");
		REQUIRE(modifier == Approx(0));
	}

	SECTION("Invalid Skill")
	{
		Yars::Id character_id = yars.characterCreate();

		float modifier = yars.calculateModifier(character_id, "skill");
		REQUIRE(modifier == Approx(0));
	}

	Yars::Id character_id = yars.characterCreate();

	SECTION("Skill Level 0")
	{
		Yars::Skill skill;
		yars.skillAdd("skill", skill);

		yars.characterSkillAdd(character_id, "skill");

		REQUIRE(yars.calculateModifier(character_id, "skill") == Approx(0));
	}

	SECTION("No Attributes")
	{
		Yars::Skill skill;
		yars.skillAdd("skill", skill);

		yars.characterSkillAdd(character_id, "skill");
		yars.characterSkillIncrease(character_id, "skill", 1.0f);

		REQUIRE(yars.calculateModifier(character_id, "skill") == Approx(0));
	}

	SECTION("Attributes")
	{
		Yars::Skill skill =
		{	.attribute_contribution =
			{	{ "A", 1 }
			,	{ "B", 2 }
			,	{ "C", 3 }
			}
		};
		yars.skillAdd("skill", skill);

		yars.characterSkillAdd(character_id, "skill");
		yars.characterSkillIncrease(character_id, "skill", 1.0f);

		Yars::levelSet(yars.characterAttribute(character_id, "A").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "B").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "C").level, 12, 0);

		REQUIRE(yars.calculateModifier(character_id, "skill") == Approx(12));

		Yars::levelSet(yars.characterAttribute(character_id, "A").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "B").level,  6, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "C").level,  3, 0);

		REQUIRE(yars.calculateModifier(character_id, "skill") == Approx(5.5));
	}

	SECTION("Shared Attributes")
	{
		Yars::Skill skill_1 =
		{	.attribute_contribution =
			{	{ "A", 1 }
			,	{ "B", 2 }
			}
		};
		yars.skillAdd("skill_1", skill_1);

		Yars::Skill skill_2 =
		{	.attribute_contribution =
			{	{ "A", 1 }
			,	{ "C", 3 }
			}
		};
		yars.skillAdd("skill_2", skill_2);

		yars.characterSkillAdd(character_id, "skill_1");
		yars.characterSkillAdd(character_id, "skill_2");
		yars.characterSkillIncrease(character_id, "skill_1", 1.0f);
		yars.characterSkillIncrease(character_id, "skill_2", 1.0f);

		Yars::levelSet(yars.characterAttribute(character_id, "A").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "B").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "C").level, 12, 0);

		REQUIRE(yars.calculateModifier(character_id, "skill_1") == Approx(12));
		REQUIRE(yars.calculateModifier(character_id, "skill_2") == Approx(12));

		Yars::levelSet(yars.characterAttribute(character_id, "A").level, 12, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "B").level,  9, 0);
		Yars::levelSet(yars.characterAttribute(character_id, "C").level,  8, 0);

		// (12 * 0.3) + (9 * 0.6) = 4 + 6 = 10
		REQUIRE(yars.calculateModifier(character_id, "skill_1") == Approx(10));

		// (12 * 0.25) + (8 * 0.75) = 3 + 6 = 9
		REQUIRE(yars.calculateModifier(character_id, "skill_2") == Approx(9));
	}
}


TEST_CASE("Calculate.Probability", "[calculate]")
{
	Yars yars;

	SECTION("Sampling")
	{
		REQUIRE(yars.calculateProbability(-1.00) == Approx(-1.000f).margin(0.001));
		REQUIRE(yars.calculateProbability(-0.75) == Approx(-0.469f).margin(0.001));
		REQUIRE(yars.calculateProbability(-0.50) == Approx(-0.218f).margin(0.001));
		REQUIRE(yars.calculateProbability(-0.25) == Approx(-0.088f).margin(0.001));
		REQUIRE(yars.calculateProbability( 0.00) == Approx( 0.000f).margin(0.001));
		REQUIRE(yars.calculateProbability( 0.25) == Approx( 0.088f).margin(0.001));
		REQUIRE(yars.calculateProbability( 0.50) == Approx( 0.218f).margin(0.001));
		REQUIRE(yars.calculateProbability( 0.75) == Approx( 0.469f).margin(0.001));
		REQUIRE(yars.calculateProbability( 1.00) == Approx( 1.000f).margin(0.001));
	}
}


TEST_CASE("Calculate.Difficulty_To_Result", "[calculate]")
{
	REQUIRE(Yars::difficultyToResult(-2.00) == Yars::Result::Success_Automatic);
	REQUIRE(Yars::difficultyToResult(-1.01) == Yars::Result::Success_Automatic);
	REQUIRE(Yars::difficultyToResult(-1.00) == Yars::Result::Chance);
	REQUIRE(Yars::difficultyToResult( 0.00) == Yars::Result::Chance);
	REQUIRE(Yars::difficultyToResult( 1.00) == Yars::Result::Chance);
	REQUIRE(Yars::difficultyToResult( 1.01) == Yars::Result::Failure_Automatic);
	REQUIRE(Yars::difficultyToResult( 2.00) == Yars::Result::Failure_Automatic);
}
