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

#include "devstandardpanel.h"

#include "coreedit.h"
#include "foldingpanel.h"
#include "linemarkspanel.h"
#include "linenumberpanel.h"

DevStandardPanel::DevStandardPanel(CoreEdit *e)
 : QFrame(0), editor(e)
{
	QHBoxLayout *box = new QHBoxLayout;
	
	box->setMargin(0);
	box->setSpacing(0);
	
	f = new FoldingPanel(e);
	m = new LineMarksPanel(e);
	l = new LineNumberPanel(e);
	
	box->addWidget(m);
	box->addWidget(l);
	box->addWidget(f);
	
	setLayout(box);
	
	setFrameShadow(QFrame::Sunken);
	setFrameShape(QFrame::StyledPanel);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

DevStandardPanel::~DevStandardPanel()
{
	;
}

void DevStandardPanel::mousePressEvent(QMouseEvent *e)
{
	e->accept();
	
	QPoint p = editor->viewport()->mapFromGlobal(e->globalPos());
	QTextCursor c( editor->cursorForPosition(p) );
	
	clicked(c);
	
	int i = editor->line(c);
	
	if ( i != -1 )
		clicked(i);
	
	update(); //update widget
}

