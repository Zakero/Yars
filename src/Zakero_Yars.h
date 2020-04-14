/******************************************************************************
 * Copyright 2020 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * \file
 */

/******************************************************************************
 * Defines
 */

#if defined (ZAKERO_YARS_ENABLE_DEBUG)
#define ZAKERO_YARS__DEBUG_DISABLED false
#else
#define ZAKERO_YARS__DEBUG_DISABLED true
#endif


#define ZAKERO_YARS_DEBUG \
	if(ZAKERO_YARS__DEBUG_DISABLED) {} \
	else std::cerr \
		<< __FILE__"(" \
		<< std::to_string(__LINE__) \
		<< ") "__PRETTY_FUNCTION__
		
#define ZAKERO_YARS_DEBUG_VAR(var_) \
	ZAKERO_YARS_DEBUG << #var_ << ": " << var_ << "\n";
		

/******************************************************************************
 * Includes
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <unordered_map>
#include <vector>


/******************************************************************************
 * Class
 */

namespace zakero
{
class Yars
{
	// {{{ Utility

	public:
		template <typename Map>
		static std::vector<typename Map::key_type> mapKeyList(const Map& map) noexcept
		{
			std::vector<typename Map::key_type> retval;

			retval.reserve(map.size());

			std::transform(std::begin(map), std::end(map)
				, std::back_inserter(retval)
				, [](const auto& iter)
				{
					return iter.first;
				});

			return retval;
		}

		template <typename Map>
		static bool multimapContains(const Map& map, const typename Map::key_type key, const typename Map::mapped_type value) noexcept
		{
			const auto range = map.equal_range(key);

			for(auto iter = range.first; iter != range.second; iter++)
			{
				if(iter->second == value)
				{
					return true;
				}
			}

			return false;
		}

		template <typename Map>
		static void multimapErase(Map& map, const typename Map::key_type key, const typename Map::mapped_type value) noexcept
		{
			const auto range = map.equal_range(key);

			for(auto iter = range.first; iter != range.second; iter++)
			{
				if(iter->second == value)
				{
					map.erase(iter);
					break;
				}
			}
		}

		template <class Type>
		static bool vectorContains(const std::vector<Type>& vector, const Type& value) noexcept
		{
			return (std::find(std::begin(vector), std::end(vector), value) != std::end(vector));
		}

		template <class InputIter, class Type>
		static bool vectorContains(InputIter first, InputIter last, const Type& value) noexcept
		{
			return (std::find(first, last, value) != last);
		}

	// }}}
	// {{{ Ctor / Dtor

	public:
		Yars();
		explicit Yars(std::mt19937&);

	// }}}
	// {{{ Data Types

	public:
		using Id                   = uint64_t;
		using MapStringBool        = std::unordered_map<std::string, bool>;
		using MapStringFloat       = std::unordered_map<std::string, float>;
		using MapStringInt32       = std::unordered_map<std::string, int32_t>;
		using MapStringString      = std::unordered_map<std::string, std::string>;
		using MapIdMapStringBool   = std::unordered_map<Yars::Id, Yars::MapStringBool>;
		using MapIdMapStringFloat  = std::unordered_map<Yars::Id, Yars::MapStringFloat>;
		using MapIdMapStringInt32  = std::unordered_map<Yars::Id, Yars::MapStringInt32>;
		using MapIdMapStringString = std::unordered_map<Yars::Id, Yars::MapStringString>;
		using MultimapIdId         = std::unordered_multimap<Yars::Id, Yars::Id>;
		using VectorId             = std::vector<Yars::Id>;
		using VectorString         = std::vector<std::string>;

	// }}}
	// {{{ Health

	public:
		struct Health
		{
			uint32_t current          = 0;
			uint32_t maximum          = 0;
			uint32_t maximum_base     = 0;
			int32_t  maximum_modifier = 0;
		};

		using MapIdHealth = std::unordered_map<Yars::Id, Yars::Health>;

		static constexpr int32_t Health_Max = std::numeric_limits<int32_t>::max();
		static constexpr int32_t Health_Min = std::numeric_limits<int32_t>::min();

		static void healthSet(Yars::Health&, const uint32_t, const uint32_t) noexcept;
		static void healthModifierSet(Yars::Health&, const int32_t) noexcept;

		static void healthAdjust(Yars::Health&, const int32_t) noexcept;
		static void healthMaximumAdjust(Yars::Health&, const int32_t) noexcept;
		static void healthModifierAdjust(Yars::Health&, const int32_t) noexcept;

		static void healthIncreaseToMaximum(Yars::Health&) noexcept;

	private:
		static void healthUpdate(Yars::Health&) noexcept;

	// }}}
	// {{{ Level

	public:
		struct Level
		{
			float   partial    = 0;
			int32_t modifier   = 0;
			int32_t value      = 0;
			int32_t value_base = 0;
		};

		static constexpr int32_t Level_Max = std::numeric_limits<int32_t>::max();
		static constexpr int32_t Level_Min = std::numeric_limits<int32_t>::min();

		static uint32_t levelIncrease(Yars::Level&, const float) noexcept;
		static void     levelModifierAdjust(Yars::Level&, const int32_t) noexcept;
		static void     levelSet(Yars::Level&, const int32_t, const float = 0) noexcept;
	
	private:
		static void     levelUpdate(Yars::Level&) noexcept;

	// }}}
	// {{{ Attribute

	public:
		struct Attribute
		{
			Yars::Level level = {};
		};

		using MapStringAttribute      = std::unordered_map<std::string, Yars::Attribute>;
		using MapIdMapStringAttribute = std::unordered_map<Yars::Id, Yars::MapStringAttribute>;

		Yars::Attribute&       attribute(const std::string&) noexcept;
		const Yars::Attribute& attribute(const std::string&) const noexcept;
		bool                   attributeAdd(const std::string&) noexcept;
		bool                   attributeExists(const std::string&) const noexcept;

	private:
		MapStringAttribute attribute_map;

	// }}}
	// {{{ Skill

	public:
		struct Skill
		{
			Yars::MapStringInt32 attribute_contribution = {};
			Yars::Level          level                  = {};
		};

		using MapStringSkill      = std::unordered_map<std::string, Yars::Skill>;
		using MapIdMapStringSkill = std::unordered_map<Yars::Id, Yars::MapStringSkill>;

		Yars::Skill&       skill(const std::string&) noexcept;
		const Yars::Skill& skill(const std::string&) const noexcept;
		bool               skillAdd(const std::string&, const Yars::Skill&) noexcept;
		bool               skillRemove(const std::string&) noexcept;
		bool               skillExists(const std::string&) const noexcept;
		Yars::VectorString skillList() const noexcept;

	private:
		MapStringSkill skill_map;

	// }}}
	// {{{ Item

	public:
		struct Item
		{
			Yars::MapStringInt32 affect_attribute      = {};
			Yars::MapStringInt32 affect_skill          = {};
			Yars::MapStringInt32 modify_attribute      = {};
			Yars::MapStringInt32 modify_skill          = {};
			Yars::VectorString   skill_attacker        = {};
			Yars::VectorString   skill_defender        = {};
			uint32_t             count                 = 0;
			int32_t              affect_health_current = 0;
			int32_t              affect_health_maximum = 0;
			int32_t              modify_health_current = 0;
			int32_t              modify_health_maximum = 0;
		};

		using MapStringItem           = std::unordered_map<std::string, Yars::Item>;
		using MultimapStringItem      = std::unordered_multimap<std::string, Yars::Item>;
		using MapIdMapStringItem      = std::unordered_map<Yars::Id, Yars::MapStringItem>;
		using MapIdMultimapStringItem = std::unordered_map<Yars::Id, Yars::MultimapStringItem>;

		Yars::Item&        item(const std::string&) noexcept;
		const Yars::Item&  item(const std::string&) const noexcept;
		bool               itemAdd(const std::string&, const Yars::Item&) noexcept;
		bool               itemRemove(const std::string&) noexcept;
		bool               itemExists(const std::string&) const noexcept;
		bool               itemIsUsable(const std::string&) const noexcept;
		bool               itemIsUsableByAttacker(const std::string&) const noexcept;
		bool               itemIsUsableByDefender(const std::string&) const noexcept;
		Yars::VectorString itemList() const noexcept;

	private:
		MapStringItem item_map;

	// }}}
	// {{{ Character

	public:
		Yars::Id                        characterCreate() noexcept;
		bool                            characterDelete(const Yars::Id) noexcept;
		bool                            characterExists(const Yars::Id) const noexcept;
		bool                            characterConfigItemStackMax(const Yars::Id, const uint32_t) noexcept;

		Yars::Health&                   characterHealth(const Yars::Id) noexcept;
		const Yars::Health&             characterHealth(const Yars::Id) const noexcept;

		Yars::Attribute&                characterAttribute(const Yars::Id, const std::string&) noexcept;
		const Yars::Attribute&          characterAttribute(const Yars::Id, const std::string&) const noexcept;
		bool                            characterAttributeExists(const Yars::Id, const std::string&) const noexcept;
		Yars::MapStringAttribute&       characterAttributeMap(const Yars::Id) noexcept;
		const Yars::MapStringAttribute& characterAttributeMap(const Yars::Id) const noexcept;

		bool                            characterDataClear(const Yars::Id) noexcept;
		bool                            characterDataErase(const Yars::Id, const std::string&) noexcept;
		bool                            characterDataGet(const Yars::Id, const std::string&, bool&) const noexcept;
		bool                            characterDataGet(const Yars::Id, const std::string&, float&) const noexcept;
		bool                            characterDataGet(const Yars::Id, const std::string&, int32_t&) const noexcept;
		bool                            characterDataGet(const Yars::Id, const std::string&, std::string&) const noexcept;
		bool                            characterDataSet(const Yars::Id, const std::string&, const bool) noexcept;
		bool                            characterDataSet(const Yars::Id, const std::string&, const float) noexcept;
		bool                            characterDataSet(const Yars::Id, const std::string&, const int32_t) noexcept;
		bool                            characterDataSet(const Yars::Id, const std::string&, const std::string&) noexcept;

		Yars::Skill&                    characterSkill(const Yars::Id, const std::string&) noexcept;
		const Yars::Skill&              characterSkill(const Yars::Id, const std::string&) const noexcept;
		bool                            characterSkillAdd(const Yars::Id, const std::string&) noexcept;
		bool                            characterSkillRemove(const Yars::Id, const std::string&) noexcept;
		bool                            characterSkillIncrease(const Yars::Id, const std::string&, const float) noexcept;
		bool                            characterSkillExists(const Yars::Id, const std::string&) const noexcept;
		Yars::VectorString              characterSkillList(const Yars::Id) const noexcept;
		Yars::MapStringSkill&           characterSkillMap(const Yars::Id) noexcept;
		const Yars::MapStringSkill&     characterSkillMap(const Yars::Id) const noexcept;

		Yars::Item&                     characterItem(const Yars::Id, const std::string&, const uint32_t = 0) noexcept;
		const Yars::Item&               characterItem(const Yars::Id, const std::string&, const uint32_t = 0) const noexcept;
		bool                            characterItemAdd(const Yars::Id, const std::string&, const uint32_t = 1) noexcept;
		bool                            characterItemRemove(const Yars::Id, const std::string&, const uint32_t = 1) noexcept;
		Yars::Item                      characterItemTake(const Yars::Id, const std::string&) noexcept;
		bool                            characterItemExists(const Yars::Id, const std::string&) const noexcept;
		uint32_t                        characterItemCount(const Yars::Id, const std::string&) const noexcept;
		uint32_t                        characterItemCount(const Yars::Id, const std::string&, uint32_t&) const noexcept;
		bool                            characterItemIsUsable(const Yars::Id, const std::string&) const noexcept;
		bool                            characterItemIsUsableByAttacker(const Yars::Id, const std::string&) const noexcept;
		bool                            characterItemIsUsableByAttacker(const Yars::Id, const std::string&, const std::string&) const noexcept;
		bool                            characterItemIsUsableByAttacker(const Yars::Id, const Yars::Item&) const noexcept;
		bool                            characterItemIsUsableByDefender(const Yars::Id, const std::string&) const noexcept;
		bool                            characterItemIsUsableByDefender(const Yars::Id, const std::string&, const std::string&) const noexcept;
		bool                            characterItemIsUsableByDefender(const Yars::Id, const Yars::Item&) const noexcept;
		Yars::VectorString              characterItemList(const Yars::Id) const noexcept;
		bool                            characterItemRestack(const Yars::Id) noexcept;
		bool                            characterItemRestack(const Yars::Id, const std::string&) noexcept;

		std::string                     characterItemSkillAttacker(const Yars::Id, const std::string&) const noexcept;
		Yars::VectorString              characterItemSkillAttackerList(const Yars::Id, const std::string&) const noexcept;
		std::string                     characterItemSkillDefender(const Yars::Id, const std::string&) const noexcept;
		Yars::VectorString              characterItemSkillDefenderList(const Yars::Id, const std::string&) const noexcept;

		Yars::Item&                     characterEquipmentItem(const Yars::Id, const std::string&) noexcept;
		const Yars::Item&               characterEquipmentItem(const Yars::Id, const std::string&) const noexcept;
		std::string                     characterEquipmentName(const Yars::Id, const std::string&) const noexcept;
		bool                            characterEquipmentAdd(const Yars::Id, const std::string&, const std::string&) noexcept;
		bool                            characterEquipmentRemove(const Yars::Id, const std::string&) noexcept;
		bool                            characterEquipmentExists(const Yars::Id, const std::string&) const noexcept;

		bool                            characterLevelIncrease(const Yars::Id, const uint32_t) noexcept;
		bool                            characterLevelIncrease(const Yars::Id, const std::string&, const uint32_t) noexcept;
		bool                            characterLevelSet(const Yars::Id, const std::string&, const uint32_t) noexcept;

		bool                            characterAffectApply(const Yars::Id, const Yars::Id) noexcept;
		bool                            characterAffectDefend(const Yars::Id, const Yars::Id, const std::string&) noexcept;
		bool                            characterAffectExists(const Yars::Id, const Yars::Id) const noexcept;
		Yars::VectorId                  characterAffectList(const Yars::Id) const noexcept;
		void                            characterAffectRemove(const Yars::Id, const Yars::Id) noexcept;
		void                            characterAffectRevert(const Yars::Id, const Yars::Id) noexcept;

	private:
		struct CharacterConfig
		{
			uint32_t item_stack_max = 1;
			bool     consolidate    = true;
		};

		using MapIdCharacterConfig = std::unordered_map<Yars::Id, Yars::CharacterConfig>;

		void                          characterAttributeAdd(const Yars::Id, const std::string&) noexcept;
		void                          characterClear(const Yars::Id&) noexcept;
		bool                          characterItemAdd(const Yars::Id, const std::string&, const Yars::Item&) noexcept;

		Yars::Id                      character_id_max;
		Yars::VectorId                character_pool;
		Yars::MapIdCharacterConfig    character_config;
		Yars::MapIdHealth             character_health;
		Yars::MapIdMapStringAttribute character_attribute;
		Yars::MapIdMapStringBool      character_data_bool;
		Yars::MapIdMapStringFloat     character_data_float;
		Yars::MapIdMapStringInt32     character_data_int32;
		Yars::MapIdMapStringSkill     character_skill;
		Yars::MapIdMapStringString    character_data_string;
		Yars::MapIdMapStringItem      character_equipment_item;
		Yars::MapIdMapStringString    character_equipment_name;
		Yars::MapIdMultimapStringItem character_item;
		Yars::MultimapIdId            character_affect;

	// }}}
	// {{{ Affect

	public:
		struct Affect
		{
			uint32_t             level                 = 0;
			Yars::Id             origin_character      = 0;
			std::string          origin_skill          = {};
			float                experience            = 0.0;
			int32_t              modify_health_current = 0;
			int32_t              modify_health_maximum = 0;
			Yars::MapStringInt32 modify_skill          = {};
			Yars::MapStringInt32 modify_attribute      = {};
			Yars::VectorString   defense_skill         = {};
		};

		using VectorAffect = std::vector<Affect>;

		Yars::Affect&       affect(const Yars::Id) noexcept;
		const Yars::Affect& affect(const Yars::Id) const noexcept;
		Yars::Id            affectCreate() noexcept;
		bool                affectDelete(const Yars::Id) noexcept;
		bool                affectExists(const Yars::Id) const noexcept;
		bool                affectInit(const Yars::Id, const Yars::Id, const Yars::Item&, const std::string&, const int32_t = 0) noexcept;

	private:
		void affectClear(const Yars::Id) noexcept;

		Yars::VectorAffect affect_vector;
		Yars::VectorId     affect_pool;

	// }}}
	// {{{ Calculations

	public:
		enum class Result : uint8_t
		{	Error
		,	Failure_Automatic
		,	Failure
		,	Chance
		,	Success
		,	Success_Automatic
		};

		// Deprecated
		//Yars::Result approxDifficultyCheck(const Yars::Id, const std::string&, const Yars::Id) const noexcept;
		//Yars::Result approxDifficultyCheck(const Yars::Id, const std::string&, const std::string&, const Yars::Id, const std::string&) const noexcept;

		float calculateDifficulty(const Yars::Skill&, const Yars::Skill&) const noexcept;
		float calculateDifficulty(const Yars::Skill&, const uint32_t) const noexcept;
		float calculateDifficulty(const uint32_t, const uint32_t) const noexcept;

		bool  calculateIsSuccess(const Yars::Id&, const std::string&, const float) const noexcept;
		bool  calculateIsSuccess(const Yars::Id&, const std::string&, const float, float&) const noexcept;

		float calculateModifier(const Yars::Id, const std::string&) const noexcept;

		float calculateProbability(const float) const noexcept;

		static Yars::Result difficultyToResult(const float) noexcept;

	private:

	// }}}
	// {{{ To String
#if defined (ZAKERO_YARS_ENABLE_TO_STRING)

	public:
		static std::string to_string(const Yars::MapStringFloat&) noexcept;
		static std::string to_string(const Yars::MapStringInt32&) noexcept;
		static std::string to_string(const Yars::MapStringString&) noexcept;
		static std::string to_string(const Yars::VectorId&) noexcept;
		static std::string to_string(const Yars::VectorString&) noexcept;
		static std::string to_string(const Yars::Result) noexcept;
		static std::string to_string(const Yars::Health&) noexcept;
		static std::string to_string(const Yars::Level&) noexcept;
		static std::string to_string(const Yars::Attribute&) noexcept;
		static std::string to_string(const Yars::MapStringAttribute&) noexcept;
		static std::string to_string(const Yars::Skill&) noexcept;
		static std::string to_string(const Yars::MapStringSkill&) noexcept;
		static std::string to_string(const Yars::Item&) noexcept;
		static std::string to_string(const Yars::MapStringItem&) noexcept;
		static std::string to_string(const Yars::Affect&) noexcept;

#endif
	// }}}
	// {{{ Private Data

	private:
		std::mt19937&  mt_engine;

	// }}}
};
};

// {{{ Operator

bool operator==(const zakero::Yars::Item&, const zakero::Yars::Item&);
bool operator!=(const zakero::Yars::Item&, const zakero::Yars::Item&);

// }}}

#if defined (ZAKERO_YARS_IMPLEMENTATION)

namespace zakero
{

// {{{ Defines

/**
 * \page page_Define Define Customization
 *
 * It is possible to add compile time customizations to \yars by adding the 
 * following `#define`'s before including the \yars header.
 *
 *
 *
 * \par ZAKERO_YARS_ENABLE_DEBUG
 *
 * Enabled debugging messages to be written to `stderr`.  These messages are 
 * written by using `ZAKERO_YARS_DEBUG` macro which is also enabled by the 
 * `ZAKERO_YARS_ENABLE_DEBUG` macro.  This macro mostly only useful if 
 * `ZAKERO_YARS_ENABLE_SAFE_MODE` is also enabled.
 *
 * \code
 * #define ZAKERO_YARS_ENABLE_DEBUG
 * \endcode
 *
 *
 *
 * \par ZAKERO_YARS_ENABLE_ID_SHUFFLE
 *
 * When possible, YARS will reuse previously used memory.  The order in which 
 * the memory is reused is predicable.  If this is potentially a problem, 
 * enabling this macro will randomize the reuse order.
 *
 * For (some value of) high amounts of unused memory, this feature can 
 * introduce noticeable overhead.  Experiment to find out if the performance 
 * hit is acceptable in your project.
 *
 * For maximum "data paranoia", enabled both `ZAKERO_YARS_ENABLE_ID_SHUFFLE` 
 * and `ZAKERO_YARS_ENABLE_CLEAN_ON_DELETE`.
 *
 * \code
 * #define ZAKERO_YARS_ENABLE_ID_SHUFFLE
 * \endcode
 *
 *
 *
 * \par ZAKERO_YARS_ENABLE_CLEAR_ON_DELETE
 *
 * Some data objects can be "deleted".  These objects are really placed in an 
 * "unused" pool to be reused later.  This means that they objects could still 
 * be used, much like using free'ed memory.
 * 
 * Defining this macro will cause the objects to be initialized to a clean 
 * state before going into the "unused" pool.  While this can be useful for 
 * debugging it will result in some over-head.
 *
 * \note The data will always be cleared when it is pulled out of the "unused" 
 * pool.
 *
 * For maximum "data paranoia", enabled both `ZAKERO_YARS_ENABLE_ID_SHUFFLE` 
 * and `ZAKERO_YARS_ENABLE_CLEAN_ON_DELETE`.
 *
 * \code
 * #define ZAKERO_YARS_CLEAR_ON_DELETE
 * \endcode
 *
 *
 *
 * \par ZAKERO_YARS_ENABLE_SAFE_MODE
 *
 * Before a method executes, it should validate the argument values that it 
 * receives.  This is useful while developing code but adds overhead.  If the 
 * calling code can guarantee that all argument data is valid, this macro can 
 * be enabled to reduce the validation checks and overhead.
 *
 * \code
 * #define ZAKERO_YARS_ENABLE_SAFE_MODE
 * \endcode
 *
 *
 *
 * \par ZAKERO_YARS_ENABLE_TO_STRING
 *
 * Some data structures have a corresponding [Yars::to_string()](\ref 
 * group_ToString) method to help make debugging easier.  The output of these 
 * methods is a JSON formatted object or array.  This was done to be agnostic 
 * to all the available JSON libraries.  Enabling the 
 * `ZAKERO_YARS_ENABLE_TO_STRING` will make those methods available.
 *
 * \code
 * #define ZAKERO_YARS_ENABLE_TO_STRING
 * \endcode
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \defgroup group_ToString ZAKERO_YARS_ENABLE_TO_STRING
 *
 * \brief Functionality that is only available when \yars is compiled with the 
 * [ZAKERO_YARS_ENABLE_TO_STRING](\ref page_Define) macro.
 *
 * Most of \yars objects and custom data types have conversion to JSON 
 * formatted string support.  These methods will create a std::string object 
 * with all the data and return it.  Creating the std::string can be an 
 * expensive operation and considering the generated strings are mostly only 
 * useful for debugging, the %to_string() methods are disabled by default.
 */
#endif

#if defined (ZAKERO_YARS_DEBUG)
/**
 * \defgroup group_Debugging ZAKERO_YARS_DEBUG
 *
 * \brief Functionality that is only available when \yars is compiled with the 
 * [ZAKERO_YARS_ENABLE_DEBUG](\ref page_Define) macro.
 *
 * These macros are available to help make debugging easier.
 */

/**
 * \ingroup group_Debugging
 *
 * \def ZAKERO_YARS__DEBUG_DISABLED
 *
 * \brief Internal Use Only
 *
 * The inverse of ZAKERO_YARS_DEBUG_ENABLED and is used to enable or disable 
 * the debugging messages.
 */

/**
 * \ingroup group_Debugging
 *
 * \def ZAKERO_YARS_DEBUG
 *
 * \brief Write debugging message.
 *
 * If the `ZAKERO_YARS_ENABLE_DEBUG` macro is enabled, this macro will write to 
 * the `std::cerr` stream.  Nothing will happen when the macro is disabled.
 *
 * \example
 * ZAKERO_YARS_DEBUG << "Insert debugging message here.\n";
 * \endexample
 */

/**
 * \ingroup group_Debugging
 *
 * \def ZAKERO_YARS_DEBUG_VAR(var_)
 *
 * \brief Write debugging message.
 *
 * Does the samething as `ZAKERO_YARS_DEBUG` but is specialized to work with 
 * std::ostream compatible objects.
 *
 * \example
 * uint64_t foo = 42;
 * ZAKERO_YARS_DEBUG_VAR(foo);
 * \endexample
 *
 * \param var_ The variable to write.
 */
#endif

// }}}
// {{{ Anonymous Namespace

namespace
{
	std::mt19937 zakero_yars_mt19937(
		std::chrono::system_clock::now().time_since_epoch().count()
		);

	std::uniform_real_distribution<float> zakero_yars_distribution_(-1.0f, 1.0f);
};

// }}}

/**
 * \todo{Doxygen} Add Doxygen support
 * \todo{STLAllocator} Expose the containers so that custom STL Allocators can 
 * be used.
 * \todo{Events} Add Event: Health.Current.Changed
 * \todo{Events} Add Event: Health.Maximum.Changed
 * \todo{Events} Add Event: Attribute.Added
 * \todo{Events} Add Event: Attribute.Decreased
 * \todo{Events} Add Event: Attribute.Increased
 * \todo{Events} Add Event: Skill.Added
 * \todo{Events} Add Event: Skill.Decreased
 * \todo{Events} Add Event: Skill.Increased
 * \todo{Events} Add Event: Item.Added
 * \todo{Events} Add Event: Item.Removed
 * \todo{Events} Add Event: Level.Up
 * \todo{Events} Add Event: Died
 * \todo{Config} Config.ItemAutoStack[bool] See: characterItemAdd()
 * - Per Character config?
 * .
 * \todo{Config} Config.EventThread
 * - [time in milliseconds to flush the event queue]
 * - Compile Time?
 * .
 * \todo{Config} Config.CharacterCountMax
 * - Compile Time?
 * .
 * \todo{Config} Config.ItemStackCountMax
 * - Compile Time?
 * - Per Character config?
 * .
 * \todo{Config} Config.InventoryItemCountMax
 * - Per Character config
 * .
 * \todo Thread-Safety
 * \todo Damage Over Time
 * \todo Add "Affect Data" support like "Character Data"
 * - "Affect Data" is based on the Affect Id, not per-instance
 * .
 * \todo Data Storage?
 * \todo What if damage was scaled to success?
 * - Automatic success : %100 damage
 * - Automatic failure : %0 damage
 * - Chance: 1 - ((rand - mod) / 2)
 * - --- Maybe a bad idea ---
 * - This would remove the need for (chance to hit).  A chance to hit 
 * for %10 of max damage is not fun.  If an enemy has a 100% chance to 
 * hit, regardless of damage, then that may not be fun.
 * - This effectively reduces the average damage by 50%.
 *
 * \class Yars
 *
 * \brief Y.A.R.S.
 *
 * The %Yars class embodies the core RPG Rule System.  The \yars contains all 
 * the basic objects needed in RPG's such as characters and items, plus how all 
 * parts can interact with each other.  Wars of magic, swords, and words can be 
 * done by using Affects that impose changes on the characters.
 *
 * Almost everything revolves around the Character.  Characters are made up of 
 * Skills and Skills are made up of Attributes.  Attributes, Characters, and 
 * Skills all have Levels.  Characters also have Health.  Items are used to 
 * create Affects and those Affects can be applied to Characters.
 *
 * While it is encouraged to change the values of data objects using the 
 * methods, all values are public to make it easier to modify if needed.
 *
 * \par Debugging
 *
 * If you would like the ability to send debugging output to Standard Error, 
 * enable the [ZAKERO_YARS_ENABLE_DEBUG](\ref page_Define) macro when building 
 * \yars.  This will in turn enable the debugging macros \ref ZAKERO_YARS_DEBUG 
 * and \ref ZAKERO_YARS_DEBUG_VAR.
 *
 * \par Data Lifespan
 *
 * \yars stores data in maps and vectors.  When new data is added to maps and 
 * vectors, it can cause those containers to another area in memory.  This is 
 * important to realize because all the Yars methods return a reference to a 
 * data item.  __This means that holding on to a reference and adding data of 
 * the same type can invalidate that reference.__
 *
 * The intent of using references was to allow the caller to have direct access 
 * to the internal data structures so that they can read (and write if 
 * necessary) directly without going through any indirection.  This also allows 
 * the C++ STL to manage the memory resource reducing the possibility of memory 
 * leaks to almost 0.
 */

// {{{ Utility

/**
 * \defgroup group_Utility Utility
 *
 * \brief Utility functions.
 *
 * A collection of utility methods within the Yars class that improve the 
 * Quality-Of-Life.
 */

/**
 * \ingroup group_Utility
 *
 * \fn static std::vector<typename Map::key_type> Yars::mapKeyList(const Map& map) noexcept
 *
 * \brief Get all the keys in a map.
 *
 * This function template will collect all the keys of the provided \p map and 
 * return them in a vector.
 *
 * \return A std::vector of keys.
 *
 * \tparam Map The container type
 *
 * \param  map The map container
 */

/**
 * \ingroup group_Utility
 *
 * \fn static bool Yars::multimapContains(const Map& map, const typename Map::key_type key, const typename Map::mapped_type value) noexcept
 *
 * \brief Check the contents of a MultiMap.
 * 
 * Checking the contents of a multimap for a key/value pair requires a lot of 
 * boiler-plate code.  This function template provides the needed generic 
 * boiler-plate.
 *
 * \retval true  The key/value pair was found.
 * \retval false The key/value pair was __not__ found.
 *
 * \tparam Map The multimap
 *
 * \param map   The multimap to check
 * \param key   The key in the multimap
 * \param value The value in the multimap
 */

/**
 * \ingroup group_Utility
 *
 * \fn static void Yars::multimapErase(Map& map, const typename Map::key_type key, const typename Map::mapped_type value) noexcept
 *
 * \brief Remove a value from MultiMap.
 *
 * Removing a specific key/value pair from a MulitMap is an easy thing to do 
 * but it does require excessive amounts of code.  This function template 
 * provides the needed generic boiler-plate code.
 *
 * \tparam Map The multimap
 *
 * \param map   The multimap to check
 * \param key   The key in the multimap
 * \param value The value in the multimap
 */

/**
 * \ingroup group_Utility
 *
 * \fn static bool Yars::vectorContains(const std::vector<Type>& vector, const Type& value) noexcept
 *
 * \brief Check if a vector contains a value
 *
 * Searching a vector just to find out if a value is present can be done by 
 * using `std::find()`.  However, littering your code with calls to `std::find() 
 * == std::end()` is excessively ugly.  Using this template function to simplify 
 * your code.
 *
 * \tparam Type The content type of the vector
 *
 * \retval true  The value was found
 * \retval false The value was not found
 *
 * \param vector The vector to search
 * \param value  The value to look for
 */

/**
 * \ingroup group_Utility
 *
 * \fn static bool Yars::vectorContains(InputIter first, InputIter last, const Type& value) noexcept
 *
 * \brief Check if a vector contains a value
 *
 * Searching a vector just to find out if a value is present can be done by 
 * using `std::find()`.  However, littering your code with calls to `std::find() 
 * == std::end()` is excessively ugly.  Using this template function to simplify 
 * your code.
 *
 * \tparam InputIter The vector iterator type
 * \tparam Type      The content type of the vector
 *
 * \retval true  The value was found
 * \retval false The value was not found
 *
 * \param first The starting location for the search
 * \param last  The ending location for the search
 * \param value  The value to look for
 */

// }}}
// {{{ Ctor / Dtor

/**
 * \brief Default Constructor
 *
 * Create a new instance using the default random number generator.
 */
Yars::Yars()
	: Yars(zakero_yars_mt19937)
{
}


/**
 * \brief Constructor
 *
 * Create a new instance using the provided random number generator.  For the 
 * life-time of the new %Yars instance, it will keep a reference to the \p 
 * mt19937 object.
 *
 * \todo Find a way to support other random number generators. Maybe via 
 * template parameters.
 */
Yars::Yars(std::mt19937& mt19937 ///< The random number generator
	)
	: attribute_map()
	, skill_map()
	, item_map()
	, character_id_max(0)
	, character_pool()
	, character_config()
	, character_health()
	, character_attribute()
	, character_data_bool()
	, character_data_float()
	, character_data_int32()
	, character_skill()
	, character_data_string()
	, character_equipment_item()
	, character_equipment_name()
	, character_item()
	, character_affect()
	, affect_vector()
	, affect_pool()
	, mt_engine(mt19937)
{
}

// }}}
// {{{ Data Types

/**
 * \typedef Yars::Id
 *
 * \brief Used for unique IDs
 */

/**
 * \typedef Yars::MapStringBool
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapStringFloat
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapStringInt32
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapStringString
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapIdMapStringBool
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapIdMapStringFloat
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapIdMapStringInt32
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapIdMapStringString
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MultimapIdId
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::VectorId
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::VectorString
 *
 * \brief A convenience type
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringFloat to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringFloat& map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	bool first = true;
	for(const auto& iter : map)
	{
		if(first == true)
		{
			first = false;
			oss << "{";
		}
		else
		{
			oss << ",";
		}

		const std::string& name  = iter.first;
		const float&       value = iter.second;

		oss << " \"" << name << "\": " << std::to_string(value);
	}

	oss << " }";
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringInt32 to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringInt32& map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	bool first = true;
	for(const auto& iter : map)
	{
		if(first == true)
		{
			first = false;
			oss << "{";
		}
		else
		{
			oss << ",";
		}

		const std::string& name  = iter.first;
		const float&       value = iter.second;

		oss << " \"" << name << "\": " << std::to_string(value);
	}

	oss << " }";
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringString to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringString& map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	bool first = true;
	for(const auto& iter : map)
	{
		if(first == true)
		{
			first = false;
			oss << "{";
		}
		else
		{
			oss << ",";
		}

		const std::string& name  = iter.first;
		const std::string& value = iter.second;

		oss << " \"" << name << "\": \"" << value << "\"";
	}

	oss << " }";
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::VectorId to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::VectorId& vector ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss << "[";

	bool first = true;
	for(const auto& value : vector)
	{
		if(first == false)
		{
			oss << ",";
		}

		oss << " " << std::to_string(value);

		first = false;
	}

	oss << " ]";
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::VectorString to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::VectorString& vector ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss << "[";

	if(vector.empty() == false)
	{
		oss << " \"";

		std::copy(std::begin(vector), std::end(vector) - 1
			, std::ostream_iterator<std::string>(oss, "\" ,\"")
			);

		oss << vector.back();
		oss << "\"";
	}

	oss << " ]";
	
	return oss.str();
}
#endif

// }}}
// {{{ Health

/**
 * \defgroup group_Health Health
 *
 * \brief Health data and manipulation
 *
 * The main components of Health in \yars is Yars::Health.current and 
 * Yars::Health.maximum.  The Yars::Health.current value can not be greater 
 * than the Yars::Health.maximum level.  Also, the Yars::Health.current value 
 * and the Yars::Health.maximum value can not be less than `0`.
 *
 * If the Yars::Health.current reaches a value of `0` that is to be considered 
 * as character death, or an Item is either broken or unusable.  If 
 * Yars::Health.maximum is `0` then it that character can not be healed or that 
 * item can not be repaired.
 *
 * \bug Yars::Item does not have Yars::Health.  Determine if Yars::Health 
 * should be added to Yars::Item or if the documentation needs to be updated.
 *
 * If at any point the maximum health value is reduced to be below the current 
 * health value (using the health methods), the current health value will be 
 * automatically reduced.  This means that __order matters__.  For example:
 * 1. Bad order
 *   - health modifier is set to a large negative value
 *     - health maximum value is reduced to `0`
 *     - health current value is reduced to `0`
 *     - considered dead
 *   - health maximum base increased
 *     - health maximum value is > `0`
 *     - health current value is still `0`
 *     - still considered dead
 * 2. Good order
 *   - health maximum base increased
 *     - health maximum value is > `0`
 *     - health current value is > `0`
 *     - not dead
 *   - health modifier is set to a large negative value
 *     - health maximum value is still > `0`
 *     - health current value is still > `0`
 *     - still not dead
 *
 *
 * There is no concern of overflowing the Yars::Health values because they they 
 * are promoted to 64-bit values before any calculations are preformed.  The 
 * 64-bit values are then clamped to the valid range of values, then finally 
 * demoted back to 32-bit values.
 *
 * Using the health related methods is highly recommended.  Any value in 
 * Yars::Health that is modified to be outside a valid range will be reset the 
 * next time one of the health methods are called.
 *
 * __Value Ranges__
 *
 * \f$Health.current=(0<=Health.current<=Health.maximum)\f$
 *
 * \f$Health.maximum=(0<=(Health.maximum\_base+Health.maximum\_modifier)<=Health\_Max)\f$
 *
 * \f$Health.maximum\_base=(0<=Health.maximum\_base<=Health\_Max)\f$
 *
 * \f$Health.maximum\_modifier=(Health\_Min<=Health.maximum\_modifier<=Health\_Max)\f$
 *
 * \see Yars::Health_Max
 * \see Yars::Health_Min
 */

/**
 * \ingroup group_Health
 *
 * \struct Yars::Health
 *
 * \brief Health Data
 */

/**
 * \var Yars::Health::current
 *
 * \brief The current amount of health
 */

/**
 * \var Yars::Health::maximum
 *
 * \brief The calculated value for the maximum amount of health
 */

/**
 * \var Yars::Health::maximum_base
 *
 * \brief The base value of maximum health
 */

/**
 * \var Yars::Health::maximum_modifier
 *
 * \brief The maximum health modifier
 */

/**
 * \typedef Yars::MapIdHealth
 *
 * \brief A convenience type.
 */

/**
 * \ingroup group_Health
 *
 * \var Yars::Health_Max
 *
 * \brief The maximum health value.
 */

/**
 * \ingroup group_Health
 *
 * \var Yars::Health_Min
 *
 * \brief The minimum health value.
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Health to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Health& health ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"current\": "          << std::to_string(health.current)
		<< ", \"maximum\": "          << std::to_string(health.maximum)
		<< ", \"maximum_base\": "     << std::to_string(health.maximum_base)
		<< ", \"maximum_modifier\": " << std::to_string(health.maximum_modifier)
		<< " }"
		;
	
	return oss.str();
}
#endif


/**
 * \ingroup group_Health
 *
 * \brief Set health values.
 *
 * The current health will be set as well as the maximum base value.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * // health.current == 200
 * // health.maximum == 200
 * \endexample
 */
void Yars::healthSet(Yars::Health& health ///< The health data to modify.
	, const uint32_t current          ///< The new current value.
	, const uint32_t maximum          ///< The new maximum value.
	) noexcept
{
	health.current      = current;
	health.maximum_base = std::min(maximum, (uint32_t)Yars::Health_Max);

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Set the maximum modifier value.
 *
 * The current health will be set as well as the maximum base value.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * Yars::healthModifierSet(health, -100);
 * // health.current == 100
 * // health.maximum == 100
 * \endexample
 */
void Yars::healthModifierSet(Yars::Health& health ///< The health data to modify.
	, const int32_t modifier                  ///< The new modifier value
	) noexcept
{
	health.maximum_modifier = modifier;

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Adjust the current health value.
 *
 * The current health value will be adjusted by the provided \p amount.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * Yars::healthAdjust(health, -10);
 * // health.current == 190
 * // health.maximum == 200
 * \endexample
 */
void Yars::healthAdjust(Yars::Health& health ///< The health data to modify.
	, const int32_t amount               ///< The amount to change the current health
	) noexcept
{
	const int64_t value = (int64_t)health.current + (int64_t)amount;

	health.current = (uint32_t)std::clamp(value
		, (int64_t)0
		, (int64_t)Yars::Health_Max
		);

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Adjust the maximum health value.
 *
 * The maximum health value will be adjusted by the provided \p amount.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * Yars::healthMaximumAdjust(health, -10);
 * // health.current == 190
 * // health.maximum == 190
 * \endexample
 */
void Yars::healthMaximumAdjust(Yars::Health& health ///< The health data to modify.
	, const int32_t amount                      ///< The amount to change the current health
	) noexcept
{
	const int64_t value = (int64_t)health.maximum_base + (int64_t)amount;

	health.maximum_base = (uint32_t)std::clamp(value
		, (int64_t)0
		, (int64_t)Yars::Health_Max
		);

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Adjust the modifier health value.
 *
 * The maximum health modifier value will be adjusted by the provided \p 
 * amount.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * Yars::healthModifierAdjust(health, -10);
 * // health.current == 190
 * // health.maximum == 190
 * \endexample
 */
void Yars::healthModifierAdjust(Yars::Health& health ///< The health data to modify.
	, const int32_t amount                       ///< The amount to change the current health
	) noexcept
{
	const int64_t value = (int64_t)health.maximum_modifier + (int64_t)amount;

	health.maximum_modifier = (uint32_t)std::clamp(value
		, (int64_t)Yars::Health_Min
		, (int64_t)Yars::Health_Max
		);

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Increase the current health value.
 *
 * The current health value will be increased to the maximum value.
 *
 * \example
 * Yars::Health health;
 * Yars::healthSet(health, 200, 200);
 * Yars::healthAdjust(health, -90);
 * Yars::healthIncreaseToMaximum(health);
 * // health.current == 200
 * // health.maximum == 200
 * \endexample
 */
void Yars::healthIncreaseToMaximum(Yars::Health& health ///< The health data to modify.
	) noexcept
{
	health.current = health.maximum;

	healthUpdate(health);
}


/**
 * \ingroup group_Health
 *
 * \brief Update the current and maximum values
 */
void Yars::healthUpdate(Yars::Health& health ///< The health data to modify.
	) noexcept
{
	int64_t value = (int64_t)health.maximum_base + (int64_t)health.maximum_modifier;

	health.maximum = (uint32_t)std::clamp(value
		, (int64_t)0
		, (int64_t)Yars::Health_Max
		);

	health.current = std::clamp(health.current
		, (uint32_t)0
		, health.maximum
		);
}

// }}}
// {{{ Level

/**
 * \todo Determine if Level value can be negative without any problems.
 *
 * \defgroup group_Level Level
 *
 * \brief Level data and manipulation
 *
 * Level plays an important role in \yars because it determines how strong the 
 * character's attributes and skills are.  The following aspects make up a 
 * Level:
 * - __Value__<br>
 *   This is the raw value of a Level, such as "Skill is Level 5".  When a 
 *   Yars::Skill or an Yars::Attribute refers to a Level, this is the value 
 *   that is used.
 *   <br><br>
 * - __Partial__<br>
 *   When a Level increases, the amount of the increase is a floating point 
 *   number.  The partial values accumulate the level increases until the 
 *   amount reaches the threshold to increase the __value__.
 *   <br><br>
 * - __Modifier__<br>
 *   The modifier is a bonus or penalty that is applied to the __value__.
 *
 * The requirements for a level increase is: 
 * \f$Level.partial>=Level.value\_base\f$
 * When the requirements are satisfied, Level.partial is decreased by 
 * Level.value_base then the Level.value_base is increased by 1.  Finally, the 
 * Level.modifier is applied and the result is stored in Level.value.
 *
 * The reason that the Level.modifier is applied to Level.value_base and not 
 * Level.partial is to maintain the effectiveness of the modifier.  The higher 
 * the Level.value is, the less likely it is that an increase to Level.partial 
 * will matter.  Applying the modifier to the value, a +1 is useful to all 
 * levels.
 *
 * The Level.modifier only takes affect when the Level.value_base is greater 
 * than `0`.  The Level.value will never go below `0`, regardless of the 
 * Level.modifier value.
 */

/**
 * \ingroup group_Level
 *
 * \struct Yars::Level
 *
 * \brief %Level Data
 */

/**
 * \bug Create a partial_base that tracks the uint32_t partial value.  The 
 * partial will still collect the increases and for each whole number above 
 * zero, that value will be added to partial_base.  __Avoid rounding errors in 
 * higher floating point values.__
 *
 * \var Yars::Level::partial
 *
 * \brief %Level increase accumulator.
 */

/**
 * \var Yars::Level::modifier
 *
 * \brief Modify the %Level value
 */

/**
 * \var Yars::Level::value
 *
 * \brief %Level value.
 */

/**
 * \var Yars::Level::value_base
 *
 * \brief The raw %Level value.
 */

/**
 * \ingroup group_Level
 *
 * \var Yars::Level_Max
 *
 * \brief The maximum level value.
 */

/**
 * \ingroup group_Level
 *
 * \var Yars::Level_Min
 *
 * \brief The minimum level value.
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Level to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Level& level ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"value_base\": " << level.value_base
		<< ", \"value\": "      << level.value
		<< ", \"partial\": "    << std::to_string(level.partial)
		<< ", \"modifier\": "   << level.modifier
		<< " }"
		;
	
	return oss.str();
}
#endif


/**
 * \ingroup group_Level
 *
 * \brief Increase the Level
 *
 * Increase the provided \p level with the specified \p amount.  If \p amount 
 * is less than `0`, then the method will just return `0`.
 *
 * \example
 * Yars::Level level;
 * Yars::levelIncrease(level, 1.5f);
 * // level.modifier   == 0
 * // level.partial    == 0.5
 * // level.value_base == 1
 * // level.value      == 1
 * \endexample
 *
 * \return The number of levels gained
 */
uint32_t Yars::levelIncrease(Yars::Level& level ///< The Level to increase
	, const float amount                    ///< The amount to increase
	) noexcept
{
	if(amount < 0)
	{
		return 0;
	}

	level.partial += amount;

	uint32_t level_up = 0;

	if(level.partial >= 1)
	{
		if(level.value_base == 0)
		{
			level.value_base = 1;
			level.partial -= 1.0f;
			level_up++;
		}

		while(level.partial >= level.value_base)
		{
			level.partial -= level.value_base;
			level.value_base++;
			level_up++;
		}
	}

	levelUpdate(level);

	return level_up;
}


/**
 * \ingroup group_Level
 *
 * \brief Adjust the modifier value
 *
 * Apply the \p amount to the Level modifier.
 *
 * \example
 * Yars::Level level;
 * Yars::levelIncrease(level, 1.5f);
 * Yars::levelModifierAdjust(level, 2);
 * // level.modifier   == 2
 * // level.partial    == 0.5
 * // level.value_base == 1
 * // level.value      == 3
 * \endexample
 */
void Yars::levelModifierAdjust(Yars::Level& level ///< The Level modifier to increase
	, const int32_t amount                    ///< The amount to increase
	) noexcept
{
	int64_t modifier = (int64_t)level.modifier + (int64_t)amount;

	level.modifier = (int32_t)std::clamp(modifier
		, (int64_t)Yars::Level_Min
		, (int64_t)Yars::Level_Max
		);

	levelUpdate(level);
}


/**
 * \ingroup group_Level
 *
 * \brief Set the Level
 *
 * The Level value and partial will be set as specified.  If the partial value 
 * is large enough, the level value will be adjusted accordingly.
 *
 * \example
 * Yars::Level level;
 * Yars::levelSet(level, 1, 0.5f);
 * // level.modifier   == 0
 * // level.partial    == 0.5
 * // level.value_base == 1
 * // level.value      == 1
 * \endexample
 */
void Yars::levelSet(Yars::Level& level ///< The Level to set
	, const int32_t value          ///< The new Level value
	, const float partial          ///< The new Level partial value
	) noexcept
{
	level.value_base = value;
	level.partial    = 0;

	levelIncrease(level, partial);
}


/**
 * \ingroup group_Level
 *
 * \brief Update the Level value
 *
 */
void Yars::levelUpdate(Yars::Level& level
	) noexcept
{
	// Level modifiers only apply if the base value is not 0
	if(level.value_base == 0)
	{
		level.value = 0;
	}
	else
	{
		int64_t value = (int64_t)level.value_base + (int64_t)level.modifier;
		level.value = (int32_t)std::clamp(value
			, (int64_t)0
			, (int64_t)std::numeric_limits<int32_t>::max()
			);
	}
}

// }}}
// {{{ Attribute

/**
 * \defgroup group_Attribute Attribute
 *
 * \brief Attributes
 *
 * Attributes are optional elements of Skills and if present, they provide 
 * additional bonus to the Skill.
 *
 * Creating Attributes is automatic but if Attributes needs to be added 
 * manually, they only have two parts:
 * - __Level__<br>
 *   The Level of a Attribute represents how much it can influence a Skill.
 *   <br><br>
 * - __Name__<br>
 *   All Attributes must have a name.  No two Attributes may have the same 
 *   name.
 *
 * Attributes can not be used directly in \yars.  When Skills are used, they 
 * will used the Attributes Level as needed.
 *
 * Attributes that are added directly to the Yars class are treated as the 
 * baseline Attributes.  This means that if an Attribute is added to Yars and 
 * it is set to have a Level value of 10, then when that same Attribute is 
 * added to a Character the Attribute for Yars is copied into the Character.
 *
 * Currently, Attributes can not be removed.  The reason is that Skills can be 
 * added and removed which could cause the Attribute to be added right back.  
 * And adding some type of "count" to the Attribute to track the number of 
 * dependencies to determine when to "clean" unused Attributes does not out 
 * weigh the overhead.  Yet.
 */

/**
 * \ingroup group_Attribute
 *
 * \struct Yars::Attribute
 *
 * \brief Attribute data
 */

/**
 * \var Yars::Attribute::level
 *
 * \brief The level of the Attribute.
 */

/**
 * \typedef Yars::MapStringAttribute
 *
 * \brief A convenience type
 */

/**
 * \typedef Yars::MapIdMapStringAttribute
 *
 * \brief A convenience type
 */

/**
 * \var Yars::attribute_map
 *
 * \brief A collection of Attributes
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Attribute to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Attribute& attribute ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"level\": "
		<< Yars::to_string(attribute.level)
		<< " }"
		;
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringAttribute to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringAttribute& attribute_map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss << "{";

	bool first = true;
	for(const auto& iter : attribute_map)
	{
		if(first == false)
		{
			oss << ",";
		}

		const std::string&     name      = iter.first;
		const Yars::Attribute& attribute = iter.second;

		oss	<< " \""
			<< name
			<< "\": "
			<< Yars::to_string(attribute)
			;

		first = false;
	}

	oss << " }";

	return oss.str();
}
#endif


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Attribute
 *
 * \brief Get an Attribute
 *
 * Access an Attribute.
 *
 * \example
 * Yars yars;
 * yars.attributeAdd("Attr");
 * Yars::Attribute& attr = yars.attribute("Attr");
 * \endexample
 *
 * \return An Attribute
 */
Yars::Attribute& Yars::attribute(const std::string& attribute_name ///< The Attribute name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(attribute_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		Attribute* retval = nullptr;
		return *retval;
	}

	if(attributeExists(attribute_name) == false)
	{
		//ZAKERO_YARS_DEBUG << "Does Not Exist" << std::endl;
		Attribute* retval = nullptr;
		return *retval;
	}
#endif

	return attribute_map[attribute_name];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Attribute
 *
 * \brief Get an Attribute
 *
 * Access an Attribute.
 *
 * \example
 * Yars yars;
 * yars.attributeAdd("Attr");
 * const Yars& y = yars;
 * const Yars::Attribute& attr = y.attribute("Attr");
 * \endexample
 */
const Yars::Attribute& Yars::attribute(const std::string& attribute_name ///< The Attribute name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(attribute_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		Attribute* retval = nullptr;
		return *retval;
	}

	if(attributeExists(attribute_name) == false)
	{
		//ZAKERO_YARS_DEBUG << "Does Not Exist" << std::endl;
		Attribute* retval = nullptr;
		return *retval;
	}
#endif

	return attribute_map.at(attribute_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Attribute
 *
 * \brief Add an Attribute
 *
 * Add a new Attribute.
 *
 * \example
 * Yars yars;
 * yars.attributeAdd("Attr");
 * \endexample
 *
 * \retval true  The Attribute was added
 * \retval false Failed to add the Attribute
 */
bool Yars::attributeAdd(const std::string& attribute_name ///< The Attribute name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(attribute_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		return false;
	}

	if(attributeExists(attribute_name))
	{
		//ZAKERO_YARS_DEBUG << "Already Exists" << std::endl;
		return false;
	}
#endif

	attribute_map[attribute_name] = {};

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Attribute
 *
 * \brief Check if an Attribute already exists
 *
 * \example
 * Yars yars;
 * yars.attributeAdd("Attr");
 * if(yars.attributeExists("Attr"))
 * {
 * 	std::cout << "Woot!\n";
 * }
 * \endexample
 *
 * \retval true  The Attribute was added
 * \retval false Failed to add the Attribute
 */
bool Yars::attributeExists(const std::string& attribute_name ///< The Attribute name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(attribute_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		return false;
	}
#endif

	return attribute_map.contains(attribute_name);
}

// }}}
// {{{ Skill

/**
 * \defgroup group_Skill Skill
 *
 * \brief Skills
 *
 * Skills are the core of \yars.  For anything to interact with anything else 
 * there must be a Skill that allows the interaction.  Most methods in the Yars 
 * class only require the name of the Skill.
 *
 * Creating Skills is very easy and consists of three parts:
 * - __Attributes__<br>
 *   This defines which Attributes will apply when the Skill is used.  These 
 *   Attribute will automatically be added to a Character if the Character does 
 *   not have them.
 *   <br><br>
 * - __Level__<br>
 *   The Level of a Skill represents how effective it is.  This effectiveness 
 *   is relative however.  If an attacker is using a Skill with a Level of 10 
 *   and the defender is using a Skill with a Level of 8, then the attacker's 
 *   Skill Level is actually 2.  (attacker - defender = 10 - 8 = 2).
 *   <br><br>
 * - __Name__<br>
 *   All Skills must have a name.  No two Skills may have the same name.
 *
 * The main use of Skills is with [Affects](\ref group_Affect).
 *
 * Skills must be added to the Yars class before they can be added to a 
 * Character.  When added to a Character, the Skill is copied from the Yars 
 * class into the Character.  If the Skill is changed in the Yars class after 
 * it was added to a Character, the Character will __not__ see the changes.
 *
 * To learn how the Yars::Skill.attribute_contribution is used, refer to 
 * Yars::calculateModifier().
 */

/**
 * \ingroup group_Skill
 *
 * \struct Yars::Skill
 *
 * \brief Skill data
 */

/**
 * \var Yars::Skill::attribute_contribution
 *
 * \brief Attribute names and how much they contribute.
 */

/**
 * \var Yars::Skill::level
 *
 * \brief The Skill level.
 */

/**
 * \typedef Yars::MapStringSkill
 *
 * \brief A convenience type.
 */

/**
 * \typedef Yars::MapIdMapStringSkill
 *
 * \brief A convenience type.
 */

/**
 * \var Yars::skill_map
 *
 * \brief A collection of Skills.
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Skill to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Skill& skill ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"level\": "
		<< Yars::to_string(skill.level)
		<< ", \"attribute_contribution\": " << Yars::to_string(skill.attribute_contribution)
		<< " }"
		;
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringSkill to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringSkill& skill_map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss << "{";

	bool first = true;
	for(const auto& iter : skill_map)
	{
		if(first == false)
		{
			oss << ",";
		}

		const std::string& name  = iter.first;
		const Yars::Skill& skill = iter.second;

		oss	<< " \""
			<< name
			<< "\": "
			<< Yars::to_string(skill)
			;

		first = false;
	}

	oss << " }";
	
	return oss.str();
}
#endif


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Skill
 *
 * \brief Get an Skill
 *
 * Access a Skill.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * Yars::Skill& skill = yars.skill("Skill");
 * \endexample
 *
 * \return A Skill
 */
Yars::Skill& Yars::skill(const std::string& skill_name ///< The Skill name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(skill_name.empty())
	{
		Skill* retval = nullptr;
		return *retval;
	}

	if(skillExists(skill_name) == false)
	{
		Skill* retval = nullptr;
		return *retval;
	}
#endif

	return skill_map[skill_name];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Skill
 *
 * \brief Get an Skill
 *
 * Access a Skill.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * const Yars& y = yars;
 * const Yars::Skill& skill = y.skill("Skill");
 * \endexample
 *
 * \return A Skill
 */
const Yars::Skill& Yars::skill(const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(skill_name.empty())
	{
		Skill* retval = nullptr;
		return *retval;
	}

	if(skillExists(skill_name) == false)
	{
		Skill* retval = nullptr;
		return *retval;
	}
#endif

	return skill_map.at(skill_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Skill
 *
 * \brief Add a Skill
 *
 * Add a new Skill.  If the Attributes named in the 
 * <code>attribute_contribution</code> do not exist in the Yars class, they 
 * will be added.
 *
 * \see attributeAdd()
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Foo", {});
 * yars.skillAdd("Bar",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	,	.level =
 * 		{	.value      = 1
 * 		,	.value_base = 1
 * 		}
 * 	});
 * \endexample
 *
 * \retval true  The Skill was added
 * \retval false Failed to add the Skill
 */
bool Yars::skillAdd(const std::string& skill_name ///< The Skill name
	, const Yars::Skill& skill                ///< The Skill to add
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(skill_name.empty())
	{
		return false;
	}

	if(skillExists(skill_name))
	{
		return false;
	}
#endif

	for(const auto& iter : skill.attribute_contribution)
	{
		const std::string& attribute_name        = iter.first;
		//const int32_t&   modifier_contribution = iter.second;

		attributeAdd(attribute_name);
	}

	skill_map[skill_name] = skill;

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Skill
 *
 * \brief Remove a Skill
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Foo", {});
 * // Do stuff
 * yars.skillRemove("Foo");
 * \endexample
 *
 * \retval true  The Skill was removed
 * \retval false Failed to remove the Skill
 */
bool Yars::skillRemove(const std::string& skill_name ///< The Skill name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(skill_name.empty())
	{
		return false;
	}

	if(skillExists(skill_name) == false)
	{
		return false;
	}
#endif

	skill_map.erase(skill_name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 *
 * \ingroup group_Skill
 *
 * \brief Check if an Skill already exists
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill");
 * if(yars.skillExists("Skill"))
 * {
 * 	std::cout << "Woot!\n";
 * }
 * \endexample
 *
 * \retval true  The Skill was added
 * \retval false Failed to add the Skill
 */
bool Yars::skillExists(const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(skill_name.empty())
	{
		return false;
	}
#endif

	return skill_map.contains(skill_name);
}


/**
 * \ingroup group_Skill
 *
 * \brief Get a list of Skills.
 *
 * Provides a vector of all the Skill names in the Yars class.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * Yars::VectorString skill_list = yars.skillList();
 * \endexample
 *
 * \return A vector of Skill names.
 */
Yars::VectorString Yars::skillList() const noexcept
{
	return Yars::mapKeyList(skill_map);
}

// }}}
// {{{ Item

/**
 * \todo{ExtendedData} Add data support, like what Character has
 *
 * \todo{InventoryRewrite} Re-evaluate the need for a "global" item list
 * 
 * \todo{InventoryRewrite} All items added to a character must have a unique Id
 * 
 * \todo{InventoryRewrite} Add "Policy" support.  Each policy would be 
 * responsible for a single aspect of inventory management.  The following are 
 * the currently planned "Policies"
 * - Stacking
 *   - Maximum count (global) is configurable
 *   - Maximum count (per item) is configurable via ExtendedData
 * - Weight
 *   - Each Item is given an abstract weight via ExtendedData
 *   - Maximum total weight is configurable
 * - Packing 2D
 *   - Each Item is given a cell-pattern which must fit onto a grid via 
 *   ExtendedData
 *
 * \defgroup group_Item Item
 *
 * \brief Item
 *
 * Items are the most complicated aspect of \yars because they:
 * - Provide modifiers to health, attributes, and skills when equipped
 * - Can only be used with certain skills by an attacker
 * - Can only be used with certain skills by a defender
 *
 * The fields of Item are:
 *
 * - \header{Item.count}
 *   The number of items in the stack
 *   <br><br>
 * - \header{Item.affect_attribute}
 * - \header{Item.affect_skill}
 * - \header{Item.affect_health_current}
 * - \header{Item.affect_health_maximum}
 *   These values determine what an Affect will do when an Affect is created 
 *   using this Item.
 *   <br><br>
 * - \header{Item.modify_attribute}
 * - \header{Item.modify_skill}
 * - \header{Item.modify_health_current}
 * - \header{Item.modify_health_maximum}
 *   When an Item is equipped, these values will be applied to the Character.
 *   The modifiers will be removed once the Item is unequipped.
 *   <br><br>
 * - \header{Item.skill_attacker}
 *   A list of Skills that allow the Item to be used.
 *   <br><br>
 * - \header{Item.skill_defender}
 *   A list of Skills that allow the Item to be defended against.
 *
 * When an Item is added to a Character, it will just be in their inventory.
 * However, when a Character equips an Item from their inventory, the 
 * `modify_*` data will be applied to that Character.
 *
 * To use an Item, the Attacker must have a compatible Skill in 
 * `skill_attacker`.  For a Character to defend against an Item, they must have 
 * a compatible Skill in `skill_defender`.
 *
 * Affects created from Items will use the `affect_*` data items.
 *
 * Fortunately, all of this complication only applies to the creation of an 
 * Item.  The \yars game mechanics will use the appropriate data in the Item 
 * automatically.
 *
 * \anchor feature_InventoryRewrite
 * \par The Inventory Rewrite
 *
 * All items will be located in the Yars class.  When an Item is added to a 
 * character, that Item is actually copied in the Yars class and the character 
 * is given a handle (the vector index) to the copied Item.  If the character 
 * "restacks" the Items which results in Items no longer being valid, those 
 * Item indexes will be returned to the Yars class
 *
 * Character Equipment Items will be kept separate from Inventory Items.
 *
 * The Item will be split into 4 parts, where each part is a separate vector.  
 * A single index will be used by the 4 vectors to "combine/merge" the Item 
 * into a cohesive whole.  The following named parts are grouped by access 
 * patterns:
 * - Data
 *   - name
 *   - custom data
 * - Triggers
 *   - skill_attacker
 * - Affect Properties
 *   - affect_attribute
 *   - affect_skill
 *   - affect_health_current
 *   - affect_health_maximum
 *   - skill_defender
 * - Equipped Properties
 *   - modify_attribute
 *   - modify_skill
 *   - modify_health_current
 *   - modify_health_maximum
 *
 * \anchor feature_InventoryPolicy
 * \par Inventory Policies
 *
 * Describe the intent of this feature.
 */

/**
 * \ingroup group_Item
 *
 * \struct Yars::Item
 *
 * \brief Item data
 */

/**
 * \var Yars::Item::affect_attribute
 *
 * \brief Attributes that will have an Affect.
 *
 * When an Affect is created, that Affect can modify the values of Attributes.  
 * This container holds the Attribute name and how much it will modify a 
 * Character's attribute.
 */

/**
 * \var Yars::Item::affect_skill
 *
 * \brief Skills that will have an Affect.
 *
 * When an Affect is created, that Affect can modify the values of Skills.  
 * This container holds the Skill name and how much it will modify a 
 * Character's skill.
 */

/**
 * \var Yars::Item::affect_health_current
 *
 * \brief The Affect on Health.
 *
 * An Affect created from an Item can change a Character's current health.  
 * This value will be added to the Character's current health.
 */

/**
 * \var Yars::Item::affect_health_maximum
 *
 * \brief The Affect on Health.
 *
 * An Affect created from an Item can change a Character's maximum health.  
 * This value will be added to the Character's maximum health.
 */

/**
 * \var Yars::Item::count
 *
 * \brief The number of items in the stack.
 */

/**
 * \var Yars::Item::modify_attribute
 *
 * \brief Attribute changes when equipped.
 *
 * When an Item is equipped, it can change Attribute values.  This container 
 * contains the names of the Attributes to modify and by how much.
 */

/**
 * \var Yars::Item::modify_skill
 *
 * \brief Skill changes when equipped.
 *
 * When an Item is equipped, it can change Skill values.  This container 
 * contains the names of the Skills to modify and by how much.
 */

/**
 * \var Yars::Item::modify_health_current
 *
 * \brief Health changes when equipped.
 *
 * When an Item is equipped, this value will change current health value.  
 */

/**
 * \var Yars::Item::modify_health_maximum
 *
 * \brief Health changes when equipped.
 *
 * When an Item is equipped, this value will change maximum health value.  
 */

/**
 * \var Yars::Item::skill_attacker
 *
 * \brief Required skills to use this Item.
 *
 * In order for a Character to use this Item, they must have one of the Skills 
 * in this list.  If not, they can't use this Item.
 */

/**
 * \var Yars::Item::skill_defender
 *
 * \brief Required skills to defend against this Item.
 *
 * After an Affect has been created from this Item, a Character must have one 
 * of the Skills in this list to be able to defend against the Affect.
 */

/**
 * \ingroup group_Item
 *
 * \typedef Yars::MapStringItem
 *
 * \brief A convenience type.
 */

/**
 * \ingroup group_Item
 *
 * \typedef Yars::MultimapStringItem
 *
 * \brief A convenience type.
 */

/**
 * \ingroup group_Item
 *
 * \typedef Yars::MapIdMapStringItem
 *
 * \brief A convenience type.
 */

/**
 * \ingroup group_Item
 *
 * \typedef Yars::MapIdMultimapStringItem
 *
 * \brief A convenience type.
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Item to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Item& item ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"count\": "                 << std::to_string( item.count)
		<< ", \"affect_attribute\": "      << Yars::to_string(item.affect_attribute)
		<< ", \"affect_skill\": "          << Yars::to_string(item.affect_skill)
		<< ", \"modify_attribute\": "      << Yars::to_string(item.modify_attribute)
		<< ", \"modify_skill\": "          << Yars::to_string(item.modify_skill)
		<< ", \"skill_attacker\": "        << Yars::to_string(item.skill_attacker)
		<< ", \"skill_defender\": "        << Yars::to_string(item.skill_defender)
		<< ", \"affect_health_current\": " << std::to_string( item.affect_health_current)
		<< ", \"affect_health_maximum\": " << std::to_string( item.affect_health_maximum)
		<< ", \"modify_health_current\": " << std::to_string( item.modify_health_current)
		<< ", \"modify_health_maximum\": " << std::to_string( item.modify_health_maximum)
		<< " }"
		;
	
	return oss.str();
}
#endif


#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::MapStringItem to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::MapStringItem& item_map ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss << "{";

	bool first = true;
	for(const auto& iter : item_map)
	{
		if(first == false)
		{
			oss << ",";
		}

		const std::string& name = iter.first;
		const Yars::Item&  item = iter.second;

		oss	<< " \""
			<< name
			<< "\": "
			<< Yars::to_string(item)
			;

		first = false;
	}

	oss << " }";
	
	return oss.str();
}
#endif


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Access an Item.
 *
 * Calling this method will provide access to an Item with a matching \p 
 * item_name.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * Yars::Item& item = yars.item("Item");
 * \endexample
 *
 * \return A reference to an Item.
 */
Yars::Item& Yars::item(const std::string& item_name ///< The name of the Item
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		Item* retval = nullptr;
		return *retval;
	}

	if(itemExists(item_name) == false)
	{
		//ZAKERO_YARS_DEBUG << "Already Exists" << std::endl;
		Item* retval = nullptr;
		return *retval;
	}
#endif

	return item_map[item_name];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Access an Item.
 *
 * Calling this method will provide access to an Item with a matching \p 
 * item_name.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * const Yars& y = yars;
 * const Yars::Item& item = y.item("Item");
 * \endexample
 *
 * \return A reference to an Item.
 */
const Yars::Item& Yars::item(const std::string& item_name ///< The name of the Item
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		//ZAKERO_YARS_DEBUG << "Bad Name" << std::endl;
		Item* retval = nullptr;
		return *retval;
	}

	if(itemExists(item_name) == false)
	{
		//ZAKERO_YARS_DEBUG << "Already Exists" << std::endl;
		Item* retval = nullptr;
		return *retval;
	}
#endif

	return item_map.at(item_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Item
 *
 * \brief Add a new Item.
 *
 * Add a new Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Foo", {});
 * yars.itemAdd("Bar",
 * 	{	.affect_attribute =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	,	.affect_skill =
 * 		{	{ "Skill_1", 1 }
 * 		,	{ "Skill_2", 1 }
 * 		}
 * 	,	.modify_attribute =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	,	.modify_skill =
 * 		{	{ "Skill_1", 1 }
 * 		,	{ "Skill_2", 1 }
 * 		}
 * 	,	.skill_attacker = { "Skill_1", "Skill_3" }
 * 	,	.skill_defender = { "Skill_2", "Skill_4" }
 *      ,	.count = 1
 *      ,	.affect_health_current = -5
 *      ,	.affect_health_maximum =  0
 *      ,	.modify_health_current =  0
 *      ,	.modify_health_maximum = 10
 *      });
 * \endexample
 *
 * \retval true  The Item was added
 * \retval false Failed to add the Item
 */
bool Yars::itemAdd(const std::string& item_name ///< The name of the Item
	, const Yars::Item& item                ///< The Item data
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}

	if(itemExists(item_name))
	{
		return false;
	}
#endif

	item_map[item_name] = item;

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Item
 *
 * \brief Remove an Item.
 *
 * Remove an Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * // Do stuff
 * yars.itemRemove("Item");
 * \endexample
 *
 * \retval true  The Item was removed
 * \retval false Failed to remove the Item
 */
bool Yars::itemRemove(const std::string& item_name ///< The name of the Item
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}

	if(itemExists(item_name) == false)
	{
		return false;
	}
#endif

	item_map.erase(item_name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Check if an Item exists
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item");
 * if(yars.itemExists("Item"))
 * {
 * 	std::cout << "Woot!\n";
 * }
 * \endexample
 *
 * \retval true  The Item was removed
 * \retval false Failed to remove the Item
 */
bool Yars::itemExists(const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}
#endif

	return item_map.contains(item_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Check if an Item is usable
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item");
 * if(yars.itemExists("Item"))
 * {
 * 	std::cout << "Woot!\n";
 * }
 * \endexample
 *
 * \retval true  The Item was removed
 * \retval false Failed to remove the Item
 */
bool Yars::itemIsUsable(const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}

	if(itemExists(item_name) == false)
	{
		return false;
	}
#endif

	const Yars::Item& item = item_map.at(item_name);

	bool is_usable = (item.skill_attacker.size() > 0)
 		|| (item.skill_defender.size() > 0)
		;

	return is_usable;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Check if an attacker can use the Item
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * if(yars.itemIsUsableByAttacker("Item") == false)
 * {
 * 	std::cout << "Its useless\n";
 * }
 * \endexample
 *
 * \retval true  The Item can be used by an attacker
 * \retval false An attacker can not use the Item
 */
bool Yars::itemIsUsableByAttacker(const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}

	if(itemExists(item_name) == false)
	{
		return false;
	}
#endif

	const Yars::Item& item = item_map.at(item_name);

	bool is_usable = (item.skill_attacker.size() > 0);

	return is_usable;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Item
 *
 * \brief Check if an defender can use the Item
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * if(yars.itemIsUsableByDefender("Item") == false)
 * {
 * 	std::cout << "Its useless\n";
 * }
 * \endexample
 *
 * \retval true  The Item can be used by an defender
 * \retval false An defender can not use the Item
 */
bool Yars::itemIsUsableByDefender(const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(item_name.empty())
	{
		return false;
	}

	if(itemExists(item_name) == false)
	{
		return false;
	}
#endif

	const Yars::Item& item = item_map.at(item_name);

	bool is_usable = (item.skill_defender.size() > 0);

	return is_usable;
}


/**
 * \ingroup group_Item
 *
 * \brief Get a list of Items
 *
 * This method will provide a list of all the Item names that are currently in 
 * \yars.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 * Yars::VectorString list = yars.itemList();
 * \endexample
 *
 * \return A vector of Item names
 */
Yars::VectorString Yars::itemList() const noexcept
{
	return Yars::mapKeyList(item_map);
}

// }}}
// {{{ Character

/**
 * \defgroup group_Character Character
 * 
 * \brief A Character
 *
 * A character in \yars is more than just a PC (Player Character) or a NPC 
 * (Non-Player Character).  Anything that can be interacted with is considered 
 * a Character.
 *
 * In addition to having a collection of Items and Skill, a Character has 
 * Health to track it physical state.  Characters also have support for 
 * free-form data that can be added and changed when ever necessary to enable 
 * the Character be used in unforeseen ways.
 *
 * An example would be a PC attempting to lock-pick a safe. The PC interacts 
 * with the lock with a lock-pick skill. It is this interaction that makes the 
 * `lock` is a Character (a "Lock Character"). The "Lock Character" could have 
 * addition information such as the "Make and Model" or "Is Broken". As Once 
 * the "Lock Character" has been defeated by the PC, the PC can "trade" with 
 * the "Safe Character" to get the Items.
 *
 * Since Characters can do many things, this section has the largest 
 * collections of methods in all of \yars.
 */

/**
 * \deprecated{InventoryPolicy}
 *
 * \struct Yars::CharacterConfig
 *
 * \brief Configuration data for each Character
 *
 * The data in this structure will be replaced by the future Inventory Policy 
 * objects.
 */

/**
 * \deprecated{InventoryPolicy}
 *
 * \typedef Yars::MapIdCharacterConfig
 *
 * \brief A container.
 */

// {{{ Character: Util

/**
 * \ingroup group_Character
 *
 * \brief Create a new Character
 *
 * Create a new Character.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * \endexample
 *
 * \return A Character ID
 */
Yars::Id Yars::characterCreate() noexcept
{
	Yars::Id character_id;

	if(character_pool.empty())
	{
		character_id = this->character_id_max;
		this->character_id_max++;
	}
	else
	{
#if defined (ZAKERO_YARS_ENABLE_ID_SHUFFLE)
		std::random_shuffle(std::begin(character_pool)
			, std::end(character_pool)
			);
#endif

		character_id = character_pool.back();
		character_pool.pop_back();
	}

	characterClear(character_id);

	return character_id;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Character
 *
 * \brief Delete a Character
 *
 * Delete the request Character.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * // Do stuff
 * yars.characterDelete(bubba);
 * \endexample
 *
 * \retval true  The Character was deleted
 * \retval false Failed to delete the Character
 */
bool Yars::characterDelete(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

#if defined (ZAKERO_YARS_ENABLE_CLEAR_ON_DELETE)
	characterClear(character_id);
#endif

	character_pool.push_back(character_id);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Character
 *
 * \brief Check if a Character exists
 *
 * Check if a Character exists with the provided Id.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * if(yars.characterExists(bubba))
 * {
 * 	std::cout << "Hey, I found Bubba!\n";
 * }
 * \endexample
 *
 * \retval true  The Character was found
 * \retval false The Character does not exists
 */
bool Yars::characterExists(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
	if(character_id >= this->character_id_max)
	{
		return false;
	}

	return (Yars::vectorContains(character_pool, character_id) == false);
}


/**
 * \ingroup group_Character
 *
 * \brief Delete Character data
 *
 * All data associated with the specified Character Id is cleared.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterClear(bubba);
 * \endexample
 */
void Yars::characterClear(const Yars::Id& character_id ///< The Character Id
	) noexcept
{
	if(character_affect.contains(character_id))
	{
		character_affect.erase(character_id);
	}

	character_attribute[character_id]      = {};
	character_config[character_id]         = {};
	character_data_bool[character_id]      = {};
	character_data_float[character_id]     = {};
	character_data_int32[character_id]     = {};
	character_data_string[character_id]    = {};
	character_equipment_item[character_id] = {};
	character_equipment_name[character_id] = {};
	character_health[character_id]         = {};
	character_item[character_id]           = {};
	character_skill[character_id]          = {};
}


/**
 * \deprecated{InventoryPolicy}
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Character
 *
 * \brief Change the staking value
 *
 * Control the maximum number of Item that can be stacked.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterConfigItemStackMax(bubba, 10);
 * \endexample
 *
 * \retval true  Configuration has been updated
 * \retval false Failed to update the configuration
 */
bool Yars::characterConfigItemStackMax(const Yars::Id character_id ///< The Character Id
	, const uint32_t item_stack_max ///< New stack max value
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(item_stack_max == 0)
	{
		return false;
	}
#endif

	character_config[character_id].item_stack_max = item_stack_max;

	return true;
}

// }}}
// {{{ Character: Health

/**
 * \ingroup group_Character
 *
 * \defgroup group_CharacterHealth Health
 *
 * \brief A Character's Health.
 *
 * Every Character has Health.  The following methods allow you to access the 
 * Health object.
 *
 * \see \ref group_Health
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterHealth
 *
 * \brief Get the Character's Health
 *
 * Access the Health data of a Character.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * Yars::Health& health = yars.characterHealth(bubba);
 * \endexample
 *
 * \return The Character's Health
 */
Yars::Health& Yars::characterHealth(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Health* retval = nullptr;
		return *retval;
	}
#endif

	return character_health[character_id];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterHealth
 *
 * \brief Get the Character's Health
 *
 * Access the Health data of a Character.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 *
 * const Yars& y = yars;
 * const Yars::Health& health = y.characterHealth(bubba);
 * \endexample
 *
 * \return The Character's Health
 */
const Yars::Health& Yars::characterHealth(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Health* retval = nullptr;
		return *retval;
	}
#endif

	return character_health.at(character_id);
}

// }}}
// {{{ Character: Attribute

/**
 * \ingroup group_Character
 *
 * \defgroup group_CharacterAttribute Attribute
 *
 * \brief A Character's Attributes.
 *
 * A Character has no control over the Attributes that they have but Attribute 
 * can have an affect on everything they do.  Attributes are acquired as Skills 
 * are learned.  And even if a Skill is lost to the Character, the Attributes 
 * from that Skill will remain.
 *
 * This group of methods allow you to access the Health object.
 *
 * \see \ref group_Attribute
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterAttribute
 *
 * \brief Access an Attribute
 *
 * Provides a way to access a Character's Attribute.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * Yars::Attribute& attribute = yars.characterAttribute(bubba, "Attr_1");
 * \endexample
 *
 * \return A reference to an Attribute
 */
Yars::Attribute& Yars::characterAttribute(const Yars::Id character_id ///< The Character Id
	, const std::string& attribute_name ///< The Attribute Name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Attribute* retval = nullptr;
		return *retval;
	}

	if(characterAttributeExists(character_id, attribute_name) == false)
	{
		Yars::Attribute* retval = nullptr;
		return *retval;
	}
#endif

	return character_attribute[character_id][attribute_name];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterAttribute
 *
 * \brief Access an Attribute
 *
 * Provides a way to access a Character's Attribute.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * const Yars& y = yars;
 * const Yars::Attribute& attribute = y.characterAttribute(bubba, "Attr_1");
 * \endexample
 *
 * \return A reference to an Attribute
 */
const Yars::Attribute& Yars::characterAttribute(const Yars::Id character_id ///< The Character Id
	, const std::string& attribute_name ///< The Attribute Name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Attribute* retval = nullptr;
		return *retval;
	}

	if(characterAttributeExists(character_id, attribute_name) == false)
	{
		Yars::Attribute* retval = nullptr;
		return *retval;
	}
#endif

	return character_attribute.at(character_id).at(attribute_name);
}


/**
 * \ingroup group_CharacterAttribute
 *
 * \brief Add an Attribute to a Character.
 *
 * Adds an Attribute to a Character if it does not already exists.
 *
 * \example
 * Yars yars;
 * yars.attributeAdd("Attribute");
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterAttributeAdd(bubba, "Attribute");
 * \endexample
 */
void Yars::characterAttributeAdd(const Yars::Id character_id ///< The Character Id
	, const std::string& attribute_name ///< The Attribute Name
	) noexcept
{
	if(characterAttributeExists(character_id, attribute_name))
	{
		return;
	}

	character_attribute[character_id][attribute_name] = attribute_map[attribute_name];

	return;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterAttribute
 *
 * \brief Check if a Character has an Attribute
 *
 * Check if a Character has an Attribute
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * if(yars.characterAttributeExists(bubba, "Attr_1"))
 * {
 * 	std::cout << "Bubba has Attr_1\n";
 * }
 * \endexample
 *
 * \retval true  The Character does have the Attribute.
 * \retval false The Character does not have the Attribute.
 */
bool Yars::characterAttributeExists(const Yars::Id character_id ///< The Character Id
	, const std::string& attribute_name ///< The Attribute Name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const auto& attribute_map = character_attribute.at(character_id);

	return attribute_map.contains(attribute_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterAttribute
 *
 * \brief Get all of the Character's Attributes
 *
 * Access to all of the Attributes a Character has will be provided in the 
 * returned map.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * Yars::MapStringAttribute& map = yars.charecterAttributeMap(bubba);
 * \endexample
 *
 * \return A reference to a map of Attributes
 */
Yars::MapStringAttribute& Yars::characterAttributeMap(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::MapStringAttribute* retval = nullptr;
		return *retval;
	}
#endif

	auto& attribute = character_attribute[character_id];

	return attribute;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterAttribute
 *
 * \brief Get all of the Character's Attributes
 *
 * Access to all of the Attributes a Character has will be provided in the 
 * returned map.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * 	{	.attribute_contribution =
 * 		{	{ "Attr_1", 1 }
 * 		,	{ "Attr_2", 1 }
 * 		}
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * const Yars& y = yars;
 * const Yars::MapStringAttribute& map = y.charecterAttributeMap(bubba);
 * \endexample
 *
 * \return A reference to a map of Attributes
 */
const Yars::MapStringAttribute& Yars::characterAttributeMap(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::MapStringAttribute* retval = nullptr;
		return *retval;
	}
#endif

	const auto& attribute = character_attribute.at(character_id);

	return attribute;
}


// }}}
// {{{ Character: Data

/**
 * \ingroup group_Character
 *
 * \defgroup group_CharacterData User Data
 *
 * \brief Custom User Data
 *
 * As a generic RPG Rule System, it is impossible to anticipate all the 
 * different kinds of data that will need to be associated with a Character.  
 * However, it is possible to predict the type of data and that is where this 
 * group of methods come into play.
 *
 * Using these methods, most types of data can be associated with a Character 
 * in a free-form manner.
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Clear all Character data.
 *
 * All custom user data will be removed from the specified Character.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataClear(bubba);
 * \endexample
 *
 * \retval true  All data has been cleared
 * \retval false Failed to clear the data
 */
bool Yars::characterDataClear(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	character_data_bool  [character_id].clear();
	character_data_float [character_id].clear();
	character_data_int32 [character_id].clear();
	character_data_string[character_id].clear();

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Remove Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "Height", 1.5f);
 * yars.characterDataSet(bubba, "Height", "m");
 * yars.characterDataSet(bubba, "Name",   "Bubba");
 * // Do Stuff
 * yars.characterDataErase(bubba, "Height");
 * // Both "Height"/1.5f and "Height"/"m" have been erased.
 * \endexample
 *
 * \retval true  The data has been erased
 * \retval false Failed to erase the data
 */
bool Yars::characterDataErase(const Yars::Id character_id ///< The Character Id
	, const std::string& name ///< The data name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	bool retval = false;

	{
		Yars::MapStringBool& data = character_data_bool.at(character_id);
		if(data.contains(name))
		{
			data.erase(name);
			retval = true;
		}
	}
	{
		Yars::MapStringFloat& data = character_data_float.at(character_id);
		if(data.contains(name))
		{
			data.erase(name);
			retval = true;
		}
	}
	{
		Yars::MapStringInt32& data = character_data_int32.at(character_id);
		if(data.contains(name))
		{
			data.erase(name);
			retval = true;
		}
	}
	{
		Yars::MapStringString& data = character_data_string.at(character_id);
		if(data.contains(name))
		{
			data.erase(name);
			retval = true;
		}
	}

	return retval;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Get Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", true);
 * // Do Stuff
 * bool value = false;
 * yars.characterDataGet(bubba, "key", value);
 * \endexample
 *
 * \retval true  The data was retrieved
 * \retval false Failed to retrieve the data
 */
bool Yars::characterDataGet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, bool&              value ///< The data value
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringBool& data = character_data_bool.at(character_id);
	if(data.contains(name) == false)
	{
		return false;
	}

	value = data.at(name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Get Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", 42.0f);
 * // Do Stuff
 * float value = 0.0f;
 * yars.characterDataGet(bubba, "key", value);
 * \endexample
 *
 * \retval true  The data was retrieved
 * \retval false Failed to retrieve the data
 */
bool Yars::characterDataGet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, float&             value ///< The data value
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringFloat& data = character_data_float.at(character_id);
	if(data.contains(name) == false)
	{
		return false;
	}

	value = data.at(name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Get Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", 42);
 * // Do Stuff
 * int32_t value = false;
 * yars.characterDataGet(bubba, "key", value);
 * \endexample
 *
 * \retval true  The data was retrieved
 * \retval false Failed to retrieve the data
 */
bool Yars::characterDataGet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, int32_t&           value ///< The data value
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringInt32& data = character_data_int32.at(character_id);
	if(data.contains(name) == false)
	{
		return false;
	}

	value = data.at(name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Get Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", "value");
 * // Do Stuff
 * std::string value = "";
 * yars.characterDataGet(bubba, "key", value);
 * \endexample
 *
 * \retval true  The data was retrieved
 * \retval false Failed to retrieve the data
 */
bool Yars::characterDataGet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, std::string&       value ///< The data value
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringString& data = character_data_string.at(character_id);
	if(data.contains(name) == false)
	{
		return false;
	}

	value = data.at(name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Set Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", true);
 * \endexample
 *
 * \retval true  The data was stored
 * \retval false Failed to store the data
 */
bool Yars::characterDataSet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, const bool         value ///< The data value
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	character_data_bool[character_id][name] = value;

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Set Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", 42.0f);
 * \endexample
 *
 * \retval true  The data was stored
 * \retval false Failed to store the data
 */
bool Yars::characterDataSet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, const float        value ///< The data value
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	character_data_float[character_id][name] = value;

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Set Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", 42);
 * \endexample
 *
 * \retval true  The data was stored
 * \retval false Failed to store the data
 */
bool Yars::characterDataSet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, const int32_t      value ///< The data value
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	character_data_int32[character_id][name] = value;

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterData
 *
 * \brief Set Character data.
 *
 * The named data will be removed from the Character regardless of the type of 
 * data.  The value will be placed in \p value but if the data name is not 
 * valid or the requested data type is not available, the original contents of 
 * the \p value will not be changed.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet(bubba, "key", "value");
 * \endexample
 *
 * \retval true  The data was stored
 * \retval false Failed to store the data
 */
bool Yars::characterDataSet(const Yars::Id character_id ///< The Character Id
	, const std::string& name  ///< The data name
	, const std::string& value ///< The data value
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	character_data_string[character_id][name] = value;

	return true;
}

// }}}
// {{{ Character: Skill

/**
 * \ingroup group_Character
 *
 * \defgroup group_CharacterSkill Skill
 *
 * \brief A Character's Skills
 *
 * Anything that a Character can do must be done using Skills.  When a 
 * Character gains Skills, those skills will guide how the Character evolves.
 *
 * \see \ref group_Skill
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Access a Skill
 *
 * Provides a way to access a Character's Skill.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * Yars::Skill& skill = yars.characterSkill(bubba, "Skill");
 * \endexample
 *
 * \return A reference to a Skill
 */
Yars::Skill& Yars::characterSkill(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Skill* retval = nullptr;
		return *retval;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		Yars::Skill* retval = nullptr;
		return *retval;
	}
#endif

	return character_skill[character_id][skill_name];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Access a Skill
 *
 * Provides a way to access a Character's Skill.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * const Yars& y = yars;
 * const Yars::Skill& skill = y.characterSkill(bubba, "Skill");
 * \endexample
 *
 * \return A reference to a Skill
 */
const Yars::Skill& Yars::characterSkill(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Skill* retval = nullptr;
		return *retval;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		Yars::Skill* retval = nullptr;
		return *retval;
	}
#endif

	return character_skill.at(character_id).at(skill_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Add a Skill to a Character.
 *
 * Adds a Skill to a Character if it does not already exists.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill");
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * \endexample
 *
 * \retval true  The Skill was added
 * \retval false Failed to add the Skill
 */
bool Yars::characterSkillAdd(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(skillExists(skill_name) == false)
	{
		return false;
	}

	if(characterSkillExists(character_id, skill_name) == true)
	{
		return false;
	}
#endif

	Yars::MapStringSkill& skill = character_skill[character_id];
	skill[skill_name] = skill_map[skill_name];

	for(const auto& iter : skill[skill_name].attribute_contribution)
	{
		const std::string& attribute_name        = iter.first;
		//const int32_t&   modifier_contirbution = iter.second;

		characterAttributeAdd(character_id, attribute_name);
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Remove a Skill from a Character.
 *
 * Remove a Skill from a Character if it exists.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill");
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * \endexample
 *
 * \retval true  The Skill was removed
 * \retval false Failed to remove the Skill
 */
bool Yars::characterSkillRemove(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		return false;
	}
#endif

	Yars::MapStringSkill& skill = character_skill[character_id];
	skill.erase(skill_name);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Increase the Skill Level.
 *
 * As a Character uses a Skill, that Skill Level will eventually increase.  
 * This method give the Skill that opportunity.  Calling this method is not 
 * necessary because it will be automatically invoked when needed.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * yars.characterSkillIncrease(bubba, "Skill", 0.25f);
 * \endexample
 *
 * \retval true  The Skill increased in a Level
 * \retval false The Skill increase was not enough to increase the Level
 */
bool Yars::characterSkillIncrease(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	, const float        amount     ///< How much to increase the Skill
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		return false;
	}
#endif

	Yars::MapStringSkill& skill = character_skill[character_id];

	levelIncrease(skill[skill_name].level, amount);

	// If skill level increased, Event: Level_Up, character_id skill_name

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Check if a Character has a Skill
 *
 * Check if a Character has a Skill
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * if(yars.characterSkillExists(bubba, "Skill"))
 * {
 * 	std::cout << "Bubba has Skill\n";
 * }
 * \endexample
 *
 * \retval true  The Character does have the Skill.
 * \retval false The Character does not have the Skill.
 */
bool Yars::characterSkillExists(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringSkill& skill_map = character_skill.at(character_id);

	return skill_map.contains(skill_name);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Get a list of Skills.
 *
 * All the Skill names of the Character will be collected and returned.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * Yars::VectorString list = yars.characterSkillList(bubba);
 * \endexample
 *
 * \return A vector of Skill names
 */
Yars::VectorString Yars::characterSkillList(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringSkill& skill_map = characterSkillMap(character_id);

	return Yars::mapKeyList(skill_map);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Get a map of Skills.
 *
 * All the Skills of the Character will be accessible via the returned map.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * Yars::MapStringSkill& list = yars.characterSkillMap(bubba);
 * \endexample
 *
 * \return A map of Skills
 */
Yars::MapStringSkill& Yars::characterSkillMap(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::MapStringSkill* retval = nullptr;
		return *retval;
	}
#endif

	auto& skill = character_skill[character_id];

	return skill;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterSkill
 *
 * \brief Get a map of Skills.
 *
 * All the Skills of the Character will be accessible via the returned map.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * const Yars& y = yars;
 * const Yars::MapStringSkill& list = y.characterSkillMap(bubba);
 * \endexample
 *
 * \return A map of Skills
 */
const Yars::MapStringSkill& Yars::characterSkillMap(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::MapStringSkill* retval = nullptr;
		return *retval;
	}
#endif

	const auto& skill = character_skill.at(character_id);

	return skill;
}


// }}}
// {{{ Character: Item

/**
 * \todo{InventoryRewrite} Should Items be able to contain other Items?
 * - No. Use the custom data to define the "container".
 *
 * \todo{InventoryRewrite} The description of the Character Item is not 
 * supported by the code.  AffectInit() allows for any usable Item.  This can 
 * be fixed by adding `characterItemUse()` and characterEquipmentUse()`.
 *
 * \todo{InventoryRewrite} Item is broken.  Consider the following approaches:
 * - Items should contain their own name
 * - Items have their own "data"
 * - Items match only if all "data" matches
 *   - What about `count`?
 * - Missing "familiarity" by the user of the Item
 *   - Could this be handled by `Custom Data`?
 * - Add Policy controller support
 *   - Policy Stacking
 *     - Set Default Stacking value
 *     - Data Key: "Yars.ItemPolicy.Stacking.MaxCount"
 *   - Policy Weight
 *   - Policy Packing
 *   - Policy uses data "Yars.ItemPolicy.Stacking.KEY"
 *
 * \ingroup group_Character
 *
 * \defgroup group_CharacterItem Item
 *
 * \brief The Character's Items.
 *
 * Items allow a Character to use their Skills.  A Character must have the 
 * Skill to use Item as well as having that Item in their possession.  The 
 * outcome of using an Item is in Affect is created.  The Affect can then be 
 * applied to a Character.
 *
 * \see \ref group_Affect
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Access a Character's Item.
 *
 * Provides access to an Item that the Character possesses.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * Yars::Item& item = yars.characterItem(bubba, "Item");
 * \endexample
 *
 * \return A reference to an Item.
 */
Yars::Item& Yars::characterItem(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, const uint32_t     offset    ///< Which Item to access
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}

	if(character_item.at(character_id).count(item_name) <= offset)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}
#endif

	auto range = character_item[character_id].equal_range(item_name);

	auto iter = range.first;
	std::advance(iter, offset);

	return iter->second;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Access a Character's Item.
 *
 * Provides access to an Item that the Character possesses.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * const Yars& y = yars;
 * const Yars::Item& item = y.characterItem(bubba, "Item");
 * \endexample
 *
 * \return A reference to an Item.
 */
const Yars::Item& Yars::characterItem(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, const uint32_t     offset    ///< Which Item to access
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}

	if(character_item.at(character_id).count(item_name) <= offset)
	{
		Yars::Item* retval = nullptr;
		return *retval;
	}
#endif

	auto range = character_item.at(character_id).equal_range(item_name);

	auto iter = range.first;
	std::advance(iter, offset);

	return iter->second;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterItem
 *
 * \brief Add an Item to a Character
 *
 * The specified Item will be added to the Character.  Optionally, the number 
 * of Items can also be provided.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 * \endexample
 *
 * \retval true  The Item was added
 * \retval false Failed to add the Item
 */
bool Yars::characterItemAdd(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, const uint32_t     amount    ///< The number of Items to add
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(itemExists(item_name) == false)
	{
		return false;
	}

	if(amount == 0)
	{
		return false;
	}
#endif

	Yars::MultimapStringItem& inventory = character_item[character_id];
	const auto                range     = inventory.equal_range(item_name);
	const uint32_t            count_max = character_config[character_id].item_stack_max;
	uint32_t                  remaining = amount;

	for(auto iter = range.first; iter != range.second; iter++)
	{
		Yars::Item& new_item = iter->second;

		if(new_item.count >= count_max)
		{
			continue;
		}

		uint32_t count = count_max - new_item.count;

		if(count >= remaining)
		{
			new_item.count += std::exchange(remaining, 0);
			break;
		}

		new_item.count = count_max;
		remaining -= count;
	}

	const Yars::Item& new_item = this->item(item_name);

	while(remaining > 0)
	{
		auto iter = inventory.insert({item_name, new_item});
		Yars::Item& item = iter->second;

		if(remaining <= count_max)
		{
			item.count = std::exchange(remaining, 0);
			break;
		}

		item.count = count_max;
		remaining -= count_max;
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterItem
 *
 * \brief Add an Item to a Character
 *
 * \note This method is private, no there are no sanity checks.
 *
 * \retval true  The Item was added
 * \retval false Failed to add the Item
 */
bool Yars::characterItemAdd(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, const Yars::Item&  new_item  ///< A reference to the Item
	) noexcept
{
	Yars::MultimapStringItem& inventory = character_item[character_id];
	auto                      range     = inventory.equal_range(item_name);
	const uint32_t            count_max = character_config[character_id].item_stack_max;

	bool was_added = false;
	for(auto& iter = range.first; iter != range.second; iter++)
	{
		Yars::Item& item = iter->second;

		if(item.count < count_max)
		{
			iter->second.count++;
			was_added = true;
			break;
		}
	}
	
	if(was_added == false)
	{
		inventory.emplace(item_name, new_item);
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterItem
 *
 * \brief Remove an Item from a Character
 *
 * The specified Item will be removed from the Character.  If an amount is 
 * specified then that many Items will be removed, if possible.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 * // Do Stuff
 * yars.characterItemRemove(bubba, "Item");
 * \endexample
 *
 * \retval true  The Item was removed
 * \retval false Failed to remove the Item
 */
bool Yars::characterItemRemove(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, const uint32_t     amount    ///< The number of Items to remove
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	Yars::MultimapStringItem& inventory        = character_item[character_id];
	auto                      range            = inventory.equal_range(item_name);
	uint32_t                  amount_to_remove = amount;

	auto iter = range.first;
	while(iter != range.second)
	{
		Yars::Item& item = iter->second;

		if(item.count > amount_to_remove)
		{
			item.count -= amount_to_remove;

			break;
		}

		amount_to_remove -= item.count;

		iter = inventory.erase(iter);
	}

	return true;
}


/**
 * \todo{InventoryRewrite} Should this method be private?
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Remove an Item from a Character
 *
 * The specified Item will be removed from the Character.  This method differs 
 * from Yars::characterItemRemove() by returning a copy of the removed Item.  
 * And only one Item can be taken at a time.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 * // Do Stuff
 * yars.characterItemRemove(bubba, "Item");
 * \endexample
 *
 * \retval true  The Item was removed
 * \retval false Failed to remove the Item
 */
Yars::Item Yars::characterItemTake(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return {};
	}
#endif

	Yars::MultimapStringItem& inventory = character_item[character_id];

	const auto iter = inventory.find(item_name);

	Yars::Item& item = iter->second;

	Yars::Item taken_item = item;
	taken_item.count = 1;

	if(item.count == 1)
	{
		inventory.erase(iter);
	}
	else
	{
		item.count--;
	}

	return taken_item;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if a Character has an Item
 *
 * Find out if a Character has the requested Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemExists(bubba, "Item"))
 * {
 * 	std::cout << "Bubba has an Item!\n";
 * }
 * \endexample
 *
 * \retval true  The Character does have the Item
 * \retval false The Character does not have the Item.
 */
bool Yars::characterItemExists(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	if(inventory.contains(item_name))
	{
		return true;
	}

	return false;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Get the number of Items
 *
 * Get a count of the number if instances of the specified Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item", 100);
 *
 * uint32_t count = yars.characterItemCount(bubba, "Item");
 * \endexample
 *
 * \return The number of Items
 */
uint32_t Yars::characterItemCount(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return 0;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return 0;
	}
#endif

	uint32_t stacks = 0;

	return this->characterItemCount(character_id, item_name, stacks);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Get the number of Items
 *
 * Get a count of the number if instances of the specified Item.  This method 
 * will also provide the number of Item stacks.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item", 100);
 *
 * uint32_t stacks = 0;
 * uint32_t count = yars.characterItemCount(bubba, "Item", stacks);
 * \endexample
 *
 * \return The number of Items
 */
uint32_t Yars::characterItemCount(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	, uint32_t&          stacks    ///< Store the number of Item stacks
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return 0;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return 0;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);
	auto                            range     = inventory.equal_range(item_name);

	uint32_t item_count = 0;
	stacks = 0;
	for(auto iter = range.first; iter != range.second; iter++)
	{
		const Yars::Item& item = iter->second;
		item_count += item.count;
		stacks++;
	}

	return item_count;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Get a list of Item names
 *
 * Calling this method will provide a list of all the Item names that the 
 * Character currently has.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * Yars::VectorString list = yars.characterItemList(bubba);
 * \endexample
 *
 * \return A vector of Item names.
 */
Yars::VectorString Yars::characterItemList(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}
#endif

	const Yars::MultimapStringItem& item_map = character_item.at(character_id);
	return Yars::mapKeyList(item_map);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsable(bubba, "Item"))
 * {
 * 	std::cout << "Bubba can use Item!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsable()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsable(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	auto iter = inventory.find(item_name);
	const Yars::Item& item = iter->second;

	if(this->characterItemIsUsableByAttacker(character_id, item) == true)
	{
		return true;
	}

	if(this->characterItemIsUsableByDefender(character_id, item) == true)
	{
		return true;
	}

	return false;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item as an 
 * "Attacker".
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByAttacker(bubba, "Item"))
 * {
 * 	std::cout << "Bubba can use Item as an Attacker!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByAttacker()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByAttacker(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	auto iter = inventory.find(item_name);
	const Yars::Item& item = iter->second;

	return characterItemIsUsableByAttacker(character_id, item);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item with 
 * the specified Skill as an "Attacker".
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByAttacker(bubba, "Item", "Skill"))
 * {
 * 	std::cout << "Bubba can use Item as an Attacker!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByAttacker()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByAttacker(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name  ///< The Item name
	, const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	auto iter = inventory.find(item_name);
	const Yars::Item& item = iter->second;

	if(Yars::vectorContains(item.skill_attacker, skill_name) == false)
	{
		return false;
	}

	return true;
}


/**
 * \todo{InventoryRewrite} Should this method be private?
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByAttacker(bubba, yars.characterItem("Item")))
 * {
 * 	std::cout << "Bubba can use Item as an Attacker!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByAttacker()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByAttacker(const Yars::Id character_id ///< The Character Id
	, const Yars::Item& item ///< The Item
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringSkill& skill_map = character_skill.at(character_id);

	for(const auto& skill_name : item.skill_attacker)
	{
		if(skill_map.contains(skill_name))
		{
			return true;
		}
	}

	return false;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item as an 
 * "Defender".
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByDefender(bubba, "Item"))
 * {
 * 	std::cout << "Bubba can use Item as a Defender!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByDefender()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByDefender(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	auto iter = inventory.find(item_name);
	const Yars::Item& item = iter->second;

	return characterItemIsUsableByDefender(character_id, item);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item with 
 * the specified Skill as an "Defender".
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByDefender(bubba, "Item", "Skill"))
 * {
 * 	std::cout << "Bubba can use Item as a Defender!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByDefender()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByDefender(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name  ///< The Item name
	, const std::string& skill_name ///< The Skill name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	const Yars::MultimapStringItem& inventory = character_item.at(character_id);

	auto iter = inventory.find(item_name);
	const Yars::Item& item = iter->second;

	if(Yars::vectorContains(item.skill_defender, skill_name) == false)
	{
		return false;
	}

	return true;
}


/**
 * \todo{InventoryRewrite} Should this method be private?
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Check if an Item is usable.
 *
 * This method will determine if a Character has use the requested Item.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * if(yars.characterItemIsUsableByDefender(bubba, yars.characterItem("Item")))
 * {
 * 	std::cout << "Bubba can use Item as a Defender!\n";
 * }
 * \endexample
 *
 * \see Yars::itemIsUsableByDefender()
 *
 * \retval true  The Item is usable
 * \retval false The Item is not usable
 */
bool Yars::characterItemIsUsableByDefender(const Yars::Id character_id ///< The Character Id
	, const Yars::Item& item ///< The Item
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringSkill& skill_map = character_skill.at(character_id);

	for(const auto& skill_name : item.skill_defender)
	{
		if(skill_map.contains(skill_name))
		{
			return true;
		}
	}

	return false;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterItem
 *
 * \brief Restack all Items
 *
 * All the Items that the Character has will be restacked in the most optimum 
 * way possible.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item", 20);
 * yars.characterItemAdd(bubba, "Item", 40);
 * yars.characterItemRestack(bubba);
 * \endexample
 *
 * \retval true  Successfully restacked the Items
 * \retval false Failed to restack the Items
 */
bool Yars::characterItemRestack(const Yars::Id character_id ///< The Character Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	Yars::VectorString name_list = characterItemList(character_id);

	for(const auto& item_name : name_list)
	{
		characterItemRestack(character_id, item_name);
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterItem
 *
 * \brief Restack Items
 *
 * All the Items, with a matching name, that the Character has will be 
 * restacked in the most optimum way possible.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item", 20);
 * yars.characterItemAdd(bubba, "Item", 40);
 * yars.characterItemRestack(bubba, "Item");
 * \endexample
 *
 * \retval true  Successfully restacked the Items
 * \retval false Failed to restack the Items
 */
bool Yars::characterItemRestack(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	Yars::MultimapStringItem& inventory = character_item[character_id];
	auto                      range     = inventory.equal_range(item_name);
	const uint32_t            count_max = character_config[character_id].item_stack_max;

	uint32_t remaining = 0;
	for(auto iter = range.first; iter != range.second; iter++)
	{
		Yars::Item& item = iter->second;
		remaining += std::exchange(item.count, 0);
	}

	auto iter = range.first;

	while(iter != range.second)
	{
		Yars::Item& item = iter->second;
		iter++;

		if(remaining <= count_max)
		{
			item.count = remaining;
			remaining = 0;
			break;
		}

		item.count = count_max;
		remaining -= count_max;
	}

	if(remaining == 0)
	{
		if(iter != range.second)
		{
			inventory.erase(iter, range.second);
		}
	}
	else
	{
		characterItemAdd(character_id, item_name, remaining);
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Select a Skill for an Item
 *
 * This method to determine what the best Skill to use an Item.  Attributes are 
 * not considered, so it is possible that another Skill may be better.  This is 
 * intentional so that the "best" skill may not be automatically found.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * std::string skill_name = yars.characterItemSkillAttacker(bubba, "Item");
 * \endexample
 *
 * \return The Skill name
 */
std::string Yars::characterItemSkillAttacker(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	const Yars::Item&           item      = characterItem(character_id, item_name);

	int32_t     skill_level = 0;
	std::string skill_name  = "";

	for(const auto& name : item.skill_attacker)
	{
		auto iter = skill_map.find(name);
		if(iter == skill_map.end())
		{
			continue;
		}

		const Yars::Skill& skill = iter->second;

		if(skill.level.value > skill_level)
		{
			skill_level = skill.level.value;
			skill_name  = name;
		}
	}

	return skill_name;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Get a list of usable Skills for an Item
 *
 * A list of Skill names will be created of all the Skills that a Character can 
 * use with the specified Item.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * Yars::VectorString list = yars.characterItemSkillAttacker(bubba, "Item");
 * \endexample
 *
 * \return A vector of Skill names
 */
Yars::VectorString Yars::characterItemSkillAttackerList(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	const Yars::Item&           item      = characterItem(character_id, item_name);

	Yars::VectorString list;

	for(const auto& name : item.skill_attacker)
	{
		if(skill_map.contains(name) == false)
		{
			continue;
		}

		list.push_back(name);
	}

	return list;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Select a Skill for an Item
 *
 * This method to determine what the best Skill to use an Item.  Attributes are 
 * not considered, so it is possible that another Skill may be better.  This is 
 * intentional so that the "best" skill may not be automatically found.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * std::string skill_name = yars.characterItemSkillDefender(bubba, "Item");
 * \endexample
 *
 * \return The Skill name
 */
std::string Yars::characterItemSkillDefender(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	const Yars::Item&           item      = characterItem(character_id, item_name);

	int32_t     skill_level = 0;
	std::string skill_name  = "";

	for(const auto& name : item.skill_defender)
	{
		auto iter = skill_map.find(name);
		if(iter == skill_map.end())
		{
			continue;
		}

		const Yars::Skill& skill = iter->second;

		if(skill.level.value > skill_level)
		{
			skill_level = skill.level.value;
			skill_name  = name;
		}
	}

	return skill_name;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterItem
 *
 * \brief Get a list of usable Skills for an Item
 *
 * A list of Skill names will be created of all the Skills that a Character can 
 * use with the specified Item.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.skill_defender = { "Skill" }
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 * yars.characterItemAdd(bubba, "Item");
 *
 * Yars::VectorString list = yars.characterItemSkillDefender(bubba, "Item");
 * \endexample
 *
 * \return A vector of Skill names
 */
Yars::VectorString Yars::characterItemSkillDefenderList(const Yars::Id character_id ///< The Character Id
	, const std::string& item_name ///< The Item name
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	const Yars::Item&           item      = characterItem(character_id, item_name);

	Yars::VectorString list;

	for(const auto& name : item.skill_defender)
	{
		if(skill_map.contains(name) == false)
		{
			continue;
		}

		list.push_back(name);
	}

	return list;
}

// }}}
// {{{ Character: Equipment

/**
 * \todo{Equipment} Be able to define equipment locations.
 * - characterEquipmentLocationAdd(const Yars::Id, const std::string&)
 * - characterEquipmentLocationAdd(const Yars::Id, const Yars::VectorString&)
 * - characterEquipmentLocationRemove(const Yars::Id, const std::string&)
 * - characterEquipmentLocationRemoveAll(const Yars::Id)
 * - characterEquipmentLocationDisable(const Yars::Id, const std::string&)
 * - characterEquipmentLocationEnable(const Yars::Id, const std::string&)
 *
 * \ingroup group_Character
 *
 * \defgroup group_CharacterEquipment Equipment
 *
 * \brief A Character's Equipment
 *
 * Equipment allows the passive modifiers of an Item to affect a Character.  
 * What separates equipable items from unequipable items? Nothing in \yars 
 * enforces that distinction, so any item can be equipped.
 *
 * When an Item is equipped, the Yars::Item::modify_attribute,
 * Yars::Item::modify_skill, Yars::Item::modify_health_current, and 
 * Yars::Item::modify_health_maximum will be applied to the Character. Of 
 * course unequipping the Item will reverse these affects.  However, be careful 
 * when removing equipment! For example, if a Character has less than 5 health 
 * and unequips an Item that with a `modify_health_current +5`, this will drop 
 * the Character's health to `0` and kill that Character.
 *
 * Equipment is based on location.  Locations are free-form strings that make 
 * sense for the RPG.
 */

/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 * \todo{Equipment}    Rename to `characterEquipment()`
 *
 * \ingroup group_CharacterEquipment
 *
 * \brief Access a piece of equipment.
 *
 * Provides access to an equipped Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 * Yars::Item& item = yars.characterEquipmentItem(bubba, "Location");
 * \endexample
 *
 * \return A reference to an Item
 */
Yars::Item& Yars::characterEquipmentItem(const Yars::Id character_id ///< The Character Id
	, const std::string& location ///< The location
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Item* item = nullptr;
		return *item;
	}

	if(characterEquipmentExists(character_id, location) == false)
	{
		Yars::Item* item = nullptr;
		return *item;
	}
#endif

	Yars::MapStringItem& equipment_item = character_equipment_item.at(character_id);

	return equipment_item[location];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 * \todo{Equipment}    Rename to `characterEquipment()`
 *
 * \ingroup group_CharacterEquipment
 *
 * \brief Access a piece of equipment.
 *
 * Provides access to an equipped Item.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 *
 * const Yars& y = yars;
 * const Yars::Item& item = y.characterEquipmentItem(bubba, "Location");
 * \endexample
 *
 * \return A reference to an Item
 */
const Yars::Item& Yars::characterEquipmentItem(const Yars::Id character_id ///< The Character Id
	, const std::string& location ///< The location
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		Yars::Item* item = nullptr;
		return *item;
	}

	if(characterEquipmentExists(character_id, location) == false)
	{
		Yars::Item* item = nullptr;
		return *item;
	}
#endif

	const Yars::MapStringItem& equipment_item = character_equipment_item.at(character_id);

	return equipment_item.at(location);
}


/**
 * \deprecated{InventoryRewrite}
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_CharacterEquipment
 *
 * \brief Get the name of a piece of equipment.
 *
 * Get the name of the equipped Item at the specified \p location.  If the 
 * equipment location does not have an Item, then an empty string will be 
 * returned.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 * std::string item_name = yars.characterEquipmentName(bubba, "Location");
 * \endexample
 *
 * \return The Item name
 */
std::string Yars::characterEquipmentName(const Yars::Id character_id ///< The Character Id
	, const std::string& location ///< The location
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}

	if(characterEquipmentExists(character_id, location) == false)
	{
		return {};
	}
#endif

	const Yars::MapStringString& equipment_name = character_equipment_name.at(character_id);

	return equipment_name.at(location);
}


/**
 * \ingroup group_CharacterEquipment
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \brief Equip an Item
 *
 * The Character will take the Item out of its inventory and a equip it at the 
 * specified location.  The Character must have the Item and the location must 
 * not already be occupied, otherwise the Item can not be equipped.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 * \endexample
 *
 * \retval true  The Item was equipped
 * \retval false Failed to equip the Item
 */
bool Yars::characterEquipmentAdd(const Yars::Id character_id ///< The Character Id
	, const std::string& location  ///< The location
	, const std::string& item_name ///< The Item name
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemExists(character_id, item_name) == false)
	{
		return false;
	}
#endif

	if(itemIsUsable(item_name) == false)
	{
		return false;
	}

	Yars::MapStringItem&   equipment_item = character_equipment_item[character_id];
	Yars::MapStringString& equipment_name = character_equipment_name[character_id];

	if(equipment_item.contains(location) == true)
	{
		// Error Type: Equipment must be removed first
		return false;
	}

	Yars::Item item = characterItemTake(character_id, item_name);

	equipment_item[location] = item;
	equipment_name[location] = item_name;

	Yars::Health& health = this->characterHealth(character_id);
	healthModifierAdjust(health, item.modify_health_maximum);
	healthAdjust(        health, item.modify_health_current);

	if(health.current == 0)
	{
		// Event: Character Died
	}

	Yars::MapStringAttribute& attr_map = characterAttributeMap(character_id);
	for(const auto& iter : item.modify_attribute)
	{
		Yars::Attribute& attr = attr_map[iter.first];
		Yars::levelModifierAdjust(attr.level, +iter.second);
	}

	Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	for(const auto& iter : item.modify_skill)
	{
		Yars::Skill& skill = skill_map[iter.first];
		Yars::levelModifierAdjust(skill.level, +iter.second);
	}

	return true;
}


/**
 * \ingroup group_CharacterEquipment
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \brief Unequip an Item
 *
 * The Character will remove an equipped Item and place it back into the 
 * Inventory.
 *
 * \note In the future, \ref feature_InventoryPolicy "Inventory Policies" may 
 * prevent Items from going back into the Inventory, so unequipping items may 
 * not be possible until the Policy conditions have been met.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 * // Do Stuff
 * yars.characterEquipmentRemove(bubba, "Location");
 * \endexample
 *
 * \retval true  The Item was equipped
 * \retval false Failed to equip the Item
 */
bool Yars::characterEquipmentRemove(const Yars::Id character_id ///< The Character Id
	, const std::string& location ///< The location
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	Yars::MapStringItem&   equipment_item = character_equipment_item[character_id];
	Yars::MapStringString& equipment_name = character_equipment_name[character_id];

	if(equipment_item.contains(location) == false)
	{
		// No Equipment to remove
		return false;
	}

	const Yars::Item& item = equipment_item[location];

	Yars::Health& health = this->characterHealth(character_id);
	healthModifierAdjust(health, -item.modify_health_maximum);
	healthAdjust(        health, -item.modify_health_current);

	if(health.current == 0)
	{
		// Event: Character Died
	}

	Yars::MapStringAttribute& attr_map = characterAttributeMap(character_id);
	for(const auto& iter : item.modify_attribute)
	{
		Yars::Attribute& attr = attr_map[iter.first];
		Yars::levelModifierAdjust(attr.level, -iter.second);
	}

	Yars::MapStringSkill& skill_map = characterSkillMap(character_id);
	for(const auto& iter : item.modify_skill)
	{
		Yars::Skill& skill = skill_map[iter.first];
		Yars::levelModifierAdjust(skill.level, -iter.second);
	}

	characterItemAdd(character_id, equipment_name[location], equipment_item[location]);

	equipment_item.erase(location);
	equipment_name.erase(location);

	return true;
}


/**
 * \ingroup group_CharacterEquipment
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \brief Check if equipment exists.
 *
 * Check if an Item is present at the specified location.
 *
 * \example
 * Yars yars;
 * yars.itemAdd("Item", {});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterItemAdd(bubba, "Item");
 *
 * yars.characterEquipmentAdd(bubba, "Location", "Item");
 * if(yars.characterEquipmentExists(bubba, "Location"))
 * {
 * 	std::cout << "Bubba has an Item equipped at Location!\n";
 * }
 * \endexample
 *
 * \retval true  An Item is equipped at that location
 * \retval false Nothing is equipped at that location
 */
bool Yars::characterEquipmentExists(const Yars::Id character_id ///< The Character Id
	, const std::string& location ///< The location
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	const Yars::MapStringItem& equipment_item = character_equipment_item.at(character_id);

	return equipment_item.contains(location);
}

// }}}
// {{{ Character: Level

/**
 * \ingroup group_Character
 *
 * \defgroup group_CharacterLevel Level
 *
 * \brief A Character's Level
 *
 * When a Character is able to _Level Up_, that Character must choose between 
 * one of two options:
 * 1. Increase health
 * 2. Increase a Skill's Attributes
 *
 * This forces a Character to be highly skilled but easily killed, or lowly 
 * skilled but durable.  Items play a pivotal role as well because Items have 
 * the capability to increase a Character's health and skills.
 *
 * Using the dynamics of leveling and Items, an RPG can limit a Character to a 
 * fixed amount of health and only allow Skills to increase forcing the 
 * Character to rely upon Items for defense and health boosting properties.
 * The inverse is also possible, where a Character can only increase their 
 * Health and must rely on Items for Skills.  It is possible, of course, to do 
 * something in the middle of these two extremes.
 */

/**
 * \ingroup group_CharacterLevel
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \brief Increase a Character's Health
 *
 * The Character's health will be increased by the specified \p amount.
 *
 * \example
 * Yars yars;
 * Yars::Id bubba = yars.characterCreate();
 * yar.characterLevelIncrease(bubba, 1);
 * \endexample
 *
 * \retval true  The health was increased
 * \retval false Failed to increase the health
 */
bool Yars::characterLevelIncrease(const Yars::Id character_id ///< The Character Id
	, const uint32_t amount ///< The amount of increase
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	Yars::Health& health = character_health[character_id];

	Yars::healthMaximumAdjust(health, amount);

	return true;
}


/**
 * \ingroup group_CharacterLevel
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \brief Increase a Character's Attributes
 *
 * The Character's Attributes that are associated with a Skill will be 
 * increased by the specified \p amount.  The \p amount is not applied evenly 
 * to all attributes.  The Attribute's contribution to the Skill determines how 
 * much they will increase.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * {	.attribute_contribution =
 * 	{	{ "Attr_1", 1 }
 * 	,	{ "Attr_2", 3 }
 * 	}
 * });
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 *
 * yar.characterLevelIncrease(bubba, "Skill", 1);
 * // Total contribution is 4
 * // Attr_1 will increase by 1/4 = 0.25
 * // Attr_2 will increase by 3/4 = 0.75
 * \endexample
 *
 * \retval true  The Attributes were increased
 * \retval false Failed to increase the Attributes
 */
bool Yars::characterLevelIncrease(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill
	, const uint32_t     amount     ///< The amount of increase
	) noexcept
{
	if(skill_name.empty())
	{
		return characterLevelIncrease(character_id, amount);
	}

	Yars::MapStringSkill& skill_data = character_skill[character_id];

	if(skill_data.contains(skill_name) == false)
	{
		return false;
	}

	Yars::Skill& skill = skill_data[skill_name];

	float contribution_total = std::accumulate(std::begin(skill.attribute_contribution)
		, std::end(skill.attribute_contribution)
		, 0
		, [](const float& total, const auto& pair)
		{
			return total + pair.second;
		}
		);

	auto& attribute = character_attribute.at(character_id);

	for(const auto& iter : skill.attribute_contribution)
	{
		const std::string& attribute_name      = iter.first;
		const int32_t&     contribution_amount = iter.second;

		float increase = amount * (contribution_amount / contribution_total);

		levelIncrease(attribute[attribute_name].level, increase);
	}

	return true;
}


/**
 * \ingroup group_CharacterLevel
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \brief Set a Character's Attribute Levels
 *
 * The Character's Attributes that are associated with a Skill will be set by 
 * the specified \p amount.  The \p amount is not applied evenly to all 
 * attributes.  The Attribute's contribution to the Skill determines how much 
 * they will receive of the \p amount.
 *
 * Normal \ref group_Level "leveling" rules will apply.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill",
 * {	.attribute_contribution =
 * 	{	{ "Attr_1", 1 }
 * 	,	{ "Attr_2", 3 }
 * 	}
 * });
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 *
 * yar.characterLevelSet(bubba, "Skill", 1);
 * // Total contribution is 4
 * // Attr_1 will be set to 1/4 = 0.25
 * // Attr_2 will be set to 3/4 = 0.75
 * \endexample
 *
 * \retval true  The Attributes were increased
 * \retval false Failed to increase the Attributes
 */
bool Yars::characterLevelSet(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill to change
	, const uint32_t     amount     ///< The new level amount
	) noexcept
{
	if(skill_name.empty())
	{
		return characterLevelIncrease(character_id, amount);
	}

	Yars::MapStringSkill& skill_data = character_skill[character_id];

	if(skill_data.contains(skill_name) == false)
	{
		return false;
	}

	Yars::Skill& skill = skill_data[skill_name];

	float contribution_total = std::accumulate(std::begin(skill.attribute_contribution)
		, std::end(skill.attribute_contribution)
		, 0
		, [](const float& total, const auto& pair)
		{
			return total + pair.second;
		}
		);

	auto& attribute = character_attribute.at(character_id);

	for(const auto& iter : skill.attribute_contribution)
	{
		const std::string& attribute_name      = iter.first;
		const int32_t&     contribution_amount = iter.second;

		float increase = amount * (contribution_amount / contribution_total);

		levelSet(attribute[attribute_name].level, 0, increase);
	}

	return true;
}


// }}}
// {{{ Character: Affect

/**
 * \todo Character should have a subset of the main data. Yars::Attribute has a 
 * name, data, and a level each of which or in separate containers.  When a 
 * Character gains an Attribute, a new Attribute is create in the Character's 
 * data-space with Id's that point to the original name and data.  However a 
 * new Level is created and the level Id will point to that instead.  The same 
 * pattern will work for Skills.
 *
 * \todo Everything should be access using Id's. Get a Skill Id from a 
 * Character, then use that Id to get info about the Skill.
 *
 * \ingroup group_Character
 *
 * \defgroup group_CharacterAffect Affect
 *
 * \brief Character and Affect interaction
 *
 * After an Affect has been created, it must be applied to a Character in order 
 * for it to do something.  This group of methods control that interaction.
 *
 * Once an Affect is active in a Character, that Affect can not be applied 
 * again.  There is no problem if a new Affect is created and applied to a 
 * Character that does the same thing.
 */

/**
 * \todo Add a method to get the best skill from an Affect, like with Item.
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Apply an Affect to a Character
 *
 * The Affect will be applied to the Character.  The targeted Character has no 
 * chance of defending against the Affect as the Affect is applied immediately.
 *
 * Since there is no chance of defending against the Affect, no experience will 
 * be gained by either the creator or defender of the Affect.
 *
 * \see Yars::characterAffectDefend()
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_current = 5
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id healing = yars.affectCreate();
 * yars.affectInit(healing, bubba, "Item", "Skill", 100);
 *
 * yars.characterAffectApply(bubba, healing);
 * \endexample
 *
 * \retval true  The Affect was applied
 * \retval false Failed to apply the Affect
 */
bool Yars::characterAffectApply(const Yars::Id character_id ///< The Character Id
	, const Yars::Id affect_id ///< The Affect Id
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(affectExists(affect_id) == false)
	{
		return false;
	}

	if(characterAffectExists(character_id, affect_id) == true)
	{
		return false;
	}
#endif

	this->character_affect.emplace(character_id, affect_id);

	const Yars::Affect& affect = this->affect(affect_id);
	Yars::Health& health = this->characterHealth(character_id);

	healthModifierAdjust(health, affect.modify_health_maximum);
	healthAdjust(        health, affect.modify_health_current);

	if(health.current == 0)
	{
		// Event: Character Died
	}

	for(const auto& iter : affect.modify_skill)
	{
		const std::string& skill_name = iter.first;
		const int32_t&     modifier   = iter.second;

		if(characterSkillExists(character_id, skill_name) == false)
		{
			characterSkillAdd(character_id, skill_name);
		}

		Yars::Skill& skill = characterSkill(character_id, skill_name);
		levelModifierAdjust(skill.level, modifier);
	}

	for(const auto& iter : affect.modify_attribute)
	{
		const std::string& attribute_name = iter.first;
		const int32_t&     modifier       = iter.second;

		Yars::Attribute& attribute = characterAttribute(character_id, attribute_name);
		levelModifierAdjust(attribute.level, modifier);
	}

	return true;
}


/**
 * \bug A skill level of 0 should be the same as not having that skill
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Add error codes to the debugging messages
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Apply an Affect to a Character
 *
 * The Affect will be applied to the Character.  However, unlike 
 * Yars::characterAffectApply(), the character will have a chance to defend 
 * against the Affect using the specified skill.
 *
 * If the Character successfully defends against the Affect, the defender will 
 * gain the experience.  However, if the Affect is applied, the creator of the 
 * Affect will get the experience.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_current = -5
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id poison = yars.affectCreate();
 * yars.affectInit(poison, bubba, "Item", "Skill", 100);
 *
 * Yars::Id billybob = yars.characterCreate();
 * yars.characterAffectDefend(billybob, poison, "Skill");
 * \endexample
 *
 * \retval true  The Affect was applied
 * \retval false Failed to apply the Affect
 */
bool Yars::characterAffectDefend(const Yars::Id character_id ///< The Character Id
	, const Yars::Id     affect_id  ///< The Affect Id
	, const std::string& skill_name ///< The Defense Skill
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(affectExists(affect_id) == false)
	{
		return false;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		return false;
	}
#endif
	
	Yars::Affect& affect = this->affect(affect_id);

	auto iter = std::find(std::begin(affect.defense_skill)
		, std::end(affect.defense_skill)
		, skill_name
		);

	if(iter != affect.defense_skill.end())
	{
		// Do skill check
		const Yars::Skill& skill       = characterSkill(character_id, skill_name);
		const uint32_t     skill_level = skill.level.value;

		const float difficulty    = calculateDifficulty(skill_level, affect.level);
		const Yars::Result result = difficultyToResult(difficulty);

		if(result == Yars::Result::Success_Automatic)
		{
			// successfully defended
			return false;
		}

		if(result == Yars::Result::Success)
		{
			// successfully defended
			characterSkillIncrease(character_id
				, skill_name
				, affect.experience
				);

			return false;
		}

		if(result == Yars::Result::Chance)
		{
			bool success = calculateIsSuccess(character_id, skill_name, difficulty);

			if(success == true)
			{
				// successfully defended
				characterSkillIncrease(character_id
					, skill_name
					, affect.experience
					);

				return false; // Affect was not applied
			}
		}
	}

	if(characterAffectExists(character_id, affect_id) == false)
	{
		characterAffectApply(character_id, affect_id);

		if(affect.experience > 0 && affect.origin_skill.empty() == false)
		{
			characterSkillIncrease(affect.origin_character
				, affect.origin_skill
				, affect.experience
				);
		}
	}

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Add error codes to the debugging messages
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Check if an Affect has been applied
 *
 * Checks the Character to determine if the Affect is currently active.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_maximum = 10
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id hp_boost = yars.affectCreate();
 * yars.affectInit(hp_boost, bubba, "Item", "Skill", 100);
 * yars.characterAffectApply(bubba, hp_boost);
 *
 * if(yars.characterAffectExists(bubba, hp_boost))
 * {
 * 	std::cout << "Bubba has an HP Boost!\n";
 * }
 * \endexample
 *
 * \retval true  The Affect exists
 * \retval false The Affect does not exist
 */
bool Yars::characterAffectExists(const Yars::Id character_id ///< The Character Id
	, const Yars::Id affect_id ///< The Affect
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return false;
	}
#endif

	return multimapContains(character_affect, character_id, affect_id);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Add error codes to the debugging messages
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Get a list of Affects
 *
 * Creates a list of all the Affects that are currently applied to the 
 * Character.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_maximum = 10
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id hp_boost = yars.affectCreate();
 * yars.affectInit(hp_boost, bubba, "Item", "Skill", 100);
 * yars.characterAffectApply(bubba, hp_boost);
 *
 * Yars::VectorId list = yars.characterAffectList(bubba);
 * \endexample
 *
 * \return A list of Affects
 */
Yars::VectorId Yars::characterAffectList(const Yars::Id character_id ///< The Character Id
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return {};
	}
#endif

	Yars::VectorId vector;

	auto range = character_affect.equal_range(character_id);

	std::for_each(range.first, range.second
		, [&](const auto& iter)
		{
			const Yars::Id& affect_id = iter.second;

			vector.push_back(affect_id);
		});

	return vector;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Remove an Affect
 *
 * Removes an Affect from a Character.  After Affect has been removed, the 
 * changes in the Character that were made by the Affect will remain.  
 * Effectively becoming permanent.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_maximum = 10
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id hp_boost = yars.affectCreate();
 * yars.affectInit(hp_boost, bubba, "Item", "Skill", 100);
 * yars.characterAffectApply(bubba, hp_boost);
 *
 * yars.characterAffectRemove(bubba, hp_boost);
 * // Bubba's max health is still at +10.
 * \endexample
 */
void Yars::characterAffectRemove(const Yars::Id character_id ///< The Character Id
	, const Yars::Id affect_id ///< The Affect
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return;
	}

	if(characterAffectExists(character_id, affect_id) == false)
	{
		return;
	}
#endif

	multimapErase(character_affect, character_id, affect_id);
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_CharacterAffect
 *
 * \brief Remove an Affect
 *
 * Removes an Affect from a Character.  After Affect has been removed, the 
 * changes in the Character will return to what they were before the Affect was 
 * applied.
 *
 * If an Affect increased the Character's current health, there is a chance 
 * that the Character can die if the current health goes to `0` after the 
 * revert.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Skill", {});
 * yars.itemAdd("Item",
 * 	{	.skill_attacker = { "Skill" }
 * 	,	.affect_health_maximum = 10
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd("Skill");
 * yars.levelSet(yars.characterSkill(bubba, "Skill").level, 10);
 * yars.characterItemAdd("Item");
 *
 * Yars::Id hp_boost = yars.affectCreate();
 * yars.affectInit(hp_boost, bubba, "Item", "Skill", 100);
 * yars.characterAffectApply(bubba, hp_boost);
 *
 * yars.characterAffectRevert(bubba, hp_boost);
 * // Bubba's max health no longer has the +10
 * \endexample
 */
void Yars::characterAffectRevert(const Yars::Id character_id ///< The Character Id
	, const Yars::Id affect_id ///< The Affect
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return;
	}

	if(characterAffectExists(character_id, affect_id) == false)
	{
		return;
	}
#endif
	
	const Yars::Affect& affect = this->affect(affect_id);
	Yars::Health& health = this->characterHealth(character_id);

	healthModifierAdjust(health, -affect.modify_health_maximum);
	if(health.current == 0)
	{
		// Event: Character Died
	}

	for(const auto& iter : affect.modify_skill)
	{
		const std::string& skill_name = iter.first;
		const int32_t&     modifier   = iter.second;

		if(characterSkillExists(character_id, skill_name) == true)
		{
			Yars::Skill& skill = characterSkill(character_id, skill_name);
			levelModifierAdjust(skill.level, -modifier);
		}
	}

	for(const auto& iter : affect.modify_attribute)
	{
		const std::string& attribute_name = iter.first;
		const int32_t&     modifier       = iter.second;

		// Attributes are never deleted, no need to check for existence

		Yars::Attribute& attribute = characterAttribute(character_id, attribute_name);
		levelModifierAdjust(attribute.level, -modifier);
	}
}

// }}}
// }}}
// {{{ Affect

/**
 * \todo Affects should keep track of the Characters that they have been 
 * applied to.
 * \todo If an Affect is applied to multiple Characters, should the creator of 
 * the Affect get multiple experiences or just one.
 * \todo Affects should not create events, the Character should generate the 
 * events as data is modified.
 *
 * \defgroup group_Affect Affect
 *
 * \brief Change Character data
 *
 * Affects are used to modify Character data in positive (healing) and negative 
 * (damage) ways.  For a Character to create an Affect, they must pass a "Skill 
 * Check" against the Item that contains the Affect.  In a fantasy setting this 
 * would be a fighter using his Skill to force their sword to strike a foe.  Or 
 * a mage using their Skill with their magical implement to create a force of 
 * magic.
 *
 * If the fighter or mage fail this "Skill Check", then it can be seen as the 
 * fight missing or the mage miscasting.
 *
 * However, if the fighter or mage is successful then the result of their 
 * Skillful use of the Item is an Affect that can be applied to a Character, 
 * the Defender.  The Defender can now do "Skill Check" not against the 
 * "Attacking" Character, but against the Affect the Character created.
 *
 * If the Defending Character is successful in their "Skill Check" against the 
 * Affect, none of the Affect's modifiers will be applied. This can be 
 * interpreted as the Defender dodging the Affect or some other action.  But, 
 * if the "Skill Check" fails, the Affect's modifiers will be applied 
 * immediately.
 *
 * Another example of using Affects would be to simulate environmental effects 
 * such as Cold Weather.  An Affect could be created to reduce the "Nimble 
 * Fingers" attribute which is a contributor to the "Lockpick" skill.
 *
 * As described above, the intention of an "Affect" is to create a single 
 * common thing that a Character can create and defend against.  The advantage 
 * of this design is that it makes programming Character-To-Character 
 * interaction much easier by having a single interface.
 */

/**
 * \ingroup group_Affect
 *
 * \struct Yars::Affect
 *
 * \brief A collection of Character modifiers
 *
 * This container has all the modifiers that can be applied to a Character.
 */

/**
 * \var Yars::Affect::level
 *
 * \brief The Level of the Affect.
 */

/**
 * \var Yars::Affect::origin_character
 *
 * \brief The Character that created the Affect.
 */

/**
 * \var Yars::Affect::origin_skill
 *
 * \brief The Skill that was used to create the Affect.
 */

/**
 * \var Yars::Affect::experience
 *
 * \brief The amount of experience that may be gained from the Affect.
 */

/**
 * \var Yars::Affect::modify_health_current
 *
 * \brief When applied, how much to change the current Health.
 */

/**
 * \var Yars::Affect::modify_health_maximum
 *
 * \brief When applied, how much to change the maximum Health.
 */

/**
 * \var Yars::Affect::modify_skill
 *
 * \brief When applied, how much to change the Character's Skills
 */

/**
 * \var Yars::Affect::modify_attribute
 *
 * \brief When applied, how much to change the Character's Attributes
 */

/**
 * \var Yars::Affect::defense_skill
 *
 * \brief A collection of Skills that can be used to defend against the Affect.
 */

/**
 * \typedef Yars::VectorAffect
 *
 * \brief A convenience type.
 */

/**
 * \var Yars::affect_vector
 *
 * \brief A container of all Affects.
 */

/**
 * \var Yars::affect_pool
 *
 * \brief A container for all unused Affect Id's.
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Affect to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Affect& affect ///< Object to convert
	) noexcept
{
	std::stringstream oss;

	oss	<< "{ \"level\": "                 << std::to_string(affect.level)
		<< ", \"origin_character\": "      << std::to_string(affect.origin_character)
		<< ", \"origin_skill\": \""        << affect.origin_skill << "\""
		<< ", \"experience\": "            << std::to_string(affect.experience)
		<< ", \"modify_health_current\": " << std::to_string(affect.modify_health_current)
		<< ", \"modify_health_maximum\": " << std::to_string(affect.modify_health_maximum)
		<< ", \"modify_skill\": "          << Yars::to_string(affect.modify_skill)
		<< ", \"modify_attribute\": "      << Yars::to_string(affect.modify_attribute)
		<< ", \"defense_skill\": "         << Yars::to_string(affect.defense_skill)
		<< " }"
		;
	
	return oss.str();
}
#endif


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Affect
 *
 * \brief Access the Affect data.
 *
 * Access the Affect data.
 *
 * \example
 * Yars yars;
 * Yars::Id affect_id = yars.affectCreate();
 * Yars::Affect& affect = yars.affect(affect_id);
 * \endexample
 *
 * \return A reference to the Affect data.
 */
Yars::Affect& Yars::affect(const Yars::Id affect_id ///< The Affect ID
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(affectExists(affect_id) == false)
	{
		Affect* retval = nullptr;
		return *retval;
	}
#endif

	return affect_vector[affect_id];
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Affect
 *
 * \brief Access the Affect data.
 *
 * Access the Affect data.
 *
 * \example
 * Yars yars;
 * Yars::Id affect_id = yars.affectCreate();
 *
 * const Yars& y = yars;
 * const Yars::Affect& affect = y.affect(affect_id);
 * \endexample
 *
 * \return A reference to the Affect data.
 */
const Yars::Affect& Yars::affect(const Yars::Id affect_id ///< The Affect ID
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(affectExists(affect_id) == false)
	{
		Affect* retval = nullptr;
		return *retval;
	}
#endif

	return affect_vector.at(affect_id);
}


/**
 * \ingroup group_Affect
 *
 * \brief Create a new Affect.
 *
 * A new Affect will be created. Before using the Affect, it will need to be 
 * initialized.
 * 
 * \example
 * Yars yars;
 * Yars::Id affect = yars.affectCreate();
 * \endexample
 *
 * \see Yars::affectInit()
 *
 * \return The Affect Id
 */
Yars::Id Yars::affectCreate() noexcept
{
	Yars::Id affect_id = 0;

	if(affect_pool.empty())
	{
		affect_id = affect_vector.size();
		affect_vector.emplace_back();
	}
	else
	{
#if defined (ZAKERO_YARS_ENABLE_ID_SHUFFLE)
		std::random_shuffle(std::begin(affect_pool)
			, std::end(affect_pool)
			);
#endif
		affect_id = affect_pool.back();
		affect_pool.pop_back();

		affectClear(affect_id);
	}

	return affect_id;
}


/**
 * \bug It should not be possible to delete an Affect that is active.
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Affect
 *
 * \brief Delete an Affect
 *
 * Delete an affect that has been created.
 *
 * \example
 * Yars yars;
 * Yars::Id affect = yars.affectCreate();
 * // Do Stuff
 * yars.affectDelete(affect);
 * \endexample
 *
 * \retval true  The Affect was deleted
 * \retval false Failed to delete the Affect
 */
bool Yars::affectDelete(const Yars::Id affect_id ///< The Affect
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(affectExists(affect_id) == false)
	{
		return false;
	}
#endif

#if defined (ZAKERO_YARS_ENABLE_CLEAR_ON_DELETE)
	affectClear(affect_id);
#endif

	affect_pool.push_back(affect_id);

	return true;
}


/**
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Use error codes in debugging messages
 *
 * \ingroup group_Affect
 *
 * \brief Check if an Affect exists.
 *
 * Check if an Affect exists.
 *
 * \example
 * Yars yars;
 * Yars::Id affect = yars.affectCreate();
 *
 * if(yars.affectExists(affect) == true)
 * {
 * 	std::cout << "Affect exists!\n";
 * }
 * \endexample
 *
 * \retval true  The Affect Exists
 * \retval false The Affect does not Exist
 */
bool Yars::affectExists(const Yars::Id affect_id ///< The Affect
	) const noexcept
{
	if(affect_id >= affect_vector.size())
	{
		return false;
	}

	return Yars::vectorContains<Yars::Id>(affect_pool, affect_id) == false;
}


/**
 * \bug Should not be able to initialize an Affect that already has been 
 * initialized.
 *
 * \todo{DebugMessage} Add debugging messages
 * \todo{ErrorCode}    Return an error code instead of a boolean
 *
 * \ingroup group_Affect
 *
 * \brief Initialize an Affect
 *
 * The Affect will be initialized.  To initialize an Affect, the Character that 
 * is creating the Affect, the Item, and the Skill being used must be provided.  
 * A "Skill Check" will be made and if successful, the Affect is initialized 
 * and the Level of the Affect will be the same as the Level of the Skill.
 *
 * The resulting Level of the Affect can be changed by using the \p adjustment 
 * parameter.  The adjustment will be added to the Affect Level.  Keep in mind 
 * that a positive \p adjustment value will make the "Skill Check" more 
 * difficult and a negative \p adjustment value will make the "Skill Check" 
 * easier.
 *
 * When a Character defends against an Affect, their Skill Level and the Affect 
 * Level will be compared.
 *
 * The intent of the \p adjustment is to provide a way for a Character to put 
 * more effort into an action at the expense of a higher chance of failure.  So 
 * treating this as "free" power, it should come at a "cost".  An example of 
 * this would be to use a Character's stamina, as in an Action RPG.
 *
 * \example
 * Yars yars;
 * yars.skillAdd("Sword.Swing", {});
 * yars.itemAdd("Sword",
 * 	{	.skill_attacker = { "Sword.Swing" }
 * 	,	.skill_defender = { "Shield.Block", "Movement.Dodge" }
 * 	,	.affect_health_current = -5
 * 	});
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterDataSet("Stamina", int32_t(4));
 * yars.characterSkillAdd("Sword.Swing");
 * yars.levelSet(yars.characterSkill(bubba, "Sword.Swing").level, 10);
 * yars.characterItemAdd("Sword");
 *
 * Yars::Id sword_swing = yars.affectCreate();
 * int32_t effort = 2
 * int32_t stamina = 0;
 * yars.characterDataGet("Stamina", stamina);
 * stamina -= effort;
 * yars.characterDataSet("Stamina", stamina);
 * yars.affectInit(sword_swing, bubba, "Sword", "Sword.Swing", effort);
 * \endexample
 *
 * \retval true  The Affect was initialized
 * \retval false Failed to initialize the Affect
 */
bool Yars::affectInit(const Yars::Id affect_id    ///< The Affect
	, const Yars::Id             character_id ///< The "creator" Character
	, const Yars::Item&          item         ///< The Item
	, const std::string&         skill_name   ///< The Skill
	, const int32_t              adjustment   ///< Weaker <--> Stronger
	) noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(affectExists(affect_id) == false)
	{
		return false;
	}

	if(characterExists(character_id) == false)
	{
		return false;
	}

	if(characterItemIsUsableByAttacker(character_id, item) == false)
	{
		return false;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		return false;
	}
#endif

	const Yars::Skill& skill = characterSkill(character_id, skill_name);

	const uint32_t skill_level  = skill.level.value;
	const uint32_t affect_level = uint32_t(std::max(0
		, int32_t(skill_level + adjustment)
		));

	const float difficulty = calculateDifficulty(affect_level, skill_level);
	Yars::Result result    = difficultyToResult(difficulty);

	if(result == Yars::Result::Failure_Automatic)
	{
		return false;
	}

	float experience = 0;

	if(result == Yars::Result::Chance)
	{
		float probability = 0;
		bool success = calculateIsSuccess(character_id, skill_name, difficulty, probability);

		if(success == false)
		{
			return false;
		}

		experience = probability + 1.0f;
	}

	Yars::Affect& affect = this->affect(affect_id);

	affect.level                 = affect_level;
	affect.origin_character      = character_id;
	affect.origin_skill          = skill_name;
	affect.experience            = experience;
	affect.modify_health_current = item.affect_health_current;
	affect.modify_health_maximum = item.affect_health_maximum;
	affect.modify_skill          = item.affect_skill;
	affect.modify_attribute      = item.affect_attribute;
	affect.defense_skill         = item.skill_defender;

	return true;
}


/**
 * \ingroup group_Affect
 *
 * \brief Remove all data from an Affect.
 */
void Yars::affectClear(const Yars::Id affect_id ///< The Affect
	) noexcept
{
	Yars::Affect& affect = affect_vector[affect_id];

	affect.level                 = 0;
	affect.origin_character      = 0;
	affect.experience            = 0;
	affect.modify_health_current = 0;
	affect.modify_health_maximum = 0;

	affect.origin_skill.clear();
	affect.modify_skill.clear();
	affect.modify_attribute.clear();
	affect.defense_skill.clear();
}

// }}}
// {{{ Calculations

/**
 * \defgroup group_Calculations Calculations
 *
 * \brief Calculations
 *
 * The following methods handle all the math calculations in \yars.
 * 
 * These methods do not need to be called directly as the Yars class will do 
 * that as needed.  Then why make the calculations available if they don't need 
 * to be used? If your RPG needs to do \yars style calculations, they are here 
 * for you convenience, no reason to reverse engineer anything.
 */

/**
 * \ingroup group_Calculations
 *
 * \enum Yars::Result
 *
 * \brief The result of something
 */

#if defined (ZAKERO_YARS_ENABLE_TO_STRING)
/**
 * \ingroup group_ToString
 *
 * \brief Yars::Result to JSON formatted string
 *
 * \return A JSON formatted string
 */
std::string Yars::to_string(const Yars::Result result ///< Object to convert
	) noexcept
{
	switch(result)
	{
		case Yars::Result::Error:             return "Error";
		case Yars::Result::Failure_Automatic: return "Automatic Failure";
		case Yars::Result::Failure:           return "Failure";
		case Yars::Result::Chance:            return "Chance";
		case Yars::Result::Success:           return "Success";
		case Yars::Result::Success_Automatic: return "Automatic Success";
	};

	return "Unknown";
}
#endif

// {{{ Deprecated
/*
Yars::Result Yars::approxDifficultyCheck(const Yars::Id character_attacker
	, const std::string& item_name
	, const Yars::Id     character_defender
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_attacker) == false)
	{
		return Yars::Result::Error;
	}

	if(characterItemExists(character_attacker, item_name) == false)
	{
		return Yars::Result::Error;
	}

	if(characterExists(character_defender) == false)
	{
		return Yars::Result::Error;
	}
#endif

	std::string skill_attacker = characterItemSkillAttacker(character_attacker, item_name);
	std::string skill_defender = characterItemSkillAttacker(character_defender, item_name);

	return approxDifficultyCheck(character_attacker
		, item_name
		, skill_attacker
		, character_defender
		, skill_defender
		);
}


Yars::Result Yars::approxDifficultyCheck(const Yars::Id character_attacker
	, const std::string& item_name
	, const std::string& skill_attacker
	, const Yars::Id     character_defender
	, const std::string& skill_defender
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_attacker) == false)
	{
		return Yars::Result::Error;
	}

	if(characterSkillExists(character_attacker, skill_attacker) == false)
	{
		return Yars::Result::Error;
	}

	if(characterItemExists(character_attacker, item_name) == false)
	{
		return Yars::Result::Error;
	}

	if(characterItemIsUsableByAttacker(character_attacker, item_name, skill_attacker) == false)
	{
		return Yars::Result::Error;
	}

	if(characterExists(character_defender) == false)
	{
		return Yars::Result::Error;
	}

	if(characterSkillExists(character_defender, skill_defender) == false)
	{
		return Yars::Result::Error;
	}

	if(characterItemIsUsableByDefender(character_defender, item_name, skill_defender) == false)
	{
		return Yars::Result::Error;
	}
#endif

	const Yars::Skill& attacker = characterSkill(character_attacker, skill_attacker);
	const Yars::Skill& defender = characterSkill(character_defender, skill_defender);

	float difficulty = calculateDifficulty(attacker, defender);

	Yars::Result result = difficultyToResult(difficulty);

	return result;
}
*/
// }}}

/**
 * \ingroup group_Calculations
 *
 * \brief Determine the difficulty.
 *
 * The Attacker Skill Level and the Defender Skill Level will be passed to 
 * Yars::calculateDifficulty(const uint32_t, const uint32_t target) const.
 *
 * This is a convenience method to make it easier to compare to Skills.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Sword.Swing",
 * {	.level =
 * 	{	.value      = 100
 * 	,	.value_base = 100
 * 	}
 * });
 *
 * yars.skillAdd("Sheild.Block",
 * {	.level =
 * 	{	.value      = 110
 * 	,	.value_base = 110
 * 	}
 * });
 *
 * // Normally, Character Skills would be used. But to
 * // keep this example short, use the Skill directly.
 *
 * Yars::Skill& attacker = yars.skill("Sword.Swing");
 * Yars::Skill& defender = yars.skill("Sheild.Block");
 * float difficulty = yars.calculateDifficulty(attacker, defender);
 * \endexample
 *
 * \return A difficulty ratio
 */
float Yars::calculateDifficulty(const Yars::Skill& attacker_skill ///< Attacker Skill
	, const Yars::Skill& defender_skill ///< Defender Skill
	) const noexcept
{
	return calculateDifficulty(attacker_skill.level.value
		, defender_skill.level.value
		);
}


/**
 * \ingroup group_Calculations
 *
 * \brief Determine the difficulty.
 *
 * Using the Skill Level, compare that against the target level.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Sword.Swing",
 * {	.level =
 * 	{	.value      = 100
 * 	,	.value_base = 100
 * 	}
 * });
 *
 * // Normally, Character Skills would be used. But to
 * // keep this example short, use the Skill directly.
 *
 * Yars::Skill& skill = yars.skill("Sword.Swing");
 * uint32_t affect_level = 90;
 * float difficulty = yars.calculateDifficulty(skill, affect_level);
 * \endexample
 *
 * \see Yars::calculateDifficulty(const uint32_t, const uint32_t target) const
 *
 * \return A difficulty ratio
 */
float Yars::calculateDifficulty(const Yars::Skill& skill ///< Attacker Skill
	, const uint32_t level_target ///< Target Level
	) const noexcept
{
	return calculateDifficulty(skill.level.value, level_target);
}


/**
 * \ingroup group_Calculations
 *
 * \brief Determine the difficulty.
 *
 * Difficulty is simply comparing two numbers and calculating a ratio.  The 
 * ratio is as follows:<br>
 * <br>
 * \f$ratio=\frac{(base-target)}{(-0.5*target)}\f$<br>
 * <br>
 *
 * To read the results:
 * - Negative values are "easy"
 *   - Anything less than `-1` is an automatic success
 * - Positive values are "hard"
 *   - Anything greater than `+1` is an automatic failure
 *
 * Floating point numbers have a higher resolution the closer the value is to 
 * `0`.  This is why 'target' is multiplied by `-0.5` instead of multiplying 
 * '(base - target)' by `-2`.  Multiplying by `0.5` has a result closer to `0`, 
 * while multiplying by `2` moves away from `0`.
 *
 * \example
 * Yars yars;
 *
 * float difficulty = yars.calculateDifficulty(55, 66);
 * \endexample
 *
 * \return A difficulty ratio
 */
float Yars::calculateDifficulty(const uint32_t base ///< Base Difficulty
	, const uint32_t target ///< Target Difficulty
	) const noexcept
{
	//const float ratio = (((float)base - (float)target)) / ((float)target / -2.0f);
	const float ratio = (((float)base - (float)target)) / ((float)target * -0.5f);

	return ratio;
}



/**
 * \ingroup group_Calculations
 *
 * \brief Determine Success or Failure.
 *
 * Determine if a Character was successful in using a Skill.  The difficulty is 
 * usually the result of Yars::calculateDifficulty(), but any value could be 
 * used instead.
 *
 * The Skill value that is used will include all applicable modifiers.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Sword.Swing",
 * {	.level =
 * 	{	.value      = 100
 * 	,	.value_base = 100
 * 	}
 * });
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Sword.Swing");
 *
 * Yars::Skill& attacker = yars.characterSkill(bubba, "Sword.Swing");
 * uint32_t affect_level = 90;
 *
 * float difficulty = yars.calculateDifficulty(attacker, affect_level);
 *
 * if(yars.calculateIsSuccess(bubba, "Sword.Swing", affect_level) == true)
 * {
 * 	std::cout << "Bubba successfully swung a sword!\n";
 * }
 * \endexample
 *
 * \see Yars::calculateDifficulty()
 *
 * \retval true  Success
 * \retval false Failure
 */
bool Yars::calculateIsSuccess(const Yars::Id& character_id ///< The Character
	, const std::string& skill_name ///< The Skill
	, const float        difficulty ///< The difficulty
	) const noexcept
{
	float probability = 0;

	return calculateIsSuccess(character_id, skill_name, difficulty, probability);
}


/**
 * \ingroup group_Calculations
 *
 * \brief Determine Success or Failure.
 *
 * Determine if a Character was successful in using a Skill.  The difficulty is 
 * usually the result of Yars::calculateDifficulty(), but any value could be 
 * used instead.
 *
 * The Skill value that is used will include all applicable modifiers.
 *
 * The calculated probability of success will be stored in the \p probability 
 * parameter.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Sword.Swing",
 * {	.level =
 * 	{	.value      = 100
 * 	,	.value_base = 100
 * 	}
 * });
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Sword.Swing");
 *
 * Yars::Skill& attacker = yars.characterSkill(bubba, "Sword.Swing");
 * uint32_t affect_level = 90;
 *
 * float difficulty = yars.calculateDifficulty(attacker, affect_level);
 *
 * float odds = 0;
 * if(yars.calculateIsSuccess(bubba, "Sword.Swing", affect_level, odds))
 * {
 * 	std::cout << "Bubba successfully swung a sword!\n";
 * }
 * \endexample
 *
 * \see Yars::calculateDifficulty()
 *
 * \retval true  Success
 * \retval false Failure
 */
bool Yars::calculateIsSuccess(const Yars::Id& character_id ///< The Character
	, const std::string& skill_name  ///< The Skill
	, const float        difficulty  ///< The difficulty
	, float&             probability ///< Store the probability here
	) const noexcept
{
	const float modifier   = calculateModifier(character_id, skill_name);
	const float rand_value = zakero_yars_distribution_(mt_engine);

	probability = calculateProbability(difficulty);

	if((rand_value + modifier) < probability)
	{
		return false;
	}

	return true;
}


/**
 * \ingroup group_Calculations
 *
 * \brief Calculate the difficulty modifier.
 *
 * How the Attribute contributions work is not straight forward.  The best way 
 * to explain is by an example.  Consider a Skill is defined with the 
 * following:
 * | %Attribute Name | Contribution Amount |
 * |:---------------:|:-------------------:|
 * | foo             | 1                   |
 * | bar             | 2                   |
 * | blah            | 3                   |
 * First the total contribution amount is calculated:
 * \f{eqnarray*}{
 * total &=& 1+2+3 \\
 *       &=& 6
 * \f}
 * Next the contribution amounts are divided by the total to get the 
 * percentage:
 * | %Attribute Name | Contribution Amount | Divide | Percent |
 * |:---------------:|:-------------------:|:------:|:-------:|
 * | foo             | 1                   | 1/6    | 0.165   |
 * | bar             | 2                   | 2/6    | 0.333   |
 * | blah            | 3                   | 3/6    | 0.500   |
 * When the Skill needs to get the Attribute bonuses, it will get 16.5% of 
 * foo's level, 33% of bar's level, and 50% of blah's level.
 * | %Attribute Name | Level | Percent | Contribution Value |
 * |:---------------:|:-----:|:-------:|:------------------:|
 * | foo             | 8     | 0.165   | 1.320              |
 * | bar             | 6     | 0.333   | 2.000              |
 * | blah            | 7     | 0.500   | 3.500              |
 * The total modifier value from all the Attributes will be divided by the 
 * Skill Level:
 * \f{eqnarray*}{
 * modifier &=& \frac{1.32 + 2.00 + 3.50}{Skill.level.value} \\
 *          \\
 *          &=& \frac{6.82}{Skill.level.value}
 * \f}
 *
 * Using this formula, the Attributes will have less influence on the Skill if 
 * they are not increased as the Skill increases.  This reflects the 
 * Character's Skill ability increasing to the point were it can over come any 
 * deficiency in Attributes.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Skill",
 * {	.attribute_contribution =
 * 	{	{ "foo",  1 }
 * 	,	{ "bar",  2 }
 * 	,	{ "blah", 3 }
 * 	}
 * ,	.level =
 * 	{	.value      = 100
 * 	,	.value_base = 100
 * 	}
 * });
 *
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Skill");
 *
 * float modifier = yars.calculateModifier(bubba, "Skill");
 * \endexample
 *
 * \return The modifier
 */
float Yars::calculateModifier(const Yars::Id character_id ///< The Character Id
	, const std::string& skill_name ///< The Skill
	) const noexcept
{
#if defined (ZAKERO_YARS_ENABLE_SAFE_MODE)
	if(characterExists(character_id) == false)
	{
		return 0;
	}

	if(characterSkillExists(character_id, skill_name) == false)
	{
		return 0;
	}
#endif

	const Yars::Skill& skill = characterSkill(character_id, skill_name);

	if(skill.level.value == 0)
	{
		return 0;
	}

	float contribution_total = 0;

	for(const auto& iter : skill.attribute_contribution)
	{
		//const std::string& name  = iter.first;
		const int32_t&       value = iter.second;

		contribution_total += value;
	}

	if(contribution_total == 0)
	{
		return 0;
	}

	const Yars::MapStringAttribute& attribute = characterAttributeMap(character_id);
	float modifier = 0;

	for(const auto& iter : skill.attribute_contribution)
	{
		const std::string& name  = iter.first;
		const int32_t&     value = iter.second;

		const float ratio  = value / contribution_total;
		const float amount = ratio * attribute.at(name).level.value;

		modifier += amount;
	}

	return modifier / skill.level.value;
}


/**
 * \ingroup group_Calculations
 *
 * \brief Calculate the probability of something.
 *
 * Based on the difficulty \p ratio, calculate the probability of success.
 *
 * \f$
 * probability = \frac{ratio+ratio^3+ratio^5}{3.0}
 * \f$
 *
 * The image below shows the graph of the formula.
 * \image html Probability_Graph.png "X-Axis: ratio, Y-Axis: probability"
 *
 * When used to determin a pass/fail condition, an random number is generated 
 * and compaired to the probability.  If the number is greater than the 
 * probablity the result is "pass", otherwise it is "fail".  This translates to 
 * _above the line is success, below the line is failure._
 *
 * \example
 * Yars yars;
 *
 * float probability = yars.calculateProbability(0.5);
 * \endexample
 *
 * \return The probability
 */
float Yars::calculateProbability(const float ratio ///< The difficulty ratio
	) const noexcept
{
	const float probability = (ratio
		+ (ratio * ratio * ratio)
		+ (ratio * ratio * ratio * ratio * ratio)
		) / 3.0f;

	return probability;
}


/**
 * \ingroup group_Calculations
 *
 * \brief Convert the difficulty to a Result.
 *
 * Sometimes a numeric difficulty value is too fine grained and a generic 
 * result is more useful.  This method will convert a difficulty result into a 
 * Result enum.
 *
 * \example
 * Yars yars;
 *
 * yars.skillAdd("Sword.Swing",
 * {       .level =
 *         {       .value      = 100
 *         ,       .value_base = 100
 *         }
 * });
 *  
 * Yars::Id bubba = yars.characterCreate();
 * yars.characterSkillAdd(bubba, "Sword.Swing");
 *  
 * Yars::Skill& attacker = yars.characterSkill(bubba, "Sword.Swing");
 * uint32_t affect_level = 90;
 *  
 * float difficulty = yars.calculateDifficulty(attacker, affect_level);
 *
 * Result result = yars.difficultyToResult(difficulty);
 * \endexample
 *
 * \see Yars::calculateDifficulty()
 *
 * \return A Result
 */
Yars::Result Yars::difficultyToResult(const float difficulty ///< The difficulty of the action
	) noexcept
{
	if(difficulty < -1)
	{
		return Yars::Result::Success_Automatic;
	}

	if(difficulty > 1)
	{
		return Yars::Result::Failure_Automatic;
	}

	return Yars::Result::Chance;
}

// }}}

};

// {{{ Operator

/**
 * \defgroup group_GlobalNamespace Global Namespace
 *
 * \brief Things added to the Global Namespace.
 *
 * These functions were added to the Global Namespace to improve the 
 * Quality-Of-Life when using \yars.
 */

/**
 * \ingroup group_GlobalNamespace
 *
 * \brief Equal-To Operator
 *
 * Compare two Yars::Item objects for equality.  All fields must be exactly the 
 * same __except__ for `Yars::Item::count`.
 *
 * The value of `Yars::Item::count` is ignored.
 *
 * \retval true  Both Yars::Item objects match.
 * \retval false The Yars::Item objects differ.
 */
bool operator==(const zakero::Yars::Item& lhs ///< The left-hand side
	, const zakero::Yars::Item& rhs       ///< The right-hand side
	)
{
	if(        (lhs.affect_attribute      == rhs.affect_attribute)
		&& (lhs.affect_skill          == rhs.affect_skill)
		&& (lhs.modify_attribute      == rhs.modify_attribute)
		&& (lhs.modify_skill          == rhs.modify_skill)
		&& (lhs.skill_attacker        == rhs.skill_attacker)
		&& (lhs.skill_defender        == rhs.skill_defender)
		&& (lhs.affect_health_current == rhs.affect_health_current)
		&& (lhs.affect_health_maximum == rhs.affect_health_maximum)
		&& (lhs.modify_health_current == rhs.modify_health_current)
		&& (lhs.modify_health_maximum == rhs.modify_health_maximum)
		)
	{
		return true;
	}

	return false;
}


/**
 * \ingroup group_GlobalNamespace
 *
 * \brief Not-Equal-To Operator
 *
 * Compare two Yars::Item objects for inequality.  If any one of the fields 
 * differ, __except__ for `Yars::Item::count`, they will be considered `Not 
 * Equal`.
 *
 * The value of `Yars::Item::count` is ignored.
 *
 * \retval true  The Yars::Item objects differ.
 * \retval false Both Yars::Item objects match.
 */
bool operator!=(const zakero::Yars::Item& lhs ///< The left-hand side
	, const zakero::Yars::Item& rhs       ///< The right-hand side
	)
{
	return !(lhs == rhs);
}

// }}}

#endif

