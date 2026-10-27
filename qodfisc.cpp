// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2006-2007 Matteo Croce <rootkit85@yahoo.it>

#include <QClipboard>
#include <QFontDatabase>
#include <QLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>

#include "qodfisc.h"
#include "comuni.h"


QodFisc::QodFisc(): QMainWindow()
{
	setupUi(this);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	// the health insurance card, with rounded corners, is the background;
	// the image has twice the resolution of the window, for HiDPI screens
	QPixmap tessera(":/tessera.jpg");
	qreal dpr = tessera.width() / qreal(sfondo->width());
	QPixmap bg(tessera.size());
	bg.fill(Qt::transparent);
	QPainter p(&bg);
	p.setRenderHint(QPainter::Antialiasing);
	QPainterPath bordo;
	bordo.addRoundedRect(bg.rect(), 36 * dpr, 36 * dpr);
	p.setClipPath(bordo);
	p.drawPixmap(0, 0, tessera);
	p.end();
	bg.setDevicePixelRatio(dpr);
	sfondo->setPixmap(bg);

	const QList<QWidget *> campi = {cfLE, cognomeLE, nomeLE, comuneCB, provinciaCB};
	for (QWidget *w : campi) {
		QPalette pal = w->palette();
		pal.setColor(QPalette::PlaceholderText, QColor(27, 61, 122, 130));
		w->setPalette(pal);
	}

	QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	mono.setPointSizeF(font().pointSizeF() * 1.4);
	mono.setLetterSpacing(QFont::AbsoluteSpacing, 1);
	cfLE->setFont(mono);
	copia = cfLE->addAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy, QIcon(":/copia.png")), QLineEdit::TrailingPosition);
	copia->setToolTip("Copia il codice fiscale");
	copia->setEnabled(false);

	UpperCaser *uc = new UpperCaser(this);
	nomeLE->setValidator(uc);
	cognomeLE->setValidator(uc);

	QString prevprov;
	for (int i = 0; i < comuni_length; i++) {
		if (prevprov != comuni[i].prov) {
			provinciaCB->addItem(comuni[i].prov);
			prevprov = comuni[i].prov;
		}
	}
	// the province must be chosen first, then the comune
	provinciaCB->setCurrentIndex(-1);

	connect(actionInfo, &QAction::triggered, this, &QodFisc::about);
	connect(copia, &QAction::triggered, this, &QodFisc::copiaSlot);
	connect(provinciaCB, &QComboBox::currentTextChanged, this, &QodFisc::provinciaSlot);
	connect(comuneCB, &QComboBox::currentIndexChanged, this, &QodFisc::aggiornaSlot);
	connect(cognomeLE, &QLineEdit::textChanged, this, &QodFisc::aggiornaSlot);
	connect(nomeLE, &QLineEdit::textChanged, this, &QodFisc::aggiornaSlot);
	connect(dataDE, &QDateEdit::dateChanged, this, &QodFisc::aggiornaSlot);
	connect(sexF, &QRadioButton::toggled, this, &QodFisc::aggiornaSlot);
}

void QodFisc::provinciaSlot(const QString &selected) const
{
	comuneCB->clear();
	int low = 0, high = comuni_length, i = (low + high) / 2;
	for (; low <= high; i = (low + high) / 2) {
		if (comuni[i].prov > selected)
			high = i - 1;
		else if (comuni[i].prov < selected)
			low = i + 1;
		else {
			for (; i > 0; i--)
				if (comuni[i - 1].prov != selected)
					break;
			break;
		}
	}
	for (; i < comuni_length; i++) {
		if (comuni[i].prov != selected)
			break;
		comuneCB->addItem(comuni[i].comune);
	}
	comuneCB->setCurrentIndex(-1);
	comuneCB->setPlaceholderText("scegli il comune");
	comuneCB->setEnabled(true);
}



const QString QodFisc::getNome(const QString &str, bool nome) const
{
	QString consonanti, vocali;

	// decompose accented letters and skip spaces and apostrophes
	for (QChar c : str.toUpper().normalized(QString::NormalizationForm_D)) {
		if (c < u'A' || c > u'Z')
			continue;
		if (QStringLiteral("AEIOU").contains(c))
			vocali += c;
		else
			consonanti += c;
	}

	// first names with at least 4 consonants take the 1st, 3rd and 4th
	if (nome && consonanti.length() >= 4)
		return QString(consonanti[0]) + consonanti[2] + consonanti[3];

	return (consonanti + vocali + "XXX").left(3);
}

const QString QodFisc::getGS(int giorno, bool femmina) const
{
	int gs = giorno + (40 * femmina);

	if (gs < 10)
		return '0' + QString::number(gs);

	return QString::number(gs);
}

const QString QodFisc::getComune(const QString &prov, const QString &comune) const
{
	int low = 0, high = comuni_length, i = (low + high) / 2;
	for (; low <= high; i = (low + high) / 2) {
		if (comuni[i].prov > prov)
			high = i - 1;
		else if (comuni[i].prov < prov)
			low = i + 1;
		else
			break;
	}
//	qDebug("bsearch stopped at %s", comuni[i].comune);
	if (comuni[i].comune < comune) {
		for (; i < comuni_length; i++)
			if (comuni[i].comune == comune)
				return comuni[i].codice;
	} else if (comuni[i].comune > comune)
		for (; i >= 0; i--)
			if (comuni[i].comune == comune)
				return comuni[i].codice;
	return comuni[i].codice;
}

const QString QodFisc::codControllo(const QString &cod_fis) const
{
	int somma = 0;
	for (int i = 0; i < 15; i++)
		if (i % 2)
			somma += cod_fis[i].toLatin1() - (cod_fis[i].isDigit() ? '0' : 'A');
		else
			switch (cod_fis[i].toLatin1()) {
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
	return cod_fis + QChar(somma % 26 + 'A');
}

void QodFisc::aggiornaSlot()
{
	if (cognomeLE->text().isEmpty() || nomeLE->text().isEmpty() || comuneCB->currentIndex() < 0)
		cfLE->clear();
	else
		cfLE->setText(getCodice(
			cognomeLE->text(),
			nomeLE->text(),
			dataDE->date(),
			sexF->isChecked(),
			comuneCB->currentText()));
	copia->setEnabled(!cfLE->text().isEmpty());
}

void QodFisc::copiaSlot()
{
	QGuiApplication::clipboard()->setText(cfLE->text());
	QToolTip::showText(cfLE->mapToGlobal(QPoint(cfLE->width(), 0)), "Copiato", cfLE);
}

const QString QodFisc::getCodice(const QString &cognome, const QString &nome, QDate data, bool femmina, const QString &comune) const
{
	const char *indexMesi = "ABCDEHLMPRST";

	QString anno = QString::number(data.year() % 100);
	if (anno.length() == 1)
		anno.prepend('0');
	return codControllo(
		getNome(cognome, false) +
		getNome(nome, true) +
		anno +
		indexMesi[data.month() - 1] +
		getGS(data.day(), femmina) +
		getComune(provinciaCB->currentText(), comune));
}

void QodFisc::about()
{
	QMessageBox::about(this, "Informazioni su Qodfisc", "<b>Qodfisc</b><br><br>Matteo Croce <a href=\"https://matteocroce.it/\">https://matteocroce.it/</a><br>Natale Vinto <a href=\"https://natalevinto.it/\">https://natalevinto.it/</a>");
}
