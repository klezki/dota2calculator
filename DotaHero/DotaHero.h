#pragma once

#include <string>
#include <unordered_map>
#include <functional>

enum class Attribute
{
	Strength,
	Agility,
	Intellegence
};


struct DotaHero;
using traitCallback = std::function<float(DotaHero const&)>;

struct DotaHero
{

	float strengthGrowth = 0.0f;
	float agilityGrowth = 0.0f;
	float intellegenceGrowth = 0.0f;
	float baseDamage = 0.0f;
	float bat = 1.7f;

	float strength = 0;
	float agility = 0;
	float intellegence = 0;

	float as = 0;

	float greenDamage = 0;
	float redPhysicalDamage = 0;
	float redMagickDamage = 0;

	float crit = 1.0f;
	float critChance = 0.0f;

	Attribute attribute = Attribute::Agility;

	std::string name = "";

	constexpr float whiteDamage() const
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

	constexpr float physicalDamage() const
	{
		return (1.0f + crit/*Multiplier*/) * (whiteDamage() + greenDamage) + redPhysicalDamage;
	}

	constexpr float magickDamage() const
	{
		return redMagickDamage;
	}

	constexpr float aps() const
	{
		return (100.0f + agility + as) * 0.01f / bat;
	}

	std::unordered_map<std::string, traitCallback> traits;
};