#include"SettingWidget.h"

SettingDialog::SettingDialog(QDialog *parent):QDialog(parent)
{
	ui.setupUi(this);
}
void SettingDialog::SetVersion(QString ver)
{
	ui.label_version->setText(ver);
}

void SettingDialog::SetValue(int normalZ, int offsetSetp, float interval, int smooth, bool saveNormal, bool withMark, float thresholdZ, 
	bool isManual, bool showBur, double cutOffset, int& curveture)
{
	ui.OffsetStepBox->setValue(offsetSetp);
	ui.doubleSpinBox_cutlineInterval->setValue(interval);
	ui.smoothBox->setValue(smooth);
	ui.zangleBox->setValue(normalZ);
	ui.zthresholdBox->setValue(thresholdZ);

	ui.withNormalButton->setChecked(saveNormal);
	ui.withoutButton->setChecked(!saveNormal);
	ui.checkBoxMark->setChecked(withMark);

	ui.checkBox_showBur->setChecked(showBur);
	ui.doubleSpinBox_cutOffset->setValue(cutOffset);
	ui.spinBox_curveture->setValue(curveture);

	if (isManual) {
		ui.radioButton_manual->setChecked(true);
	}
	else {
		ui.radioButton_auto->setChecked(true);
	}

}
void SettingDialog::GetValue(int &normalZ, int &offsetSetp, float &interval,
	int &smooth, bool &saveNormal, bool &withMark, float &thresholdZ, bool& isManual, 
	bool& showBur, double& cutOffset, int& curveture)
{
	normalZ = ui.zangleBox->value();
	offsetSetp = ui.OffsetStepBox->value();
	interval = ui.doubleSpinBox_cutlineInterval->value();
	smooth = ui.smoothBox->value();
	thresholdZ = ui.zthresholdBox->value();
	if (ui.withNormalButton->isChecked())
		saveNormal = true;
	if (ui.withoutButton->isChecked()) {
		saveNormal = false;
	}
	if (ui.checkBoxMark->isChecked())
		withMark = true;
	else
		withMark = false;

	if (ui.radioButton_manual->isChecked()) {
		isManual = true;
	}
	else {
		isManual = false;
	}

	cutOffset = ui.doubleSpinBox_cutOffset->value();
	showBur   = ui.checkBox_showBur->isChecked();
	curveture = ui.spinBox_curveture->value();
}

void SettingDialog::SetDegreeValue(int degreefb, int cutDegreeOut, int cutDegreeInsideFront, int cutDegreeInsideBack)
{
	ui.spinBox_fbDegree->setValue(degreefb);
	ui.spinBox_cutDegreeOut->setValue(cutDegreeOut);
	ui.spinBox_cutDegreeInsideFront->setValue(cutDegreeInsideFront);
	ui.spinBox_cutDegreeInsideBack->setValue(cutDegreeInsideBack);
}

void SettingDialog::GetDegreeValue(int& degreefb, int& cutDegreeOut, int& cutDegreeInsideFront, int& cutDegreeInsideBack)
{
	degreefb = ui.spinBox_fbDegree->value();
	cutDegreeOut = ui.spinBox_cutDegreeOut->value();
	cutDegreeInsideFront = ui.spinBox_cutDegreeInsideFront->value();
	cutDegreeInsideBack = ui.spinBox_cutDegreeInsideBack->value();
}

void SettingDialog::SetFixtureValue(bool need_qr, int fixture, int correction, float qr_width)
{
	if (fixture == 0) {
		ui.radioButton_5or4Axis->setChecked(true);
	}
	else if (fixture == 1) {
		ui.radioButton_Robotic->setChecked(true);
	}
	else if (fixture == 2) {
		ui.radioButton_5or4Axis_2->setChecked(true);
	}
	else if (fixture == 3) {
		ui.radioButton_5or4Axis_3->setChecked(true);
	}
	else if (fixture == 4) {
		ui.radioButton_5or4Axis_4->setChecked(true);
	}

	if (need_qr) {
		ui.radioButton_QR->setChecked(true);
	}
	else {
		ui.radioButton_Info->setChecked(true);
	}

	ui.comboBox_correction->setCurrentIndex(correction);
	ui.doubleSpinBox_qrwidth->setValue(qr_width);
}

void SettingDialog::GetFixtureValue(bool& need_qr, int& fixture, int& correction, float& qr_width)
{
	need_qr = ui.radioButton_QR->isChecked();
	if (ui.radioButton_5or4Axis->isChecked()) {
		fixture = 0;
	}
	else if (ui.radioButton_Robotic->isChecked()) {
		fixture = 1;
	}
	else if (ui.radioButton_5or4Axis_2->isChecked()) {
		fixture = 2;
	}
	else if (ui.radioButton_5or4Axis_3->isChecked()) {
		fixture = 3;
	}
	else if (ui.radioButton_5or4Axis_4->isChecked()) {
		fixture = 4;
	}

	qr_width = ui.doubleSpinBox_qrwidth->value();
	correction = ui.comboBox_correction->currentIndex();
}
