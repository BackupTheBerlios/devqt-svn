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

#include "matcher.h"

#include "coreedit.h"
#include "blockdata.h"

ParenMatcher::ParenMatcher(CoreEdit *parent)
 : QObject(parent), edit(parent)
{
	matchFormat.setFontWeight(QFont::Bold);
	matchFormat.setForeground(Qt::red);
	
	mismatchFormat.setBackground(QColor(255, 190, 190));
	mismatchFormat.setForeground(Qt::darkRed);
	mismatchFormat.setFontWeight(QFont::Bold);
	mismatchFormat.setFontItalic(true);
}

ParenMatcher::MatchType checkOpenParenthesis(QTextCursor *cursor, QChar c)
{
	BlockData *ud = BlockData::data(cursor->block());
	if (!ud)
		return ParenMatcher::NoMatch;
	
	QList<Parenthesis> parenList = ud->parentheses;
	Parenthesis openParen, closedParen;
	QTextBlock closedParenParag = cursor->block();
	
	const int cursorPos = cursor->position() - closedParenParag.position();
	int i = 0;
	int ignore = 0;
	bool foundOpen = false;
	forever {
		if (!foundOpen) {
			if (i >= parenList.count())
				return ParenMatcher::NoMatch;

			openParen = parenList.at(i);
			if (openParen.pos != cursorPos-1) {
				++i;
				continue;
			} else {
				foundOpen = true;
				++i;
			}
		}
		
		if (i >= parenList.count()) {
			forever {
				closedParenParag = closedParenParag.next();
				if (!closedParenParag.isValid())
					return ParenMatcher::NoMatch;
				
				if (BlockData::data(closedParenParag) && !BlockData::data(closedParenParag)->parentheses.isEmpty()) {
					parenList = BlockData::data(closedParenParag)->parentheses;
					break;
				}
			}
			i = 0;
		}
		
		closedParen = parenList.at(i);
		if (closedParen.type == Parenthesis::Open) {
		    ignore++;
		    ++i;
		    continue;
		} else {
		    if (ignore > 0) {
		        ignore--;
		        ++i;
		        continue;
		    }
		
		    cursor->clearSelection();
		    cursor->movePosition(QTextCursor::PreviousCharacter);
		    cursor->setPosition(closedParenParag.position() + closedParen.pos + 1, QTextCursor::KeepAnchor);
		
		    if (c == QLatin1Char('{') && closedParen.chr != QLatin1Char('}') || c == QLatin1Char('(') && closedParen.chr != QLatin1Char(')') || c == QLatin1Char('[') && closedParen.chr != QLatin1Char(']'))
				return ParenMatcher::Mismatch;
		
			return ParenMatcher::Match;
		}
	}
}

ParenMatcher::MatchType checkClosedParenthesis(QTextCursor *cursor, QChar c)
{
	BlockData *ud = BlockData::data(cursor->block());
	if (!ud)
		return ParenMatcher::NoMatch;
	
	QList<Parenthesis> parenList = ud->parentheses;
	Parenthesis openParen, closedParen;
	QTextBlock openParenParag = cursor->block();

	const int cursorPos = cursor->position() - openParenParag.position();
	int i = parenList.count() - 1;
	int ignore = 0;
	bool foundClosed = false;
	forever {
		if (!foundClosed) {
			if (i < 0)
				return ParenMatcher::NoMatch;
				
			closedParen = parenList.at(i);
			if (closedParen.pos != cursorPos - 1) {
				--i;
				continue;
			} else {
				foundClosed = true;
				--i;
			}
		}

		if (i < 0) {
			forever {
				openParenParag = openParenParag.previous();
				if (!openParenParag.isValid())
					return ParenMatcher::NoMatch;
		
				if (BlockData::data(openParenParag) && !BlockData::data(openParenParag)->parentheses.isEmpty()) {
					parenList = BlockData::data(openParenParag)->parentheses;
					break;
				}
			}
			i = parenList.count() - 1;
		}

		openParen = parenList.at(i);
		if (openParen.type == Parenthesis::Closed) {
			ignore++;
			--i;
			continue;
		} 
		else {
			if (ignore > 0) {
				ignore--;
				--i;
				continue;
			}

			cursor->clearSelection();
			cursor->setPosition(openParenParag.position() + openParen.pos, QTextCursor::KeepAnchor);

			if ( c == '}' && openParen.chr != '{' || c == ')' && openParen.chr != '(' || c == ']' && openParen.chr != '[' )
				return ParenMatcher::Mismatch;

			return ParenMatcher::Match;
		}
	}
}

ParenMatcher::MatchType ParenMatcher::match(QTextCursor *cursor)
{
    cursor->clearSelection();

    QChar leftChar = charFromCursor(*cursor, QTextCursor::PreviousCharacter);
    
    if (leftChar == '{' || leftChar == '(' || leftChar =='[')
        return checkOpenParenthesis(cursor, leftChar);
        
    if (leftChar == '}' || leftChar == ')' || leftChar == ']')
        return checkClosedParenthesis(cursor, leftChar);
	
    return NoMatch;
}

void ParenMatcher::matchCharacter(QLine *lineToPaint)
{
	QLine line;
	
	if ( !edit )
	{
		*lineToPaint = QLine();
		return;
	}

	int rehighlightStart = currentMatch.selectionStart();
	int rehighlightEnd = currentMatch.selectionEnd();
	if (rehighlightStart != rehighlightEnd) {
		QTextBlock block = edit->document()->findBlock(rehighlightStart);
		while (block.isValid() && block.position() < rehighlightEnd) {
			if (BlockData *d = BlockData::data(block)) {
				d->parenthesisMatchStart = -1;
				d->parenthesisMatchEnd = -1;
				d->parenthesisMatchingFormat = QTextCharFormat();

				clearMarkerFormat(block, ParenthesisMatcherPropertyId);
			}
			block = block.next();
		}
	}

	currentMatch = edit->textCursor();
	
	MatchType matchType = match(&currentMatch);
	
	if ( currentMatch.hasSelection() )
	{
        rehighlightStart = qMin(rehighlightStart, currentMatch.selectionStart());
        rehighlightEnd = qMax(rehighlightEnd, currentMatch.selectionEnd());

        const int startPos = currentMatch.selectionStart();
        const int endPos = currentMatch.selectionEnd();

        QTextCursor firstCursor(edit->document());
        QTextCursor lastCursor(edit->document());
        
        firstCursor.setPosition(startPos);
        lastCursor.setPosition(endPos - 1);
        
        if (lineToPaint)
        {
			QFontMetrics fm = edit->fontMetrics();
        	QRect first = edit->cursorRect(firstCursor);
        	QRect last = edit->cursorRect(lastCursor);
        	int x = qMin(first.x(), last.x()) + (fm.width(" ") / 2);
        	
        	if (firstCursor.block() != lastCursor.block())
	        	line = QLine(x, first.y() + fm.height(), x, last.y());
        }
        
		fixBlockData(firstCursor, matchType, true);
		fixBlockData(lastCursor, matchType, false);
    }

    if ( rehighlightStart != rehighlightEnd )
        edit->document()->markContentsDirty(rehighlightStart, rehighlightEnd + 1);
        
	if ( lineToPaint )
		*lineToPaint = line;
}

void ParenMatcher::fixBlockData(QTextCursor cursor, MatchType matchType, bool open)
{
	QTextBlock block = cursor.block();

	BlockData *d = BlockData::data(block);
	if (!d)
	{
		d = new BlockData;
		d->setToBlock(block);
	}

	if (matchType == Match)
		d->parenthesisMatchingFormat = matchFormat;
	else if (matchType == Mismatch)
		d->parenthesisMatchingFormat = mismatchFormat;

	d->parenthesisMatchingFormat.setProperty(ParenthesisMatcherPropertyId, true);

	QTextLayout *layout = block.layout();
	QList<QTextLayout::FormatRange> formats = layout->additionalFormats();
	QTextLayout::FormatRange r;
	
	if (open)
	{
		d->parenthesisMatchStart = cursor.position() - block.position();
		r.start = d->parenthesisMatchStart;
	} else {
		d->parenthesisMatchEnd = cursor.position() - block.position();
		r.start = d->parenthesisMatchEnd;
	}
	
	r.length = 1;
	r.format = d->parenthesisMatchingFormat;
	formats.append(r);
	layout->setAdditionalFormats(formats);
}

void ParenMatcher::clearMarkerFormat(const QTextBlock &block, int markerId)
{
	QTextLayout *layout = block.layout();
	QList<QTextLayout::FormatRange> formats = layout->additionalFormats();
	bool formatsChanged = false;
	
	for (int i = 0; i < formats.count(); ++i)
	{
		if (formats.at(i).format.hasProperty(markerId))
		{
			QTextCharFormat fmt = formats.at(i).format.property(markerId).value<QTextCharFormat>();
			formats[i].format = fmt;
			formats[i].format.clearProperty(markerId);
			
			if (formats.at(i).format.properties().isEmpty())
			{
				formats.removeAt(i);
				--i;
			}
			
			formatsChanged = true;
		}
	}

	if (formatsChanged)
		layout->setAdditionalFormats(formats);
}

bool ParenMatcher::collapse(QTextBlock block)
{
	QTextCursor linesToCollapse(block);
	QTextCursor linesToMatch(block);
	BlockData* bd = BlockData::data(block);
	
	if (!bd)
		return false;
		
	if (!bd->couldCollapse || bd->collapsedLines.size() > 0)
		return false;
		
	QString newLine = block.text();
	int firstMatch = newLine.indexOf('{');
	
	if (firstMatch != -1)
	{
		newLine = newLine.left(firstMatch);
		linesToMatch.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, firstMatch + 1);
	} else {
		bool found = false;
		linesToMatch.movePosition(QTextCursor::Down);
		linesToMatch.movePosition(QTextCursor::StartOfLine);
		
		forever
		{
			if (!linesToMatch.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor))
				return false;
				
			if (linesToMatch.selectedText() == "{")
			{
				linesToMatch.clearSelection();
				found = true;
				break;
			}
		}
	}

	MatchType matchType = match(&linesToMatch);
	if (linesToMatch.hasSelection() && matchType == Match)
	{
        const int endPos = linesToMatch.selectionEnd();

        QTextCursor lastCursor(edit->document());
        lastCursor.setPosition(endPos - 1);
        lastCursor.movePosition(QTextCursor::EndOfLine);
        if (linesToCollapse.block() == lastCursor.block())
        	return false;
        	
        linesToCollapse.setPosition(lastCursor.position(), QTextCursor::KeepAnchor);
        QList<TextLine> collapsedLines;
        
        for (QTextBlock line = block; line.isValid() && line.position() < lastCursor.position(); line = line.next())
        	collapsedLines << TextLine(line.text(), BlockData::data(line));

		int vValBackup = edit->verticalScrollBar()->value();
		int hValBackup = edit->horizontalScrollBar()->value();
        QTextCursor backup = edit->textCursor();
        linesToCollapse.removeSelectedText();
        linesToCollapse.insertText(newLine);
        
        QTextBlock newBlock = linesToCollapse.block();

        BlockData *newBD = BlockData::data(newBlock);
        
        if (newBD == 0)
        {
        	newBD = new BlockData();
        	newBD->setToBlock(newBlock);
        }
        
       	newBD->collapsedLines = collapsedLines;
       	newBD->couldCollapse = true;
        
		edit->setTextCursor(backup);
		edit->verticalScrollBar()->setValue(vValBackup);
		edit->horizontalScrollBar()->setValue(hValBackup);
		clearMarkerFormat(newBlock, ParenthesisMatcherPropertyId);
		
        return true;
    }
    
	return false;
}

bool ParenMatcher::expand(QTextBlock block)
{
	BlockData* bd = BlockData::data(block);
	BlockData newBD(*bd);
	
	if (!bd)
		return false;
		
	if (!bd->couldCollapse || !bd->collapsedLines.size() > 0)
		return false;
	
	int vValBackup = edit->verticalScrollBar()->value();
	int hValBackup = edit->horizontalScrollBar()->value();
	QTextCursor backup = edit->textCursor();
	QTextCursor toDelete(block);
	toDelete.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	toDelete.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
	toDelete.removeSelectedText();
	
	foreach(TextLine line, newBD.collapsedLines)
	{
		toDelete.insertText(line.text());
		toDelete.block().setUserData(line.data);
		toDelete.insertBlock();
	}
	
	edit->setTextCursor(backup);
	edit->verticalScrollBar()->setValue(vValBackup);
	edit->horizontalScrollBar()->setValue(hValBackup);
	return true;
}

bool ParenMatcher::isCollapsed(QTextBlock block)
{
	BlockData *data = BlockData::data(block);
	if (data)
		return (data->collapsedLines.size() > 0 && data->couldCollapse);
	else
		return false;
}

bool ParenMatcher::couldCollapse(QTextBlock block)
{
	BlockData *data = BlockData::data(block);
	if (data)
	{
		int parenMatch = 0;
		foreach(Parenthesis p, data->parentheses)
		{
			if (p.type == Parenthesis::Open && p.chr == '{')
				++parenMatch;
			else if (p.type == Parenthesis::Closed && p.chr == '}')
				--parenMatch;
		}
		
		return (data->couldCollapse && (parenMatch > 0)) || (data->couldCollapse && (data->collapsedLines.size() > 0));
	}
	else
		return false;
}

QRect ParenMatcher::currentBlock()
{
	QRect result;
	QTextCursor currentCursor = edit->textCursor();
	QTextBlock line = currentCursor.block();
	BlockData *data;
	int parenMatch = 0;
	
	forever
	{
		data = BlockData::data(line);
		if (data)
		{
			foreach(Parenthesis p, data->parentheses) 
			{
				if (line == currentCursor.block())
				{
					if (p.pos < (currentCursor.position() - line.position()))
					{
						if (p.type == Parenthesis::Open && p.chr == '{')
							++parenMatch;
						else if (p.type == Parenthesis::Closed && p.chr == '}')
							--parenMatch;
					}
				} else {
					if (p.type == Parenthesis::Open && p.chr == '{')
						++parenMatch;
					else if (p.type == Parenthesis::Closed && p.chr == '}')
						--parenMatch;
				}
			}
			
			if (parenMatch > 0)
				break;
		}

		line = line.previous();
		
		if (!line.isValid())
			break;
	}

	if (!line.isValid())
		return QRect();
		
	int parenPos = line.text().indexOf('{');
	
	if (data && data->couldCollapse && parenPos != -1)
	{
		++parenPos;
		QTextCursor linesToMatch(line);
		linesToMatch.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, parenPos);
		MatchType matchType = match(&linesToMatch);
		
		if (linesToMatch.hasSelection() && matchType == Match)
		{
			const int startPos = linesToMatch.selectionStart() + 1;
			const int endPos = linesToMatch.selectionEnd() - 1;
			
			if (currentCursor.position() >= startPos && currentCursor.position() <= endPos)
			{
				int lineHeight = edit->fontMetrics().height();
				linesToMatch.setPosition(startPos);
				QTextCursor lastCursor(edit->document());
				lastCursor.setPosition(endPos);
				result = edit->cursorRect(linesToMatch).unite(edit->cursorRect(lastCursor));
				
				if (result.height() > lineHeight)
				{
					result.setX(0);
					result.setWidth(edit->viewport()->width());
				}
				else
					result = QRect();
			}
		}
	}
	
	return result;
}

QColor ParenMatcher::matchColor()
{
	return matchFormat.foreground().color();
}

QStringList ParenMatcher::anotations(QTextBlock block)
{
	BlockData *data = BlockData::data(block);
	if (data)
		return data->anotations;
	else
		return QStringList();
}

