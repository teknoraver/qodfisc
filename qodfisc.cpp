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

#include <QMessageBox>
#include <QDesktopWidget>

#include "qodfisc.h"
#include "comuni.h"


QodFisc::QodFisc(): QMainWindow()
{
	setupUi(this);

	UpperCaser *uc = new UpperCaser(this);
	nomeLE->setValidator(uc);
	cognomeLE->setValidator(uc);
	connect(actionInfo, SIGNAL(triggered()), SLOT(about()));
	connect(provinciaCB, SIGNAL(currentIndexChanged(const QString&)), SLOT(provinciaSlot(const QString&)));
	connect(calcola, SIGNAL(clicked()), SLOT(calcolaSlot()));
	centralwidget->setAutoFillBackground(true);
	move((QApplication::desktop()->screenGeometry(this).width() - width()) / 2, (QApplication::desktop()->screenGeometry(this).height() - height()) / 2);

	QString prevprov;
	for(int i = 0; i < comuni_length; i++) {
//		qDebug("prov=%s; comune=%s; codice=%s", buf.prov, buf.comune, buf.codice);
		if(prevprov != comuni[i].prov) {
			provinciaCB->addItem(comuni[i].prov);
			prevprov = comuni[i].prov;
		}
	}
}

void QodFisc::provinciaSlot(const QString &selected) const
{
	comuneCB->clear();
	int low = 0, high = comuni_length, i = (low + high) / 2;
	for(; low <= high; i = (low + high) / 2) {
		if(comuni[i].prov > selected)
			high = i - 1;
		else if(comuni[i].prov < selected)
			low = i + 1;
		else {
			for(; i > 0; i--)
				if(comuni[i - 1].prov != selected)
					break;
			break;
		}
	}
	for(; i < comuni_length; i++) {
		if(comuni[i].prov != selected)
			return;
		comuneCB->addItem(comuni[i].comune);
	}
}



const QString QodFisc::getNome(const QString &nome) const
{
	QString strnome;
	for(int i = 0; i < nome.length() && strnome.length() < 3; i++)
		switch(nome[i].toLatin1()) {
			case 'A':
			case 'E':
			case 'I':
			case 'O':
			case 'U': break;
			default: strnome += nome[i];
		}
	if(strnome.length() < 3) {
		for(int i = 0; i < nome.length() && strnome.length() < 3; i++)
			switch(nome[i].toLatin1()) {
				case 'A':
				case 'E':
				case 'I':
				case 'O':
				case 'U': strnome += nome[i];
			}
		if(strnome.length() < 3) {
			const char *padd[] = {"XXX", "XX", "X"};
			strnome += padd[strnome.length()];
		}
	}

// 	qDebug(qPrintable("NOMCOG: " + strcognome+strnome));
	return strnome;
}

const QString QodFisc::getGS(int giorno, bool femmina) const
{
	int gs = giorno + (40 * femmina);

	if(gs < 10)
		return '0' + QString::number(gs);

	return QString::number(gs);
}

const QString QodFisc::getComune(const QString &prov, const QString &comune) const
{
	int low = 0, high = comuni_length, i = (low + high) / 2;
	for(; low <= high; i = (low + high) / 2) {
		if(comuni[i].prov > prov)
			high = i - 1;
		else if(comuni[i].prov < prov)
			low = i + 1;
		else
			break;
	}
//	qDebug("bsearch stopped at %s", comuni[i].comune);
	if(comuni[i].comune < comune) {
		for(; i < comuni_length; i++)
			if(comuni[i].comune == comune)
				return comuni[i].codice;
	} else if(comuni[i].comune > comune)
		for(; i >= 0; i--)
			if(comuni[i].comune == comune)
				return comuni[i].codice;
	return comuni[i].codice;
}

const QString QodFisc::codControllo(const QString &cod_fis) const
{
	int somma = 0;
	for(int i = 0; i < 15; i++)
		if(i % 2)
			somma += cod_fis[i].toLatin1() - (cod_fis[i].isDigit() ? '0' : 'A');
		else
				switch(cod_fis[i].toLatin1()) {
				case '0':
				case 'A': somma += 1; break;
				case '1':
				case 'B': somma += 0; break;
				case '2':
				case 'C': somma += 5; break;
				case '3':
				case 'D': somma += 7; break;
				case '4':
				case 'E': somma += 9; break;
				case '5':
				case 'F': somma += 13; break;
				case '6':
				case 'G': somma += 15; break;
				case '7':
				case 'H': somma += 17; break;
				case '8':
				case 'I': somma += 19; break;
				case '9':
				case 'J': somma += 21; break;
				case 'K': somma += 2; break;
				case 'L': somma += 4; break;
				case 'M': somma += 18; break;
				case 'N': somma += 20; break;
				case 'O': somma += 11; break;
				case 'P': somma += 3; break;
				case 'Q': somma += 6; break;
				case 'R': somma += 8; break;
				case 'S': somma += 12; break;
				case 'T': somma += 14; break;
				case 'U': somma += 16; break;
				case 'V': somma += 10; break;
				case 'W': somma += 22; break;
				case 'X': somma += 25; break;
				case 'Y': somma += 24; break;
				case 'Z': somma += 23; break;
			}
	return cod_fis + (somma % 26 + 'A');
}

void QodFisc::calcolaSlot()
{
	QMessageBox::information(this, "Codice Fiscale",
	getCodice(
		cognomeLE->text(),
		nomeLE->text(),
		dataDE->date(),
		sexF->isChecked(),
		comuneCB->currentText()));
}

const QString QodFisc::getCodice(const QString &cognome, const QString &nome, QDate data, bool femmina, const QString &comune) const
{
	const char *indexMesi = "ABCDEHLMPRST";

	QString anno = QString::number(data.year() % 100);
	if(anno.length() == 1)
		anno.prepend('0');
	return codControllo(
		getNome(cognome) +
		getNome(nome) +
		anno +
		indexMesi[data.month() - 1] +
		getGS(data.day(), femmina) +
		getComune(provinciaCB->currentText(), comune));
}

void QodFisc::about()
{
	QMessageBox::about(this, "Informazioni su Qodfisc", "<b>Qodfisc</b><br><br>Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a><br>Natale Vinto <a href=\"http://gotext.org/\">http://gotext.org/</a>");
}
