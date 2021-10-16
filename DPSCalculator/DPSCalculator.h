#pragma once

#include "DotaHero/DotaHero.h"
#include "DotaItem/DotaItem.h"

#define MAX_ITEMS 6

class DPSCalculator
{
public:
	void load(const DotaHero& hero, unsigned int level, const DotaItem(&items)[MAX_ITEMS], const skillDataVector& skillData, const traitDataVector& traitData);

	template <class T>
	inline std::pair<bool, const T&> getTrait(Trait::ids id)
	{
		auto ptrait = _hero.traits.find(id);
		if (ptrait != _hero.traits.end())
		{
			T* trait = static_cast<T*>(ptrait->second.get());
			return { true, *trait };
		}
		else
		{
			return { false, {} };
		}
	};

	float calculate();

private:
	void setCrit();
	int _level;
	DotaHero _hero = {};
	CritData critData[MAX_ITEMS + 1];



};