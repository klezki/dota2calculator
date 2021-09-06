#pragma once

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>


class OtherDialog : public QDialog
{
	Q_OBJECT

public:
	
	OtherDialog(QWidget* parent);

public slots:

	void onDialogOpen();

private:
	void initMainLayout();

	QGridLayout* mainLayout;

	QLabel* setArmorLabel;
	QSpinBox* setArmorSpinBox;

	QCheckBox* aganimShardCheckBox;

	QCheckBox* aganimScepterCheckBox;

	QCheckBox* moonShardCheckBox;

	QPushButton* okButton;

};