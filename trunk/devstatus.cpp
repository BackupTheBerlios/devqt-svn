/****************************************************************************
**
** Copyright (C) 2006 FullMetalCoder
**
** This file is part of the DevQt project (beta version)
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** Notes :	- the DevLineNumber widget is based on the LineNumberWidget provided
** by QSA 1.2.0 (C) Trolltech, as modifications have been done to make it comply
** my requirements, DevLineNumber has to be considered as a derivative work of
** the original widget from the Trolls.
**			- the DevHighlighter class is based on the QSyntaxHighlighter class
** provided by QSA 1.2.0 (C) Trolltech, as modifications have been done to make
** it comply my requirements (and simplify its use!), DevHighlighter has to be
** considered as a derivative work of the original class from the Trolls.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "devstatus.h"

DevStatus::DevStatus(QWidget *p)
 : QStatusBar(p)
{
	l = new QLabel;
	l->setIndent(2);
	l->setMargin(2);
	addPermanentWidget(l);
}

DevStatus::DevStatus(const QString& s, Qt::Alignment a, QWidget *p)
 : QStatusBar(p)
{
	l = new QLabel(s);
	l->setIndent(2);
	l->setMargin(2);
	l->setAlignment(a);
	addPermanentWidget(l);
}

DevStatus::~DevStatus()
{
	removeWidget(l);
	delete l;
}

void DevStatus::message(const QString& s, Qt::Alignment a)
{
	l->setText(s);
	l->setAlignment(a);
}
	
