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

#include "linemarkspanel.h"

#include "coreedit.h"
#include "blockdata.h"

LineMarksPanel::LineMarksPanel(CoreEdit *e, QWidget *p)
 : DevEditPanel(e, p)
{
	setFixedWidth(32);
}

void LineMarksPanel::paintEvent(QPaintEvent *e)
{
	const QPixmap pm[] ={ 	
							QPixmap(":/break.png"),
							QPixmap(":/error.png")
						};
	
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
		
		BlockData *dat = BlockData::data(block);
		if ( !dat )
			continue;
		
		int x = 0;
		
		if ( dat->s & BlockData::BreakPoint )
		p.drawPixmap(	15*(x++) + 2,
						posY - contentsY +(fm.lineSpacing() - (*pm).height())/2,
						(*pm).width(),
						(*pm).height(),
						*pm);
		
		if ( dat->s & BlockData::Error )
		p.drawPixmap(	15*(x++) + 2,
						posY - contentsY +(fm.lineSpacing() - pm[1].height())/2,
						pm[1].width(),
						pm[1].height(),
						pm[1]);
		
	}
}
