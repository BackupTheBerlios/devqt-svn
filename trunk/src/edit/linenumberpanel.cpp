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
** Notes :	- the LineNumberPanel widget is based on the LineNumberWidget provided
** by QSA 1.2.0 (C) Trolltech, as modifications have been done to make it comply
** my requirements, LineNumberPanel has to be considered as a derivative work of
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

#include "linenumberpanel.h"

#include "coreedit.h"

/*

LineNumberPanel : the widget I've been looking for since I discovered Qt.

Draws line numbers according to attached text editor size and scrollbar's 
position.

*/

LineNumberPanel::LineNumberPanel(CoreEdit *e, QWidget *p)
 : DevEditPanel(e, p)
{
	setFixedWidth(fontMetrics().width(QLatin1String("0000")));
}

void LineNumberPanel::paintEvent(QPaintEvent *e)
{
	int n = 1, contentsY = editor->verticalScrollBar()->value();
	qreal pageBottom = contentsY + editor->viewport()->height();
	const QFontMetrics fm( editor->document()->defaultFont() );
	
	QPainter p(this);
	
	for (QTextBlock block = editor->document()->begin();
		block.isValid(); 
		block = block.next(), ++n )
	{
		QTextLayout *layout = block.layout();
		
		const QRectF boundingRect = layout->boundingRect();
		QPointF position = layout->position();
		if (position.y() + boundingRect.height() < contentsY)
			continue;
		if (position.y() > pageBottom)
			break;
			
		int posY = qRound(position.y());
            
		const QString txt = QString::number(n);
		
		p.drawText(	0,
					posY - contentsY,
					width(),
					fm.lineSpacing(),
					Qt::AlignRight | Qt::AlignVCenter,
					txt);
		
	}
}

