#pragma once

#include "DotaHero/DotaHero.h"
#include "DotaItem/DotaItem.h"

#define MAX_ITEMS 6


struct CritData
{
	float mult = 0.0f;
	float chance = 0.0f;

	constexpr float toCoef() const { return mult * chance; }

};

class DPSCalculator
{
public:
	void load(const DotaHero& hero, unsigned int level, const DotaItem (&items)[MAX_ITEMS]);
	traitCallback getTraitCallback(const std::string& traitName);
	float calculate();

private:
	void setCrit();
	DotaHero _hero = {};
	CritData critData[MAX_ITEMS + 1];



};