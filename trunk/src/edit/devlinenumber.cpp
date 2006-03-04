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

#include "devlinenumber.h"

/*

DevLineNumber : the widget I've been looking for since I discovered Qt.

Draws line numbers according to attached text editor size and scrollbar's 
position.
React to left clicks and draws breakpoints img and so on...

*/

DevLineNumber::DevLineNumber(QTextEdit *_editor, QWidget *parent)
    : QWidget(parent), editor(_editor)
{
    if ( !editor )
    	return;
    
	setFixedWidth(fontMetrics().width(QLatin1String("000000") + 35));
    
    connect(editor->document()->documentLayout(), SIGNAL(update(const QRectF &)),
            this, SLOT(update()));
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(update()));
}

void DevLineNumber::paintEvent(QPaintEvent * /*e*/)
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
		const QString txt = QString::number(n);
		
		p.drawText(	0,
					posY - contentsY,
					width(),
					fm.lineSpacing(),
					Qt::AlignRight | Qt::AlignVCenter,
					txt);
		
		if ( !dat )
			continue;
		
		int x = 0;
		
		if ( dat->s & BlockData::BreakPoint )
		p.drawPixmap(	15*(x++),
						posY - contentsY +(fm.lineSpacing() - (*pm).height())/2,
						(*pm).width(),
						(*pm).height(),
						*pm);
		
		if ( dat->s & BlockData::Error )
		p.drawPixmap(	15*(x++),
						posY - contentsY +(fm.lineSpacing() - pm[1].height())/2,
						pm[1].width(),
						pm[1].height(),
						pm[1]);
	}
}

void DevLineNumber::mousePressEvent(QMouseEvent *e)
{
	e->accept();
	
	QPoint p = editor->viewport()->mapFromGlobal(e->globalPos());
	QTextCursor c( editor->cursorForPosition(p) );
	
	clicked(c);
	
	int i = DevQt::line(editor->document(), c.block());
	
	if ( i!=-1 )
		clicked(i);
	
	update(); //update widget
}
