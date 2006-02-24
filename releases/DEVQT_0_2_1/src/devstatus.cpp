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

#include "devgui.h"

DevStatus::DevStatus(QWidget *p)
 : QStatusBar(p)
{
	QLabel *l;
	
	for ( int p = DevQt::General; p < DevQt::None; p++ )
	{
		l = new QLabel;
		l->setIndent(2);
		l->setMargin(2);
		l->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		l->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
		l->setMinimumWidth(fontMetrics().width("XXXXXX"));
		addWidget(l);
		v.prepend(l);
	}
	
	explorer = new QPushButton(QIcon(":/explorer.png"), tr("Explorer"));
	addPermanentWidget(explorer);
	
	compiler = new QPushButton(QIcon(":/compiler.png"), tr("Compiler"));
	addPermanentWidget(compiler);
	
	connect(this, SIGNAL( messageChanged(const QString&) ),
			this, SLOT  ( clearMessage() ) );
}

DevStatus::~DevStatus()
{
	foreach(QLabel *l, v)
	{
		removeWidget(l);
		delete l;
	}
	v.clear();
}

void DevStatus::connection()
{
	connect(explorer, SIGNAL( clicked() ),
			DEV_GUI	, SLOT  ( hideExplorer() ) );
	
	connect(compiler, SIGNAL( clicked() ),
			DEV_GUI	, SLOT  ( hideCompiler() ) );
}

void DevStatus::message(const QString& s, int p)
{
	if ( p >= DevQt::None )
		return;
	
	QLabel *l;
	
	if ( (l = v[p]) )
		l->setText(s);
	
}
	
