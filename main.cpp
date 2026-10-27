// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2006-2007 Matteo Croce <rootkit85@yahoo.it>

#include <QApplication>
#include "qodfisc.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QodFisc win;
	win.show();
	return app.exec();
}
