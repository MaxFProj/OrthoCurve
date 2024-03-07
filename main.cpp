#include "orthocurve.h"
#include <QtWidgets/QApplication>

//#define USE_LICENSE_MANAGE
#ifdef  USE_LICENSE_MANAGE
#include "LicenseManager\QtLicenseManager.h"
#include "LicenseManager\emxMessageHandler.h"
#endif

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

#ifdef USE_LICENSE_MANAGE
	emxMessageHandler* msgHandler = new emxMessageHandler;
	QtLicenseManager::init(); // init a license manager
	LicenseManager::instance()->setMessageHandler(msgHandler);
	char errInfo[256];
	sprintf(errInfo, "%s", QApplication::translate("QtLicenseManager", "Failed to get license! Please contact to your software provider and get a license to continue to use this software.", nullptr).toLocal8Bit().data()); 
	if (LicenseManager::instance()->check(errInfo) != LM_SUCCESS)
	{
		QMessageBox::critical(nullptr, QApplication::translate("QtLicenseManager", "Error", 0), QString::fromLocal8Bit(errInfo));
		exit(0);
	}
#endif

	OrthoCurve w;
	w.Init();

	w.showMaximized();
	return a.exec();
}
