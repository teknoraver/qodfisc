// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2006-2007 Matteo Croce <rootkit85@yahoo.it>

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
	void aggiornaSlot();
	void copiaSlot();
	void about();
private:
	QAction *copia;
	const QString getCodice(const QString&, const QString&, QDate, bool, const QString&) const;
	const QString getNome(const QString&, bool) const;
	const QString getGS(int, bool) const;
	const QString getComune(const QString&, const QString&) const;
	const QString codControllo(const QString&) const;
};
#endif
