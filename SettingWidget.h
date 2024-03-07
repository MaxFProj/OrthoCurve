#pragma once

#include <QWidget>
#include "ui_DialogButtonBottom.h"

class SettingDialog : public QDialog
{
	Q_OBJECT
public:
	SettingDialog(QDialog *parent = 0);
	~SettingDialog() {};
	
	void SetVersion(QString ver);
	void SetValue(int normalZ, int offsetSetp, float interval, int smooth, bool saveNormal,bool withMark, float thresholdZ, 
		bool isManual, bool showBur, double cutOffset, int& curveture);
	void GetValue(int &normalZ, int &offsetSetp, float &interval, int &smooth, bool &saveNormal, bool &withMark, float &thresholdZ, bool& isManual, 
		bool& showBur, double& cutOffset, int& curveture);
	void SetDegreeValue(int degreefb, int cutDegreeOut, int cutDegreeInsideFront, int cutDegreeInsideBack);
	void GetDegreeValue(int& degreefb, int& cutDegreeOut, int& cutDegreeInsideFront, int& cutDegreeInsideBack);

	void SetFixtureValue(bool need_qr, int fixture, int correction, float qr_width);
	void GetFixtureValue(bool& need_qr, int& fixture, int& correction, float& qr_width);
public:
	Ui::Dialog ui;

};