#include "DotaCalculator/HeroDialog.h"

HeroDialog::HeroDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Tune hero");

	initMainLayout();

	setLayout(mainLayout);
	resize(200, 300);
}

void HeroDialog::initMainLayout()
{
	mainLayout = new QVBoxLayout;
	mainLayout->addStretch();

}

void HeroDialog::onDialogOpen(const DotaHero& hero)
{
	if (heroName != hero.name)
	{
		deleteWidgets();

		createSkillWidgets(hero);
		createTraitWidgets(hero);
		heroName = hero.name;
	}
	open();
}

void HeroDialog::createSkillWidgets(const DotaHero& hero)
{
	skillWidgets.resize(hero.skills.size());

	for (size_t i = 0; i < hero.skills.size(); i++)
	{
		const Skill& skill = hero.skills[i];
		if (skill.flag == talentFlag::only)
		{
			QCheckBox* checkBox = new QCheckBox(skill.name.data());
			mainLayout->insertWidget(mainLayout->count() - 1, checkBox);
			skillWidgets[i] = std::make_tuple(nullptr, nullptr, checkBox);
		}
		else
		{
			QLabel* label = new QLabel(skill.name.data());
			mainLayout->insertWidget(mainLayout->count() - 1, label);

			QSpinBox* spinBox = new QSpinBox;
			spinBox->setRange(1, 4);
			spinBox->setSingleStep(1);
			mainLayout->insertWidget(mainLayout->count() - 1, spinBox);

			QCheckBox* checkBox = skill.flag == talentFlag::empty ? nullptr : new QCheckBox("talent");

			if (checkBox)
				mainLayout->insertWidget(mainLayout->count() - 1, checkBox);

			skillWidgets[i] = std::make_tuple(label, spinBox, checkBox);
		}
	}
}

void HeroDialog::createTraitWidgets(const DotaHero& hero)
{
	traitWidgets.resize(hero.traits.size());
	size_t i = 0;
	for (const auto& t : hero.traits)
	{
		const Trait& trait = *(t.second);
		QLabel* label = new QLabel(trait.name.data());
		mainLayout->insertWidget(mainLayout->count() - 1, label);

		QSpinBox* spinBox = new QSpinBox;
		spinBox->setRange(1, 4);
		spinBox->setSingleStep(1);
		mainLayout->insertWidget(mainLayout->count() - 1, spinBox);

		QCheckBox* checkBox = trait.flag == talentFlag::empty ? nullptr : new QCheckBox("talent");

		if (checkBox)
			mainLayout->insertWidget(mainLayout->count() - 1, checkBox);

		traitWidgets[i++] = std::make_tuple(t.first, label, spinBox, checkBox);
	}
}

void HeroDialog::deleteWidgets()
{
	for (const auto& i : skillWidgets)
	{
		QLabel* label = std::get<0>(i);
		if (label)
		{
			mainLayout->removeWidget(label);
			delete label;
		}
		
		QSpinBox* spinBox = std::get<1>(i);
		if (spinBox)
		{
			mainLayout->removeWidget(spinBox);
			delete spinBox;
		}

		QCheckBox* checkBox = std::get<2>(i);
		if (checkBox)
		{
			mainLayout->removeWidget(checkBox);
			delete checkBox;
		}
	}

	for (const auto& i : traitWidgets)
	{
		QLabel* label = std::get<1>(i);
		mainLayout->removeWidget(label);
		delete label;

		QSpinBox* spinBox = std::get<2>(i);
		mainLayout->removeWidget(spinBox);
		delete spinBox;

		QCheckBox* checkBox = std::get<3>(i);
		if (checkBox)
		{
			mainLayout->removeWidget(checkBox);
			delete checkBox;
		}
	}
}


skillDataVector HeroDialog::getSkillsData()
{
	std::vector<std::tuple<int, talentFlag>> skillData(skillWidgets.size());

	for (size_t i = 0; i < skillWidgets.size(); i++)
	{
		int level = 0;
		talentFlag flag = talentFlag::empty;

		const auto& widgetData = skillWidgets[i];

		QSpinBox* spinBox = std::get<1>(widgetData);
		if (spinBox)
		{
			level = spinBox->value();
		}

		QCheckBox* checkBox = std::get<2>(widgetData);
		if (checkBox)
		{
			if (checkBox->isChecked())
				flag = talentFlag::on;
			else
				flag = talentFlag::off;
		}

		skillData[i] = std::make_tuple(level, flag);
	}

	return skillData;
}

traitDataVector HeroDialog::getTraitsData()
{
	std::vector<std::tuple<Trait::ids, int, talentFlag>> traitData(traitWidgets.size());

	for (size_t i = 0; i < traitWidgets.size(); i++)
	{
		const auto& widgetData = traitWidgets[i];

		Trait::ids id = std::get<0>(widgetData);

		QSpinBox* spinBox = std::get<2>(widgetData);
		int level = spinBox->value();

		QCheckBox* checkBox = std::get<3>(widgetData);
		talentFlag flag = talentFlag::empty;
		if (checkBox)
		{
			if (checkBox->isChecked())
				flag = talentFlag::on;
			else
				flag = talentFlag::off;
		}

		traitData[i] = std::make_tuple(id, level, flag);
	}

	return traitData;
}