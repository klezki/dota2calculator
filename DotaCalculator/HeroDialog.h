#pragma once

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include "DotaHero/DotaHero.h"
#include <functional>


class HeroDialog : public QDialog
{
	Q_OBJECT

public:

	HeroDialog(QWidget* parent);
	void onDialogOpen(const DotaHero& hero);

private:
	void initMainLayout();

	QVBoxLayout* mainLayout;

	QLabel* setArmorLabel;
	QSpinBox* setArmorSpinBox;

	QCheckBox* aganimShardCheckBox;

	QCheckBox* aganimScepterCheckBox;

	QCheckBox* moonShardCheckBox;

	QPushButton* okButton;

	const DotaHero* pcurHero;

	

};
