#include "DotaCalculator/HeroDialog.h"

HeroDialog::HeroDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Tune hero");

	initMainLayout();

	setLayout(mainLayout);
	resize(200, 100);
}

void HeroDialog::initMainLayout()
{
	mainLayout = new QVBoxLayout;


}

void HeroDialog::onDialogOpen(const DotaHero& hero)
{
	pcurHero = &hero;
	open();
}