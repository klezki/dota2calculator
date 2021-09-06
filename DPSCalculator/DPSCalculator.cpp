#include "DPSCalculator/DPSCalculator.h"
#include <Traits/Traits.h>
#include <algorithm>

inline constexpr bool operator<(const CritData& a, const CritData& b) { return a.mult < b.mult; };

traitCallback DPSCalculator::getTraitCallback(const std::string& traitName)
{
	auto ptrait = _hero.traits.find(traitName);
	if (ptrait != _hero.traits.end())
	{
		return ptrait->second;
	}
	else
	{
		return {};
	}
}


void DPSCalculator::load(const DotaHero& hero, unsigned int level, const DotaItem (&items)[MAX_ITEMS])
{

	//reset hero
	_hero = hero;
	//base stats already factor 1 level
	_hero.strength += _hero.strengthGrowth * (level - 1);
	_hero.agility += _hero.agilityGrowth * (level - 1);
	_hero.intellegence += _hero.intellegenceGrowth * (level - 1);

	
	for (size_t i = 0; i < MAX_ITEMS; i++)
	{
		const DotaItem& item = items[i];

		_hero.strength += item.strength;
		_hero.agility += item.agility;
		_hero.intellegence += item.intellegence;

		_hero.greenDamage += item.greenDamage;
		_hero.redPhysicalDamage += item.redPhysicalDamage;
		_hero.redMagickDamage += item.redMagickDamage;

		_hero.as += item.as;

		_hero.traits.insert(item.traits.begin(), item.traits.end());

		critData[i] = { item.crit - 1.0f, item.critChance };
	}
}

float DPSCalculator::calculate()
{
	setCrit();

	float dps = 0.0f;
	traitCallback dpsTrait = getTraitCallback("traitRadiance");
	if (dpsTrait)
	{
		dps = dpsTrait(_hero);
	}
	
	//preprocess()

	return (/*target.armor * */_hero.physicalDamage() + /*target.mag_res * */_hero.magickDamage()) * _hero.aps() + /*target.mag_res * */dps;
}

void DPSCalculator::setCrit()
{
	traitCallback CdCrit = getTraitCallback("traitCdCrit");
	if (CdCrit)
	{
		critData[0] = { _hero.crit - 1.0f, CdCrit(_hero) };
	}
	else
	{
		critData[0] = { _hero.crit - 1.0f, _hero.critChance };
	}

	//sort in reverse order
	std::sort(std::rbegin(critData), std::rend(critData));

	_hero.crit = critData[0].toCoef();

	float diminishingChance = 1.0f - critData[0].chance;
	
	for (int i = 1; i < MAX_ITEMS; i++)
	{
		_hero.crit += diminishingChance * critData[i].toCoef();
		diminishingChance *= 1.0f - critData[i].chance;
	}
}

