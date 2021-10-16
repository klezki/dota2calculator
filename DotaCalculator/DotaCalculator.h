#pragma once

#include <QApplication>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLayout>
#include <QPushButton>
#include "DotaCalculator/OtherDialog.h"
#include "DotaCalculator/HeroDialog.h"
#include "FileManager/FileManager.h"
#include "DotaHero/DotaHero.h"
#include "DotaItem/DotaItem.h"
#include "DPSCalculator/DPSCalculator.h"
#include <Traits/Traits.h>



class DotaCalculator : public QWidget
{
	Q_OBJECT

public:
	DotaCalculator();
	DotaCalculator(QWidget*) = delete;

public slots:
	void onCalculateClick();
	void onHeroDialogOpen();

private:
	const char* HEROES_XML_PATH = "heroes.xml";
	const char* HEROES_XSD_PATH = "heroes.xsd";

	const char* ITEMS_XML_PATH = "items.xml";
	const char* ITEMS_XSD_PATH = "items.xsd";

	void initHeroData();
	void initItemData();
	void initHeroSelection();
	void initHeroDialog();
	void initLevelSelection();
	void initItemsSelection();
	void initButtons();
	void initOtherDialog();
	void initDPSResult();
	void initMainLayout();

	QVBoxLayout* mainLayout;

	QHBoxLayout* selectHeroLayout;
	QLabel* selectHeroLabel;
	QPushButton* selectHeroButton;
	QComboBox* selectHeroComboBox;

	HeroDialog* heroDialog;

	QLabel* selectLevelLabel;
	QSpinBox* selectLevelSpinBox;

	QGroupBox* itemsGroupBox;
	QVBoxLayout* itemsLayout;
	QComboBox* itemsComboBoxes[6];

	QHBoxLayout* buttonsLayout;
	QPushButton* otherButton;
	QPushButton* calculateButton;

	OtherDialog* otherDialog;

	QHBoxLayout* dpsLayout;
	QLabel* dpsLabel;
	QLabel* dpsResultLabel;

	QDomElement heroesElement;
	std::vector<DotaHero> heroesData;

	QDomElement itemsElement;
	std::vector<DotaItem> itemsData;

	DPSCalculator calculator;

};