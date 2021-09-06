#pragma once
#include "DotaHero/DotaHero.h"

struct CdCrit
{
	float cd;

	float operator()(const DotaHero& hero)
	{
		return 1 / (cd * hero.aps());
	}
};

struct Radiance
{
	float dps;

	float operator()(const DotaHero& hero)
	{
		return dps;
	}
};