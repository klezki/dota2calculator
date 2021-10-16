#include "DPSCalculator/DPSCalculator.h"
#include <Traits/Traits.h>
#include <algorithm>

namespace
{

	float getSkillData(const Skill& skill)
	{
		float talent = skill.flag == talentFlag::on ? skill.talent : 0.0f;
		return skill.val[skill.level] + talent;

	}

}

static inline constexpr bool operator<(const CritData& a, const CritData& b) { return a.mult < b.mult; };

void DPSCalculator::load(const DotaHero& hero, unsigned int level, const DotaItem (&items)[MAX_ITEMS], const skillDataVector& skillData, const traitDataVector& traitData)
{
	//reset hero
	_hero = hero;
	_level = level;

	for (size_t i = 0; i < skillData.size(); i++)
	{
		_hero.skills[i].level = std::get<0>(skillData[i]);
		_hero.skills[i].flag = std::get<1>(skillData[i]);
	}

	for (const auto& i : traitData)
	{
		Trait::ids id = std::get<0>(i);
		Trait& trait = *(_hero.traits.at(id));
		trait.level = std::get<1>(i);
		trait.flag = std::get<2>(i);
	}


	//base stats already factor 1 level
	_hero.strength += _hero.strengthGrowth * (level - 1);
	_hero.agility += _hero.agilityGrowth * (level - 1);
	_hero.intellegence += _hero.intellegenceGrowth * (level - 1);

	for (const auto& i : _hero.skills)
	{
		switch (i.id)
		{
		case Skill::ids::strength:
			_hero.strength += getSkillData(i);
			break;
		case Skill::ids::agility:
			_hero.agility += getSkillData(i);
			break;
		case Skill::ids::intellegence:
			_hero.intellegence += getSkillData(i);
			break;
		case Skill::ids::greenDamage:
			_hero.greenDamage += getSkillData(i);
			break;
		case Skill::ids::redPhysicalDamage:
			_hero.redPhysicalDamage += getSkillData(i);
			break;
		case Skill::ids::redMagickDamage:
			_hero.redMagickDamage += getSkillData(i);
			break;
		case Skill::ids::as:
			_hero.as += getSkillData(i);
			break;
		default:
			//TODO cmon man
			throw std::exception();
			break;
		}
	}

	
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

		//TODO multiple same items traits
		_hero.traits.insert(item.traits.begin(), item.traits.end());

		critData[i] = { item.crit - 1.0f, item.critChance };
	}
}

float DPSCalculator::calculate()
{
	setCrit();

	float dps = 0.0f;
	std::pair<bool, const Radiance&> radiance = getTrait<Radiance>(Trait::ids::radiance);
	if (radiance.first)
	{
		dps = radiance.second.dps;
	}

	
	//preprocess()

	return (/*target.armor * */_hero.physicalDamage() + /*target.mag_res * */_hero.magickDamage()) * _hero.aps() + /*target.mag_res * */dps;
}

void DPSCalculator::setCrit()
{
	std::pair<bool, const Crit&> crit = getTrait<Crit>(Trait::ids::crit);
	if (crit.first)
	{
		critData[MAX_ITEMS] = crit.second();
	}
	else
	{
		std::pair<bool, const CdCrit&> cdCrit = getTrait<CdCrit>(Trait::ids::cdCrit);
		if (cdCrit.first)
		{
			critData[MAX_ITEMS] = cdCrit.second(_hero);
		}
		else
		{
			critData[MAX_ITEMS] = {0.0f, 0.0f};
		}
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

