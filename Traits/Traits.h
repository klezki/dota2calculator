#pragma once
#include "DotaHero/DotaHero.h"

struct Trait : Skill
{
	enum class traitIds
	{
		crit,
		cdCrit,
	};

	traitIds traitId;
};

struct Crit : Trait
{
	talentFlag critFlag = talentFlag::empty;
	float critTalent;
	float crit[SKILL_NUM];
	
	talentFlag critChanceFlag = talentFlag::empty;
	float critChanceTalent;
	float critChance[SKILL_NUM];

	std::pair<float, float> operator()(int level)
	{
		return {crit[level], critChance[level]};
	}
};

struct CdCrit : Trait
{
	float cd;

	float operator()(const DotaHero& hero)
	{
		return 1 / (cd * hero.aps());
	}
};

struct Radiance : Trait
{
	float dps;

	float operator()(const DotaHero& hero)
	{
		return dps;
	}
};