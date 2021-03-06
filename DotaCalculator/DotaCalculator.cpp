#include <DotaCalculator/DotaCalculator.h>

namespace
{
	const std::unordered_map<std::string, Skill::ids> heroesDispatchTable = {
		{ "strength",  Skill::ids::strength},
		{ "agility",  Skill::ids::agility},
		{ "intellegence",  Skill::ids::intellegence},
		{ "greenDamage",  Skill::ids::greenDamage},
		{ "redPhysicalDamage",  Skill::ids::redPhysicalDamage},
		{ "redMagickDamage",  Skill::ids::redMagickDamage},
		{ "as",  Skill::ids::as},
	};

	const std::unordered_map<std::string, Trait::ids> heroesTraitsDispatchTable = {
		{ "cdCrit",  Trait::ids::cdCrit},
		{ "crit",  Trait::ids::crit},
	};

	const std::unordered_map<std::string, DotaItem::ids> itemsDispatchTable = {
		{ "strength",  DotaItem::ids::strength},
		{ "agility",  DotaItem::ids::agility},
		{ "intellegence",  DotaItem::ids::intellegence},
		{ "greenDamage",  DotaItem::ids::greenDamage},
		{ "redPhysicalDamage",  DotaItem::ids::redPhysicalDamage},
		{ "redMagickDamage",  DotaItem::ids::redMagickDamage},
		{ "as",  DotaItem::ids::as},
		{ "crit",  DotaItem::ids::crit},
		{ "critChance",  DotaItem::ids::critChance},
	};

	const std::unordered_map<std::string, Trait::ids> itemsTraitsDispatchTable = {
		{ "radiance",  Trait::ids::radiance},
	};

	float parseDamageRange(const QString& damageRange)
	{
		QRegExp rx("^(\\d+)-(\\d+)$");
		try
		{
			bool match = rx.exactMatch(damageRange);

			if (!match)
			{
				throw std::exception();
			}

			QStringList stringList = rx.capturedTexts();
			float damageMin = stringList[1].toFloat();
			float damageMax = stringList[2].toFloat();
			return (damageMax + damageMin) / 2;
		}

		catch (const std::exception& exe)
		{
			//TODO : add something
			throw;
		}
	}
}

DotaCalculator::DotaCalculator()
	: QWidget(nullptr, Qt::MSWindowsFixedSizeDialogHint)
{
	FileManager::loadXML(heroesElement, HEROES_XSD_PATH, HEROES_XML_PATH);
	initHeroData();
	FileManager::loadXML(itemsElement, ITEMS_XSD_PATH, ITEMS_XML_PATH);
	initItemData();

	initHeroSelection();
	initHeroDialog();
	initLevelSelection();
	initItemsSelection();
	initDPSResult();
	initButtons();
	initOtherDialog();
	initMainLayout();

	setLayout(mainLayout);
	resize(300, 500);
}

void DotaCalculator::initHeroData()
{
	QDomNodeList heroes = heroesElement.childNodes();
	heroesData.reserve(heroes.size() + 1);

	//0 index is empty hero
	heroesData.emplace_back();

	for (size_t i = 0; i < heroes.size(); i++)
	{
		QDomElement heroStatElement = heroes.at(i).firstChildElement();
		DotaHero hero;

		hero.name = heroStatElement.text().toStdString();
		heroStatElement = heroStatElement.nextSiblingElement();

		QString attribute = heroStatElement.text();
		if (attribute == "strength")
			hero.attribute = Attribute::Strength;
		if (attribute == "agility")
			hero.attribute = Attribute::Agility;
		if (attribute == "intellegence")
			hero.attribute = Attribute::Intellegence;
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.strength = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.agility = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.intellegence = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.strengthGrowth = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.agilityGrowth = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.intellegenceGrowth = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		QString damageRange = heroStatElement.text();
		hero.baseDamage = parseDamageRange(damageRange);
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.bat = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		hero.as = heroStatElement.text().toFloat();
		heroStatElement = heroStatElement.nextSiblingElement();

		QDomElement heroSkillElement = heroStatElement;

		for ( ;
			!heroSkillElement.isNull() && heroSkillElement.tagName() != "trait";
			heroSkillElement = heroSkillElement.nextSiblingElement() )
		{
			Skill skill{};
			QDomElement skillElement = heroSkillElement.firstChildElement();
			skill.name = skillElement.text().toStdString();
			skillElement = skillElement.nextSiblingElement();

			Skill::ids bonusStat = heroesDispatchTable.at(skillElement.tagName().toStdString());

			skill.id = bonusStat;
			
			skillElement = skillElement.firstChildElement();

			if (skillElement.tagName() != "talent")
			{
				for (int i = 0; i < SKILL_LEVEL_NUM; i++)
				{
					skill.val[i] = skillElement.text().toFloat();
					skillElement = skillElement.nextSiblingElement();
				}

				if (!skillElement.isNull())
				{
					skill.talent = skillElement.text().toFloat();
					skill.flag = talentFlag::off;
				}
			}
			else
			{
				skill.talent = skillElement.text().toFloat();
				skill.flag = talentFlag::only;
			}
			
			hero.skills.push_back(std::move(skill));
		}

		for (QDomElement heroTraitElement = heroSkillElement;
			!heroTraitElement.isNull();
			heroTraitElement = heroTraitElement.nextSiblingElement())
		{
			QDomElement traitElement = heroTraitElement.firstChildElement();
			std::string name = traitElement.text().toStdString();
			traitElement = traitElement.nextSiblingElement();

			Trait::ids id = heroesTraitsDispatchTable.at(traitElement.tagName().toStdString());

			switch (id)
			{
			case Trait::ids::crit:
			{
				Crit trait{};
				trait.name = std::move(name);

				QDomElement critElement = traitElement.firstChildElement();
				QDomElement critChanceElement = critElement.nextSiblingElement();

				for (int i = 0; i < SKILL_LEVEL_NUM; i++) {
					trait.crit[i] = critElement.text().toFloat();
					trait.critChance[i] = critChanceElement.text().toFloat();

					critElement = critElement.nextSiblingElement();
					critChanceElement = critChanceElement.nextSiblingElement();
				}
				if (!critElement.isNull() && critElement.tagName() == "talent")
				{
					trait.critTalent = critElement.text().toFloat();
					trait.flag = talentFlag::off;
				}
				if (!critChanceElement.isNull() && critChanceElement.tagName() == "talent")
				{
					trait.critChanceTalent = critChanceElement.text().toFloat();
					assert(trait.flag == talentFlag::empty); //TODO pls remove
					trait.flag = talentFlag::off;
				}				
				hero.traits[id] = std::make_shared<Crit>(trait);
				break;
			}				
			case Trait::ids::cdCrit:
			{
				CdCrit trait{};
				trait.name = std::move(name);

				QDomElement critElement = traitElement.firstChildElement();
				QDomElement cdElement = critElement.nextSiblingElement();

				critElement = critElement.firstChildElement();
				cdElement = cdElement.firstChildElement();

				for (int i = 0; i < SKILL_LEVEL_NUM; i++) {
					trait.crit[i] = critElement.text().toFloat();
					trait.cd[i] = cdElement.text().toFloat();

					critElement = critElement.nextSiblingElement();
					cdElement = cdElement.nextSiblingElement();
				}

				/*if (!critElement.isNull() && critElement.tagName() == "talent")
				{
					trait.critTalent = critElement.text().toFloat();
					trait.flag = talentFlag::off;
				}*/

				//TODO remove assert?
				assert(cdElement.tagName() == "talent");

				trait.cdTalent = cdElement.text().toFloat();
				trait.flag = talentFlag::off;

				hero.traits[id] = std::make_shared<CdCrit>(trait);
				break;
			}
			default:
				throw std::exception();
				break;
			}

		}

		heroesData.push_back(std::move(hero));
	}
}

void DotaCalculator::initItemData()
{
	QDomNodeList items = itemsElement.childNodes();
	itemsData.reserve(items.size() + 1);

	//0 index is empty item
	itemsData.emplace_back();

	for (size_t i = 0; i < items.size(); i++)
	{
		QDomElement itemStatElement = items.at(i).firstChildElement();
		DotaItem item;

		item.name = itemStatElement.text().toStdString();
		itemStatElement = itemStatElement.nextSiblingElement();

		item.cost = itemStatElement.text().toFloat();
		itemStatElement = itemStatElement.nextSiblingElement();

		QDomElement itemBonusStatElement = itemStatElement;

		for ( ; 
			!itemBonusStatElement.isNull() && itemBonusStatElement.tagName() != "trait";
			itemBonusStatElement = itemBonusStatElement.nextSiblingElement())
		{
			DotaItem::ids bonusStat = itemsDispatchTable.at(itemBonusStatElement.tagName().toStdString());

			switch (bonusStat)
			{
			case DotaItem::ids::strength:
				item.strength = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::agility:
				item.agility = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::intellegence:
				item.intellegence = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::greenDamage:
				item.greenDamage = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::redPhysicalDamage:
				item.redMagickDamage = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::redMagickDamage:
				item.redMagickDamage = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::as:
				item.as = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::crit:
				item.crit = itemBonusStatElement.text().toFloat();
				break;
			case DotaItem::ids::critChance:
				item.critChance = itemBonusStatElement.text().toFloat();
				break;

			default:
				throw std::exception();
			}


		}

		for (QDomElement itemTraitElement = itemBonusStatElement;
			!itemTraitElement.isNull();
			itemTraitElement = itemTraitElement.nextSiblingElement())
		{
			QDomElement traitElement = itemTraitElement.firstChildElement();


			Trait::ids id = itemsTraitsDispatchTable.at(traitElement.tagName().toStdString());

			switch (id)
			{
			case Trait::ids::radiance:
			{
				Radiance trait{};
				QDomElement dpsElement = traitElement.firstChildElement();
				float dps = dpsElement.text().toFloat();
				trait.dps = dps;
				item.traits[id] = std::make_shared<Radiance>(trait);
				break;
			}

			default:
				throw std::exception();
				break;
			}

			itemsData.push_back(std::move(item));
		}
	}
}

void DotaCalculator::initHeroSelection()
{
	selectHeroLayout = new QHBoxLayout;

	selectHeroLabel = new QLabel("Select Hero:");
	selectHeroButton = new QPushButton("Tune");

	selectHeroLayout->addWidget(selectHeroLabel);
	selectHeroLayout->addStretch();
	selectHeroLayout->addWidget(selectHeroButton);

	selectHeroComboBox = new QComboBox;
	selectHeroComboBox->addItem("No Hero");

	for (size_t i = 1; i < heroesData.size(); i++)
	{
		selectHeroComboBox->addItem(heroesData[i].name.data());
	}
}

void DotaCalculator::initHeroDialog()
{
	heroDialog = new HeroDialog(this);

	connect(selectHeroButton, &QPushButton::clicked, this, &DotaCalculator::onHeroDialogOpen);
}

void DotaCalculator::initLevelSelection()
{
	selectLevelLabel = new QLabel("Select Level:");

	selectLevelSpinBox = new QSpinBox;
	selectLevelSpinBox->setRange(1, 30);
	selectLevelSpinBox->setSingleStep(1);
}

void DotaCalculator::initItemsSelection()
{
	itemsLayout = new QVBoxLayout;

	itemsGroupBox = new QGroupBox("Select Items:");

	for (auto& itemComboBox : itemsComboBoxes)
	{
		itemComboBox = new QComboBox;
		itemComboBox->addItem("No Item");
		for (size_t i = 1; i < itemsData.size(); i++)
		{
			itemComboBox->addItem(itemsData[i].name.data());
		}
		itemsLayout->addWidget(itemComboBox);
	}
	itemsGroupBox->setLayout(itemsLayout);
}

void DotaCalculator::initButtons()
{
	buttonsLayout = new QHBoxLayout;

	otherButton = new QPushButton("Other");
	otherButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	otherButton->setMinimumWidth(otherButton->sizeHint().width());

	calculateButton = new QPushButton("Calculate");
	connect(calculateButton, &QPushButton::clicked, this, &DotaCalculator::onCalculateClick);
	
	buttonsLayout->addWidget(otherButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(calculateButton);
	buttonsLayout->addStretch();
}

void DotaCalculator::initOtherDialog()
{
	otherDialog = new OtherDialog(this);

	connect(otherButton, &QPushButton::clicked, otherDialog, &OtherDialog::onDialogOpen);
}

void DotaCalculator::initDPSResult()
{
	dpsLayout = new QHBoxLayout;

	dpsLabel = new QLabel("DPS:");

	dpsResultLabel = new QLabel("0");

	dpsLayout->addWidget(dpsLabel);
	dpsLayout->addWidget(dpsResultLabel);
}

void DotaCalculator::initMainLayout()
{
	mainLayout = new QVBoxLayout;

	mainLayout->addLayout(selectHeroLayout);
	mainLayout->addWidget(selectHeroComboBox);

	mainLayout->addSpacing(10);

	mainLayout->addWidget(selectLevelLabel);
	mainLayout->addWidget(selectLevelSpinBox);

	mainLayout->addSpacing(10);

	mainLayout->addWidget(itemsGroupBox);

	mainLayout->addSpacing(50);

	mainLayout->addLayout(buttonsLayout);

	mainLayout->addSpacing(10);

	mainLayout->addLayout(dpsLayout);
}

void DotaCalculator::onCalculateClick()
{
	//0 index == no item/hero
	const DotaHero& hero = heroesData[selectHeroComboBox->currentIndex()];

	if (hero.name.empty()) 
		return;

	const DotaItem items[6] = 
	{
		itemsData[itemsComboBoxes[0]->currentIndex()],
		itemsData[itemsComboBoxes[1]->currentIndex()],
		itemsData[itemsComboBoxes[2]->currentIndex()],
		itemsData[itemsComboBoxes[3]->currentIndex()],
		itemsData[itemsComboBoxes[4]->currentIndex()],
		itemsData[itemsComboBoxes[5]->currentIndex()],
	};

	int level = selectLevelSpinBox->value();

	calculator.load(hero, level, items, heroDialog->getSkillsData(), heroDialog->getTraitsData());

	float result = calculator.calculate();

	dpsResultLabel->setNum(result);
	
}

void DotaCalculator::onHeroDialogOpen()
{
	const DotaHero& hero = heroesData[selectHeroComboBox->currentIndex()];

	if (hero.name.empty())
		// just return?
		return;

	heroDialog->onDialogOpen(hero);

}

