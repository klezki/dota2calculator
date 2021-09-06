#include <DotaCalculator/DotaCalculator.h>
#include <Traits/Traits.h>



namespace
{
	enum class heroBonusStats
	{
		greenDamage,
		redPhysicalDamage,
		redMagickDamage,
		bat,
		as,
		crit,
		critChance,

		traitCdCrit,
	};

	enum class itemBonusStats
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

	const std::unordered_map<std::string, heroBonusStats> heroesDispatchTable = {
		{ "greenDamage",  heroBonusStats::greenDamage},
		{ "redPhysicalDamage",  heroBonusStats::redPhysicalDamage},
		{ "redMagickDamage",  heroBonusStats::redMagickDamage},
		{ "bat",  heroBonusStats::bat},
		{ "as",  heroBonusStats::as},
		{ "crit",  heroBonusStats::crit},
		{ "critChance",  heroBonusStats::critChance},

		{ "traitCdCrit",  heroBonusStats::traitCdCrit},
	};

	const std::unordered_map<std::string, itemBonusStats> itemsDispatchTable = {
		{ "strength",  itemBonusStats::strength},
		{ "agility",  itemBonusStats::agility},
		{ "intellegence",  itemBonusStats::intellegence},
		{ "greenDamage",  itemBonusStats::greenDamage},
		{ "redPhysicalDamage",  itemBonusStats::redPhysicalDamage},
		{ "redMagickDamage",  itemBonusStats::redMagickDamage},
		{ "as",  itemBonusStats::as},
		{ "crit",  itemBonusStats::crit},
		{ "critChance",  itemBonusStats::critChance},

		{ "traitRadiance",  itemBonusStats::traitRadiance},
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

		hero.name = heroStatElement.text().toLocal8Bit().constData();
		heroStatElement = heroStatElement.nextSiblingElement();

		QString attribute = heroStatElement.text().toLocal8Bit().constData();
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


		for (QDomElement heroBonusStatElement = heroStatElement; !heroBonusStatElement.isNull(); heroBonusStatElement = heroBonusStatElement.nextSiblingElement())
		{
			try
			{
				heroBonusStats bonusStat = heroesDispatchTable.at(heroBonusStatElement.tagName().toLocal8Bit().data());

				switch (bonusStat)
				{
				case heroBonusStats::greenDamage:
					hero.greenDamage = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::redPhysicalDamage:
					hero.redPhysicalDamage = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::redMagickDamage:
					hero.redMagickDamage = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::bat:
					hero.bat = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::as:
					hero.as = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::crit:
					hero.crit = heroBonusStatElement.text().toFloat();
					break;
				case heroBonusStats::critChance:
					hero.critChance = heroBonusStatElement.text().toFloat();
					break;


				case heroBonusStats::traitCdCrit:
				{
					QDomElement cdElement = heroBonusStatElement.firstChildElement();
					float cd = cdElement.text().toFloat();
					hero.traits["traitCdCrit"] = CdCrit{ cd };
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

		item.name = itemStatElement.text().toLocal8Bit().constData();
		itemStatElement = itemStatElement.nextSiblingElement();

		item.cost = itemStatElement.text().toFloat();
		itemStatElement = itemStatElement.nextSiblingElement();

		for (QDomElement itemBonusStatElement = itemStatElement; !itemBonusStatElement.isNull(); itemBonusStatElement = itemBonusStatElement.nextSiblingElement())
		{
			try
			{
				itemBonusStats bonusStat = itemsDispatchTable.at(itemBonusStatElement.tagName().toLocal8Bit().data());

				switch (bonusStat)
				{
				case itemBonusStats::strength:
					item.strength = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::agility:
					item.agility = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::intellegence:
					item.intellegence = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::greenDamage:
					item.greenDamage = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::redPhysicalDamage:
					item.redMagickDamage = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::redMagickDamage:
					item.redMagickDamage = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::as:
					item.as = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::crit:
					item.crit = itemBonusStatElement.text().toFloat();
					break;
				case itemBonusStats::critChance:
					item.critChance = itemBonusStatElement.text().toFloat();
					break;


				case itemBonusStats::traitRadiance:
				{
					QDomElement dpsElement = itemBonusStatElement.firstChildElement();
					float dps = dpsElement.text().toFloat();
					item.traits["traitRadiance"] = Radiance{ dps };
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
	selectHeroLabel = new QLabel("Select Hero:");

	selectHeroComboBox = new QComboBox;
	selectHeroComboBox->addItem("No Hero");

	for (size_t i = 1; i < heroesData.size(); i++)
	{
		selectHeroComboBox->addItem(heroesData[i].name.data());
	}
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

	mainLayout->addWidget(selectHeroLabel);
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
	const DotaHero hero = heroesData[selectHeroComboBox->currentIndex()];

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

