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

#include "normalstate.h"

#include "matcher.h"
#include "coreedit.h"
#include "indenter.h"
#include "multilinestate.h"
#include "persistentstate.h"

NormalState* NormalState::inst = 0;

NormalState* NormalState::Instance()
{
	if ( !inst )
		inst = new NormalState;
	
	return inst;
}

NormalState::NormalState()
{
	s = Normal;
}

NormalState::~NormalState()
{
}

QString NormalState::name()
{
	return tr("normal edition");
}

void NormalState::paintEvent(CoreEdit *ctxt, QPaintEvent *e)
{
	QPainter p( ctxt->viewport() );
	
	int xOffset = ctxt->horizontalScrollBar()->value() + 4;
	int yOffset = ctxt->verticalScrollBar()->value();
	
	if ( ctxt->highlightCurrentLine() )
	{
		QRect r(0,
				ctxt->cursorRect().y(),
				ctxt->viewport()->width(),
				QFontMetrics( ctxt->document()->defaultFont() ).lineSpacing() );
		
		p.fillRect(r, QColor(0x00, 0xff, 0xff, 0x30));
	}
	
	if ( ctxt->drawMargin() )
	{
		int margin = QFontMetrics(ctxt->font()).width(" ") * (ctxt->margin() + 1);
		
		margin -= xOffset;
		
		p.drawLine(	margin, 0,
					margin, ctxt->viewport()->height());
	}
	
	//I was forced to do these standard painting afterward so as to get rid
	//of some scrollbars bugs!!!
	
	QRect r = e->rect();
	p.translate(-xOffset, -yOffset);
	r.translate(xOffset, yOffset);
	
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = ctxt->palette();
	
	if ( ctxt->bCursor && ctxt->isEnabled() && ctxt->hasFocus() )
		ctx.cursorPosition = ctxt->textCursor().position();
	
	if ( ctxt->parenMatching() && !ctxt->curLine.isNull() )
	{
		QPen backupPen = p.pen();
		
		QPen newPen(Qt::DotLine);
		newPen.setWidth(0);
		newPen.setColor((ctxt->pMatcher ? ctxt->pMatcher->matchColor() : Qt::black));
		p.setPen(newPen);	
		
		const QFontMetrics fm = ctxt->fontMetrics();
		const int fontHeight = fm.height();
		const int x = ctxt->curLine.x1();// + (fm.width(" ") / 2);
		
		int y1 = ctxt->curLine.y1() + 1;
		int y2 = 0;
		
		for (int y = y1; y < ctxt->curLine.y2(); y += fontHeight)
		{
			QTextCursor cursor = ctxt->cursorForPosition(QPoint(x, y));	
			QChar ch = ParenMatcher::charFromCursor(cursor,
													QTextCursor::NextCharacter);
			
			if (ch.isSpace())
				y2 = y + fontHeight;
			
			else
			{
				QChar ch2 = ParenMatcher::charFromCursor(cursor,
												QTextCursor::PreviousCharacter);
				if (ch2 == '\t')
				{
					ch2 = ParenMatcher::charFromCursor(
									ctxt->cursorForPosition(QPoint(
												x - (ctxt->tabStopWidth() / 2),
												y) ),
									QTextCursor::NextCharacter);
					
					if (ch2 == '\t')
						y2 = y + fontHeight;
					else
					{
						if (y1 < y2)
						{
							QLine lineToPrint(x, y1, x, y2);
							lineToPrint.translate(xOffset, yOffset);
							p.drawLine(lineToPrint);
						}
						y1 = y + fontHeight;
					}
				} else {
					if (y1 < y2)
					{
						QLine lineToPrint(x, y1, x, y2);
						lineToPrint.translate(xOffset, yOffset);
						p.drawLine(lineToPrint);
					}
					y1 = y + fontHeight;
				}
			}
		}
		
		if (y1 < y2)
		{
			QLine lineToPrint(x, y1, x, y2);
			lineToPrint.translate(xOffset, yOffset);
			p.drawLine(lineToPrint);
		}
		
		p.setPen(backupPen);
	}
		
	p.setClipRect(r);
	ctx.clip = r;
	
	paintSelection(ctxt, p, xOffset, yOffset, ctx);
	
	ctxt->document()->documentLayout()->draw(&p, ctx);
}

void NormalState::timerEvent(CoreEdit *ctxt, QTimerEvent *e)
{
	if ( e->timerId() == ctxt->blinker.timerId() )
	{
		ctxt->bCursor = !ctxt->bCursor;
		
		if ( ctxt->textCursor().hasSelection() )
			ctxt->bCursor &= ctxt->style()->styleHint(
											QStyle::SH_BlinkCursorWhenTextSelected,
											0,
											ctxt) != 0;
		
		ctxt->viewport()->update(ctxt->cursorRect());
	}
}

void NormalState::keyPressEvent(CoreEdit *ctxt, QKeyEvent *e)
{
	QTextCursor cur = ctxt->textCursor();
	
	if ( e->modifiers() & Qt::ControlModifier )
	{
		switch ( e->key() )
		{
			case Qt::Key_Left :
			{
				cur.movePosition(	QTextCursor::Left,
									QTextCursor::MoveAnchor);
				
				if ( !ctxt->CtrlNavigation() )
					break;
				
				QTextBlock b = cur.block();
				
				QString txt = b.text().left(cur.position()-b.position());
				int i = txt.size();
				
				while ( !txt.at(--i).isUpper() && (i >= 0) )
				{
					if ( txt.at(i).isSpace() )
					{
						while ( txt.at(i).isSpace() )
						{
							cur.movePosition(	QTextCursor::Left,
												QTextCursor::MoveAnchor);
							i--;
						}
						
						cur.movePosition(	QTextCursor::Right,
											QTextCursor::MoveAnchor,
											2);
						
						break;
					}
						
					cur.movePosition(	QTextCursor::Left,
										QTextCursor::MoveAnchor);
				}
			
				cur.movePosition(	QTextCursor::Left,
									QTextCursor::MoveAnchor);
				
				ctxt->setTextCursor(cur);
				
				break;
			}
			
			case Qt::Key_Right :
			{
				cur.movePosition(	QTextCursor::Right,
									QTextCursor::MoveAnchor);
				
				if ( !ctxt->CtrlNavigation() )
					break;
				
				QTextBlock b = cur.block();
				QString txt = b.text().mid(cur.position() - b.position());
				int i = 0, len = txt.size();
				
				while ( !txt.at(i).isUpper() && (i < len) )
				{
					if ( txt.at(i).isSpace() )
					{
						cur.movePosition(	QTextCursor::WordRight,
											QTextCursor::MoveAnchor);
						break;
					}
					
					cur.movePosition(	QTextCursor::Right,
										QTextCursor::MoveAnchor);
					i++;
				}
				
				ctxt->setTextCursor(cur);
				
				break;
			}
			
			case Qt::Key_Asterisk :
			{
				ctxt->changeState( PersistentState::Instance() );
				break;
			}
			
			case Qt::Key_Slash :
			{
				ctxt->changeState( MultilineState::Instance() );
				break;
			}
			
			default:
				ctxt->QTextEdit::keyPressEvent(e);
				break;
		}
	} else {
		switch ( e->key() )
		{
			case Qt::Key_Insert :
			{	
				bool m = !ctxt->overwriteMode();
				ctxt->setOverwriteMode(m);
				
				if ( m )
					emit message("Overwrite", (int)DevQt::TypingMode);
				else
					emit message("Insert", (int)DevQt::TypingMode);
			}
			
			case Qt::Key_Enter :
			case Qt::Key_Return :
			{
				QString tabs = ctxt->pIndenter->spaces(	cur,
														ctxt->replaceTab(),
														ctxt->tabstop()
														);
				
				cur.insertBlock();
				cur.insertText(tabs);
				
				ctxt->setTextCursor(cur);
				
				break;
			}
			
			case Qt::Key_BraceRight :
				
				ctxt->pIndenter->backspace(	cur, ctxt->tabstop(),
											Qt::Key_BraceRight );
				
				cur.insertText("}");
				ctxt->setTextCursor(cur);
				
				break;
			
			case Qt::Key_BraceLeft :
			case Qt::Key_ParenLeft :
			case Qt::Key_BracketLeft :
				
				ctxt->QTextEdit::keyPressEvent(e);
				
				if ( !ctxt->autoClose() )
					break;
				
				break;
			
			case Qt::Key_Tab :
			{
				QString txt;
				
				if ( ctxt->replaceTab() )
					txt.fill(' ', ctxt->tabstop());
				else
					txt = "\t";
				
				cur.insertText(txt);
				ctxt->setTextCursor(cur);
				
				break;
			}
				
			default:
				ctxt->QTextEdit::keyPressEvent(e);
		}
	}
		
	//ctxt->viewport()->update();
	
	signalTextCursor(ctxt);
}

void NormalState::dropEvent(CoreEdit *ctxt, QDropEvent *e)
{
	ctxt->QTextEdit::dropEvent(e);
}

void NormalState::dragMoveEvent(CoreEdit *ctxt, QDragMoveEvent *e)
{
	ctxt->QTextEdit::dragMoveEvent(e);
}

void NormalState::dragEnterEvent(CoreEdit *ctxt, QDragEnterEvent *e)
{
	ctxt->QTextEdit::dragEnterEvent(e);
}

void NormalState::dragLeaveEvent(CoreEdit *ctxt, QDragLeaveEvent *e)
{
	ctxt->QTextEdit::dragLeaveEvent(e);
}
		
void NormalState::mouseMoveEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	QString row, col;
	const QFontMetrics fm(ctxt->font());
	const QPoint p = ctxt->viewport()->mapFromGlobal(e->globalPos());
	
	QTextCursor cur = ctxt->cursorForPosition(p);
	
	int x, y = ctxt->line(cur);
	
	if ( y != -1 )
	{
		x = ctxt->column(cur);
		
		col = QString::number(x);
		row = QString::number(y);
	}
	else
	{
		col = row = "-1";
	}
	
	emit message(QString("Mouse : Row %1 Column %2").arg(row).arg(col),
			(int)DevQt::MouseCursor);
	
	if ( e->buttons() )
	{
		//ctxt->viewport()->update();
		signalTextCursor(ctxt);
	}
	
	ctxt->QTextEdit::mouseMoveEvent(e);
}

void NormalState::mousePressEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	//ctxt->viewport()->update();
	
	ctxt->QTextEdit::mousePressEvent(e);
	
	signalTextCursor(ctxt);
}

void NormalState::mouseReleaseEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	//ctxt->viewport()->update();
	
	ctxt->QTextEdit::mouseReleaseEvent(e);
	
	signalTextCursor(ctxt);
}

void NormalState::mouseDoubleClickEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	ctxt->viewport()->update();
	ctxt->QTextEdit::mouseDoubleClickEvent(e);
}

QMimeData* NormalState::createMimeDataFromSelection(const CoreEdit *ctxt) const
{
	QMimeData *md = new QMimeData;
	
	md->setText(ctxt->textCursor().selectedText());
	
	return md;
}

void NormalState::insertFromMimeData(CoreEdit *ctxt, const QMimeData *md)
{
	if ( !md )
		return;
	
	QTextCursor cur = ctxt->textCursor();
	
	if ( !md->text().isNull() )
		cur.insertText(md->text());
	else if ( !md->html().isNull() )
		cur.insertText(md->html());			//insert plain text
	
	ctxt->setTextCursor(cur);
	
	ctxt->ensureCursorVisible();
}

void NormalState::paintSelection(CoreEdit *e, QPainter &p,
					int xOffset, int yOffset,
					QAbstractTextDocumentLayout::PaintContext &cxt)
{
	EditorState::paintSelection(e, p, xOffset, yOffset, e->textCursor(), cxt);
}

void NormalState::signalTextCursor(CoreEdit *ctxt)
{
	if ( ctxt->hasFocus() )
	{
		ctxt->pMatcher->matchCharacter(&ctxt->curLine);
	} else {
		ctxt->pMatcher->matchCharacter(0);
		ctxt->curLine = QLine();
	}
	
	ctxt->viewport()->update();
	
	QTextCursor cur = ctxt->textCursor();
	
	int x = ctxt->column(cur), y = ctxt->line(cur);
	
	QString col = QString::number(x);
	QString row = QString::number(y);
	
	emit message(QString("Text : Row %1 Column %2").arg(row).arg(col),
				(int)DevQt::TextCursor);
}
