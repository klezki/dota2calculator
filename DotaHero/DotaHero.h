#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <utility>
#include <memory>

//TODO move declarations

#define SKILL_LEVEL_NUM 4

enum class talentFlag
{
	empty,
	on,
	off,
	only,
};

struct Trait
{
	enum class ids
	{
		crit,
		cdCrit,
		
		//items
		radiance,
	};

	talentFlag flag = talentFlag::empty;
	ids traitId;
	int level;
	std::string name;
};


struct Skill
{
	enum class ids
	{
		strength,
		agility,
		intellegence,
		greenDamage,
		redPhysicalDamage,
		redMagickDamage,
		as,
	};

	talentFlag flag = talentFlag::empty;
	ids id;
	int level = 0;
	float val[SKILL_LEVEL_NUM] = {};
	float talent = 0.0f;
	std::string name;
};

enum class Attribute
{
	Strength,
	Agility,
	Intellegence
};

struct CritData
{
	float mult = 0.0f;
	float chance = 0.0f;

	constexpr float toCoef() const { return mult * chance; }

};

using skillDataVector = std::vector<std::tuple<int, talentFlag>>;
using traitDataVector = std::vector<std::tuple<Trait::ids, int, talentFlag>>;
using traitMap = std::unordered_map<Trait::ids, std::shared_ptr<Trait>>;

struct DotaHero
{

	float strengthGrowth = 0.0f;
	float agilityGrowth = 0.0f;
	float intellegenceGrowth = 0.0f;
	float baseDamage = 0.0f;
	float bat = 1.7f;

	float strength = 0.0f;
	float agility = 0.0f;
	float intellegence = 0.0f;

	float as = 0.0f;

	float greenDamage = 0.0f;
	float redPhysicalDamage = 0.0f;
	float redMagickDamage = 0.0f;

	float crit = 1.0f;
	float critChance = 0.0f;

	Attribute attribute = Attribute::Agility;

	std::string name = "";

	float whiteDamage() const
	{
		switch (attribute)
		{
		case Attribute::Strength:
			return strength + baseDamage;
		case Attribute::Agility:
			return agility + baseDamage;
		case Attribute::Intellegence:
			return intellegence + baseDamage;
		default:
			return 0;
		}
	}

	float physicalDamage() const
	{
		return (1.0f + crit/*Multiplier*/) * (whiteDamage() + greenDamage) + redPhysicalDamage;
	}

	float magickDamage() const
	{
		return redMagickDamage;
	}

	float aps() const
	{
		return (100.0f + agility + as) * 0.01f / bat;
	}

	std::vector<Skill> skills;
	traitMap traits;
};