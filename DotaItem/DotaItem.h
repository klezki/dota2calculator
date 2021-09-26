#pragma once

#include <string>
#include <DotaHero/DotaHero.h>

struct DotaItem
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
		crit,
		critChance,

		traitRadiance,
	};


	float cost = 0;
	float strength = 0;
	float agility = 0;
	float intellegence = 0;
	float greenDamage = 0;
	float redPhysicalDamage = 0;
	float redMagickDamage = 0;
	float as = 0;
	float crit = 1.0f;
	float critChance = 0.0f;

	std::string name = "";

	std::unordered_map<std::string, std::shared_ptr<Skill>> traits;
};