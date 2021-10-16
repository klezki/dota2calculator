#pragma once

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include "DotaHero/DotaHero.h"
#include <functional>
#include <tuple>


class HeroDialog : public QDialog
{
	Q_OBJECT

public:

	HeroDialog(QWidget* parent);
	void onDialogOpen(const DotaHero& hero);
	void createSkillWidgets(const DotaHero& hero);
	void createTraitWidgets(const DotaHero& hero);
	void deleteWidgets();
	skillDataVector getSkillsData();
	traitDataVector getTraitsData();

private:
	void initMainLayout();

	QVBoxLayout* mainLayout;

	std::string heroName;

	std::vector<std::tuple<QLabel*, QSpinBox*, QCheckBox*>> skillWidgets;
	std::vector<std::tuple<Trait::ids, QLabel*, QSpinBox*, QCheckBox*>> traitWidgets;

	

};
