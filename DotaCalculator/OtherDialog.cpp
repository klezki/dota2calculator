#include "DotaCalculator/OtherDialog.h"

OtherDialog::OtherDialog(QWidget* parent) 
	: QDialog(parent)
{
	setWindowTitle("Other options");

	initMainLayout();

	setLayout(mainLayout);
	resize(200, 100);
}

void OtherDialog::initMainLayout()
{
	mainLayout = new QGridLayout;

	setArmorLabel = new QLabel("Select Enemy Armor:");
	
	setArmorSpinBox = new QSpinBox;
	setArmorSpinBox->setRange(-100, 100);
	setArmorSpinBox->setSingleStep(1);

	aganimShardCheckBox = new QCheckBox("Aghanim Shard");

	aganimScepterCheckBox = new QCheckBox("Aghanim Scepter");

	moonShardCheckBox = new QCheckBox("Moon Shard");

	okButton = new QPushButton("OK");
	okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(okButton, &QPushButton::clicked, this, &OtherDialog::done);

	mainLayout->addWidget(setArmorLabel, 0, 0);

	mainLayout->addWidget(setArmorSpinBox, 1, 0);

	mainLayout->addWidget(aganimShardCheckBox, 2, 0);

	mainLayout->addWidget(aganimScepterCheckBox, 3, 0);

	mainLayout->addWidget(moonShardCheckBox, 4, 0);

	mainLayout->addWidget(okButton, 5, 0, Qt::AlignCenter);
}

void OtherDialog::onDialogOpen()
{
	exec();
}