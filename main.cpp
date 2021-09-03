#pragma once

#include <iostream>
#include <QtWidgets/qapplication.h>
#include <QtCore/QtCore>
#include "GUI.hpp"

int main(int argc, char* argv[])
{
	printf("Qt Version: %s\n", qVersion());

	QApplication app(argc, argv);
	GUI win;

	return app.exec();
}
