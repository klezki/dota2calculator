#pragma once
#include "DotaHero/DotaHero.h"

/**
 * Crit is trait because crit chance and crit mult
 * must be unified when creating widgets for HeroDialog
 */
struct Crit : Trait
{
	//assuming talent bufs only crit or only chance
	float critTalent = 0.0f;
	float critChanceTalent = 0.0f;
	float crit[SKILL_LEVEL_NUM] = {};
	float critChance[SKILL_LEVEL_NUM] = {};

	CritData operator()() const
	{
		if (flag == talentFlag::on)
		{
			return { crit[level] + critTalent, critChance[level] + critChanceTalent };
		}
		else
		{
			return { crit[level], critChance[level] };
		}		
	}
};


struct CdCrit : Trait
{
	float cdTalent = 0.0f;
	float crit[SKILL_LEVEL_NUM] = {};
	float cd[SKILL_LEVEL_NUM] = {};

	CritData operator()(const DotaHero& hero) const
	{
		if (flag == talentFlag::on)
		{
			float chance = 1 / (cd[level] - cdTalent) * hero.aps();
			return { crit[level], chance };
		}
		else
		{
			float chance = 1 / cd[level] * hero.aps();
			return { crit[level], chance };
		}
	}
};

/********ITEMS**********/

struct Radiance : Trait
{
	float dps = 0.0f;
};