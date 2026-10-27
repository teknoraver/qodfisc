/****************************************************************************
 *  Qodfisc - A Qt based tool to calculate your fiscal code                 *
 *  Copyright (C) 2006-2007 Matteo Croce <rootkit85@yahoo.it>               *
 *                                                                          *
 *  This program is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ****************************************************************************/

#ifndef QODFISC_H
#define QODFISC_H

#include <QRegularExpression>

#include "ui_mainwindow.h"

class UpperCaser : public QValidator
{
Q_OBJECT
public:
	UpperCaser(QObject *p): QValidator(p) {}
	void fixup(QString&) const override {}

	State validate(QString &input, int&) const override
	{
		input = input.toUpper();
		if(input.isEmpty())
			return Intermediate;
		static const QRegularExpression alfanum(QRegularExpression::anchoredPattern("[\\p{L}' ]+"));
		if(!alfanum.match(input).hasMatch())
			return Invalid;
		return Acceptable;
	}
};

class QodFisc : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	QodFisc();
private slots:
	void provinciaSlot(const QString&) const;
	void calcolaSlot();
	void about();
private:
	const QString getCodice(const QString&, const QString&, QDate, bool, const QString&) const;
	const QString getNome(const QString&, bool) const;
	const QString getGS(int, bool) const;
	const QString getComune(const QString&, const QString&) const;
	const QString codControllo(const QString&) const;
};
#endif
