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
		{ "crit",  Skill::ids::crit},
		{ "critChance",  Skill::ids::critChance},
	};

	const std::unordered_map<std::string, Trait::traitIds> heroesTraitsDispatchTable = {
		{ "cdCrit",  Trait::traitIds::cdCrit},
		{ "crit",  Trait::traitIds::crit},
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

		{ "traitRadiance",  DotaItem::ids::traitRadiance},
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
			QDomElement heroBonusStatElement = heroSkillElement.firstChildElement();

			Skill::ids bonusStat = heroesDispatchTable.at(heroBonusStatElement.tagName().toStdString());

			Skill skill{};
			skill.id = bonusStat;
			QDomElement bonusStatElement = heroBonusStatElement.firstChildElement();

			for (int i = 0; i < SKILL_NUM; i++) {
				skill.val[i] = bonusStatElement.text().toFloat();

				bonusStatElement = bonusStatElement.nextSiblingElement();
			}

			if (!bonusStatElement.isNull() && bonusStatElement.tagName() == "talent") {
				skill.talent = bonusStatElement.text().toFloat();
				skill.flag = talentFlag::off;
			}
			
			hero.skills.push_back(std::move(skill));


		}

		for (QDomElement heroTraitElement = heroSkillElement;
			!heroTraitElement.isNull();
			heroTraitElement = heroTraitElement.nextSiblingElement())
		{
			QDomElement traitElement = heroTraitElement.firstChildElement();


			Trait::traitIds id = heroesTraitsDispatchTable.at(traitElement.tagName().toStdString());

			switch (id)
			{
			case Trait::traitIds::crit:
			{
				Crit trait{};

				QDomElement critElement = traitElement.firstChildElement();
				QDomElement critChanceElement = critElement.nextSiblingElement();

				for (int i = 0; i < SKILL_NUM; i++) {
					trait.crit[i] = critElement.text().toFloat();
					trait.critChance[i] = critChanceElement.text().toFloat();

					critElement = critElement.nextSiblingElement();
					critChanceElement = critChanceElement.nextSiblingElement();
				}
				if (!critElement.isNull() && critElement.tagName() == "talent")
				{
					trait.critTalent = critElement.text().toFloat();
					trait.critFlag = talentFlag::off;
				}
				if (!critChanceElement.isNull() && critChanceElement.tagName() == "talent")
				{
					trait.critChanceTalent = critChanceElement.text().toFloat();
					trait.critChanceFlag = talentFlag::off;
				}				
				hero.traits["crit"] = std::make_shared<Crit>(trait);
				break;
			}				
			case Trait::traitIds::cdCrit:
				break;
			default:
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

		for (QDomElement itemBonusStatElement = itemStatElement; !itemBonusStatElement.isNull(); itemBonusStatElement = itemBonusStatElement.nextSiblingElement())
		{
			try
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


				case DotaItem::ids::traitRadiance:
				{
					//QDomElement dpsElement = itemBonusStatElement.firstChildElement();
					//float dps = dpsElement.text().toFloat();
					//item.traits["traitRadiance"] = Radiance{ dps };
					break;
				}

				default:
					throw std::exception();
				}

			}

			catch (std::out_of_range e)
			{
				//TODO : add something
				throw;
			}
		}

		itemsData.push_back(std::move(item));
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

	//mainLayout->addWidget(selectHeroLabel);
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

	calculator.load(hero, level, items);

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

