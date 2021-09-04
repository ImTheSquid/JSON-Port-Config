#include <iostream>
#include <QtWidgets/qapplication.h>
#include <QtCore/QtCore>
#include "GUI.hpp"

int main(int argc, char* argv[])
{
	printf("Qt Version: %s\n", qVersion());

#ifdef __linux__
	// Fix issue with fonts not loading properly on Linux
	setenv("FONTCONFIG_PATH", "/etc/fonts", 0);
#endif

	QApplication app(argc, argv);
	GUI win;

	return app.exec();
}
