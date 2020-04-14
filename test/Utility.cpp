/*
g++ -std=c++2a -DCATCH_CONFIG_MAIN -Wall -Werror -o Utility Utility.cpp && ./Utility
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


TEST_CASE("mapKeyList")
{
	Yars::MapStringInt32 m =
	{	{ "a", 0 }
	,	{ "b", 0 }
	,	{ "c", 0 }
	};

	Yars::VectorString v = Yars::mapKeyList(m);

	REQUIRE(v.size() == 3);
	REQUIRE(Yars::vectorContains<std::string>(v, "a") == true);
	REQUIRE(Yars::vectorContains<std::string>(v, "b") == true);
	REQUIRE(Yars::vectorContains<std::string>(v, "c") == true);
}


TEST_CASE("multimapContains")
{
	Yars::MultimapIdId m =
	{	{ 0, 0 }
	,	{ 1, 0 }
	,	{ 1, 1 }
	};

	REQUIRE(Yars::multimapContains(m, 0, 0) == true);
	REQUIRE(Yars::multimapContains(m, 1, 0) == true);
	REQUIRE(Yars::multimapContains(m, 1, 1) == true);

	REQUIRE(Yars::multimapContains(m, 0, 1) == false);
}


TEST_CASE("multimapErase")
{
	Yars::MultimapIdId m =
	{	{ 0, 0 }
	,	{ 1, 0 }
	,	{ 1, 1 }
	};

	REQUIRE(Yars::multimapContains(m, 1, 0) == true);

	Yars::multimapErase(m, 1, 0);

	REQUIRE(Yars::multimapContains(m, 1, 0) == false);

	Yars::multimapErase(m, 2, 3);
}


TEST_CASE("vectorContains.Type")
{
	Yars::VectorId v = { 0, 1 };

	REQUIRE(Yars::vectorContains<Yars::Id>(v, 0) == true);
	REQUIRE(Yars::vectorContains<Yars::Id>(v, 1) == true);

	REQUIRE(Yars::vectorContains<Yars::Id>(v, 2) == false);
}


TEST_CASE("vectorContains.Iterator")
{
	Yars::VectorId v = { 0, 1 };

	const auto first = std::begin(v);
	const auto last  = std::end(v);

	REQUIRE(Yars::vectorContains(first, last, 0) == true);
	REQUIRE(Yars::vectorContains(first, last, 1) == true);

	REQUIRE(Yars::vectorContains(first, last, 2) == false);
}
