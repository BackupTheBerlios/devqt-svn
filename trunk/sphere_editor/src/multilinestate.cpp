#include "multilinestate.h"
#include "editor.h"

#include <QMimeData>
#include <QDrag>
#include <QTimerEvent>
#include <QScrollBar>
#include <QPaintEvent>
#include <QAbstractTextDocumentLayout>
#include <QPalette>
#include <QPen>
#include <QApplication>
#include <QClipboard>

#include <QtDebug>

MultilineState::MultilineState(Editor* context) : EditorState(context) {
	_mousePressed = false;
	_mightStartDrag = false;
	_doNothing = false;
	_position = None;
}

MultilineState::~MultilineState() {
}

void MultilineState::initialize() {
	QTextCursor cursor = editor()->textCursor();
	if (cursor.hasSelection())	
		createLinesFromCursor();
	else	
		_multilineCursors.append(cursor);
		
	editor()->updateView(true);
}

void MultilineState::destroy() {
	selectFromMultiLine();
	_multilineCursors.clear();
}

void MultilineState::paintEvent(QPaintEvent *e) {
	QPainter p(editor()->viewport());
	p.setFont(editor()->font());
	
	const int xOffset = editor()->horizontalScrollBar()->value() + 4;
	const int yOffset = editor()->verticalScrollBar()->value();
	
	QRect r = e->rect();
	p.translate(-xOffset, -yOffset);
	r.translate(xOffset, yOffset);
	p.setClipRect(r);
	
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = editor()->palette();
    QTextCursor cursor = editor()->textCursor();
    
	if (!_dndFeedbackCursor.isNull())
		ctx.cursorPosition = _dndFeedbackCursor.position();
	
	if (hasSelection()) {
		foreach(QTextCursor mlCursor, _multilineCursors) {
			QAbstractTextDocumentLayout::Selection selection;
			selection.cursor = mlCursor;
			selection.format.setBackground(editor()->palette().brush(QPalette::Highlight));
			selection.format.setForeground(editor()->palette().brush(QPalette::HighlightedText));
			ctx.selections.append(selection);
		}
	}

	ctx.clip = r;
	editor()->document()->documentLayout()->draw(&p, ctx);

	if (editor()->isEnabled()) {
		foreach(QTextCursor drawedCursor, _multilineCursors)
			drawCursor(p, drawedCursor, false);
	}
}

void MultilineState::mousePressEvent(QMouseEvent *e) {
	if (e->button() & Qt::LeftButton) {
		_mousePressed = true;
		_mightStartDrag = false;
		
		_oldPosition = e->pos();
		_clickedPosition = editor()->cursorForPosition(_oldPosition);
		
		if (hasSelection()) {
			foreach(QTextCursor selectedCursor, _multilineCursors) {
				if (selectedCursor.selectionStart() <= _clickedPosition.position() && _clickedPosition.position() <= selectedCursor.selectionEnd()) {
	                _mightStartDrag = true;
	                _dragStartTimer.start(QApplication::startDragTime(), this);
	                return;
				}
			}
		}

		_multilineCursors.clear();
		_multilineCursors.append(_clickedPosition);
		_position = None;
		updateEditorCursor();
		
		editor()->updateView(true);
	}
}

void MultilineState::mouseMoveEvent(QMouseEvent *e) {
	if (_mousePressed) {
		editor()->updateView(true);
		_mousePressed = false;
	}
	
	if (!_mightStartDrag) {
		QTextCursor movedCursor = editor()->cursorForPosition(e->pos());
		int sourceCol = editor()->getColumn(_clickedPosition.block(), _clickedPosition.position() - _clickedPosition.block().position());
		int newColumn = editor()->getColumn(movedCursor.block(), movedCursor.position() - movedCursor.block().position());
		
		int dif = _oldPosition.x() - e->pos().x();
		if (dif < 0) dif *= -1;
		
		if (dif <= (editor()->fontMetrics().width(' ') / 2) && sourceCol != newColumn) {
			movedCursor = editor()->cursorForPosition(_oldPosition);
		}
		else {
			_oldPosition = e->pos();
		}
		
		_clickedPosition.setPosition(movedCursor.position(), QTextCursor::KeepAnchor);
		createLinesFromCursor(_clickedPosition);
	}
	editor()->updateView();
}

void MultilineState::mouseReleaseEvent(QMouseEvent *e) {
	editor()->updateView(true);
	_clickedPosition = QTextCursor();
	
	if (hasSelection())
		emit copyAvailable(true);
	else
		emit copyAvailable(false);	
}

void MultilineState::mouseDoubleClickEvent(QMouseEvent *e) {
	QTextCursor cursor = editor()->cursorForPosition(e->pos());
	cursor.select(QTextCursor::WordUnderCursor);
	_multilineCursors.clear();
	_multilineCursors.append(cursor);
	updateEditorCursor();
	editor()->updateView(true);
	emit copyAvailable(true);
}

void MultilineState::dragMoveEvent(QDragMoveEvent *e) {
	if (editor()->isReadOnly() || !canInsertFromMimeData(e->mimeData())) {
		e->ignore();
		return;
	}

	int cursorPos = editor()->document()->documentLayout()->hitTest(QPoint(e->pos().x() + editor()->horizontalScrollBar()->value(), e->pos().y() + editor()->verticalScrollBar()->value()), Qt::FuzzyHit);
	if (cursorPos != -1) {
		_dndFeedbackCursor = _multilineCursors.front();
		_dndFeedbackCursor.setPosition(cursorPos);
	}

	editor()->updateView(true);
	e->acceptProposedAction();
}

void MultilineState::dropEvent(QDropEvent *e) {
	if (editor()->isReadOnly() || !canInsertFromMimeData(e->mimeData()))
		return;
	
	e->acceptProposedAction();
	
	if (e->dropAction() == Qt::MoveAction && (e->source() == editor() || e->source() == editor()->viewport()))
		deleteText();
	
	_multilineCursors.clear();
	_multilineCursors.append(_dndFeedbackCursor);
	_dndFeedbackCursor = QTextCursor();
	
	paste(e->mimeData());

	editor()->updateView(true);
}

void MultilineState::setPersistentSelection(bool yes) {
	if (yes)
		changeState(Editor::MultilinePersistent);
}

bool MultilineState::isPersistentSelection() {
	return false;
}

void MultilineState::setMultilineEdit(bool yes) {
	if (!yes)
		changeState(Editor::Normal);
}

bool MultilineState::isMultilineEdit() {
	return true;
}

QMimeData* MultilineState::createMimeDataFromSelection() {
	QMimeData* data = new QMimeData();
	QString text = "";
	foreach(QTextCursor cursor, _multilineCursors) {
		text += cursor.selectedText();
		if (cursor != _multilineCursors.back())
			text += "\n";
	}

	data->setText(text);
	return data;
}

QRect MultilineState::selectionRect() {
	QTextCursor c1 = _multilineCursors.front();
	c1.setPosition(c1.selectionStart());
	QTextCursor c2 = _multilineCursors.back();
	c2.setPosition(c2.selectionEnd());
	
	return editor()->cursorRect(c1).unite(editor()->cursorRect(c2));
}

bool MultilineState::hasSelection() {
	if (_multilineCursors.size() == 0)
		return false;
	else {
		if (_position == Bottom)
			return _multilineCursors.back().hasSelection();
		else
			return _multilineCursors.front().hasSelection();
	}
}

void MultilineState::deleteText() {
	foreach(QTextCursor cursor, _multilineCursors)
		cursor.removeSelectedText();
}

void MultilineState::deleteText(QTextCursor* masterCursor) {
	foreach(QTextCursor cursor, _multilineCursors) {
		masterCursor->setPosition(cursor.anchor());
		masterCursor->setPosition(cursor.position(), QTextCursor::KeepAnchor);
		masterCursor->removeSelectedText();
	}
}

void MultilineState::paste(const QMimeData *data) {
	QStringList lines = data->text().split('\n');
	QTextCursor masterCursor(editor()->document());
	masterCursor.beginEditBlock();
	
	int column = editor()->getColumn(_multilineCursors.back().block(), _multilineCursors.back().position() - _multilineCursors.back().block().position());
	int size = 0;
	int i, lastIdx;
	
	foreach(QString lineToPaste, lines) {
		if (lineToPaste.length() > size)
			size = lineToPaste.length();
	}
	
	if (hasSelection())
		deleteText(&masterCursor);
	
	for (i = 0, lastIdx = 0; i < lines.size(); ++i) {
		if (i < _multilineCursors.size()) {
			masterCursor.setPosition(_multilineCursors[i].position());
			masterCursor.insertText(lines[i]);
			for (int j = 0; j < (size - lines[i].size()); ++j)
				masterCursor.insertText(" ");
				
			lastIdx = i;
		}
		else {
			QTextBlock block = _multilineCursors.back().block();
			int lastCursorPos = _multilineCursors.back().position();
			for (int j = 0; j < (i - lastIdx); ++j) {
				QTextBlock newLine = block.next();
				if (!newLine.isValid()) {
					masterCursor.setPosition(block.position());
					masterCursor.movePosition(QTextCursor::EndOfBlock);
					masterCursor.insertBlock();
					if (_multilineCursors.back().position() != lastCursorPos)
						_multilineCursors.back().setPosition(lastCursorPos);
						
					newLine = masterCursor.block();
				}
				block = newLine;
			}
			masterCursor.setPosition(block.position());
			int pos = editor()->getPosition(block, column);
			masterCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
			int cursorColumn = editor()->getColumn(block, masterCursor.position() - block.position());
			if (cursorColumn < column) {
				for (int k = 0; k < (column - cursorColumn); ++k)
					masterCursor.insertText(" ");
			}
			masterCursor.insertText(lines[i]);
			for (int j = 0; j < (size - lines[i].size()); ++j)
				masterCursor.insertText(" ");
		}
	}
	for (; i < _multilineCursors.size(); ++i) {
		masterCursor.setPosition(_multilineCursors[i].position());
		for (int j = 0; j < size; ++j)
			masterCursor.insertText(" ");
	}
	masterCursor.endEditBlock();
}

void MultilineState::paste() {
	paste(QApplication::clipboard()->mimeData(QClipboard::Clipboard));
}

void MultilineState::selectAll() {
	QTextCursor cursor(editor()->document());
	cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	createLinesFromCursor(cursor);
}

void MultilineState::keyPressEvent(QKeyEvent *e) {
	switch(e->key()) {
		case Qt::Key_Home:
		case Qt::Key_Left:
			_movement = Left;
			EditorState::keyPressEvent(e);
			updateCursor();
			break;
		case Qt::Key_End:
		case Qt::Key_Right:
			_movement = Right;
			EditorState::keyPressEvent(e);
			updateCursor();
			break;
		case Qt::Key_PageUp:
		case Qt::Key_Up:
			_movement = Up;
			EditorState::keyPressEvent(e);
			updateCursor();
			break;
		case Qt::Key_PageDown:
		case Qt::Key_Down:
			_movement = Down;
			EditorState::keyPressEvent(e);
			updateCursor();
			break;

		case Qt::Key_Backspace:
			if (hasSelection())
				deleteText();
			else {
				if (couldMoveLeft())
					removeChar(QTextCursor::PreviousCharacter);
			}
			setCursorOn();
				
			break;
			
		case Qt::Key_Delete:
			if (hasSelection())
				deleteText();
			else {
				if (couldMoveRight())
					removeChar(QTextCursor::NextCharacter);
			}
			setCursorOn();
				
			break;

		case Qt::Key_Tab:
			if (editor()->replaceTabWithSpaces()) {
				for (int i = 0; i < editor()->tabstop(); ++i)
					insertText(" ");
			}
			else
				insertText("\t");

			setCursorOn();
			break;

		default:
			_movement = None;
			if (!e->text().isEmpty() && e->text().at(0).isPrint()) {
				insertText(e->text());
				setCursorOn();
			}
			else
				EditorState::keyPressEvent(e);
	}
}

void MultilineState::timerEvent(QTimerEvent *e) {
	if (e->timerId() == _dragStartTimer.timerId()) {
        _dragStartTimer.stop();

		_mousePressed = false;
		QMimeData *data = createMimeDataFromSelection();
		
		QDrag *drag = new QDrag(editor());
		drag->setMimeData(data);
		
		Qt::DropActions actions = Qt::CopyAction;
		if (!editor()->isReadOnly())
			actions |= Qt::MoveAction;
			
		Qt::DropAction action = drag->start(actions);
		
		if (action == Qt::MoveAction && drag->target() != editor()) {
			foreach(QTextCursor cursor, _multilineCursors)
				cursor.removeSelectedText();
		}

	}
}

void MultilineState::updateCursor() {
	QTextCursor textCursor = editor()->textCursor();
	
	if (textCursor.hasSelection()) {
		if (_movement == Left)
			moveLeft(QTextCursor::KeepAnchor, textCursor.selectedText().length());
		else if (_movement == Right)
			moveRight(QTextCursor::KeepAnchor, textCursor.selectedText().length());
		else if (_movement == Down)
			selectDown();
		else if (_movement == Up)
			selectUp();
	}
	else {
		if (_movement == Left) {
			int lastPos;
			if (_position == Bottom)
				lastPos = _multilineCursors.back().position();
			else
				lastPos = _multilineCursors.front().position();;
				
			int firstPos = textCursor.position();
				
			moveLeft(QTextCursor::MoveAnchor, lastPos - firstPos);
		}
		else if (_movement == Right) {
			int firstPos;
			if (_position == Bottom)
				firstPos = _multilineCursors.back().position();
			else
				firstPos = _multilineCursors.front().position();

			int lastPos = textCursor.position();
				
			moveRight(QTextCursor::MoveAnchor, lastPos - firstPos);
		}
		else if (_movement == Up || _movement == Down) {
			if (_multilineCursors.size() == 1)
				_multilineCursors[0].setPosition(textCursor.position());
			else {
				QTextCursor backup;
				if (_position == Top)
					backup = _multilineCursors.front();
				else
					backup = _multilineCursors.back();
					
				QTextBlock blk = backup.block();
				int position = editor()->getColumn(blk, backup.position() - blk.position());

				if (_movement == Up)
					blk = blk.previous();
				else
					blk = blk.next();
					
				if (blk.isValid() & blk != backup.block()) {
					backup.setPosition(blk.position() + editor()->getPosition(blk, position));
					_multilineCursors.clear();
					_multilineCursors.append(backup);
					_position = None;
				}
			}
		}
	}
	
	updateEditorCursor();
	if (hasSelection())
		emit copyAvailable(true);
	else
		emit copyAvailable(false);	
	
}

bool MultilineState::couldMoveLeft(bool selection) {
	if (_multilineCursors.size() == 1 && !selection)
		return true;
		
	foreach(QTextCursor cursor, _multilineCursors) {
		QTextCursor selectedCursor = cursor;
		selectedCursor.select(QTextCursor::LineUnderCursor);
		if (cursor.position() == selectedCursor.selectionStart())
			return false;
	}
	
	return true;
}

bool MultilineState::couldMoveRight(bool selection) {
	if (_multilineCursors.size() == 1 && !selection)
		return true;

//	int maxLength = -1;
	foreach(QTextCursor cursor, _multilineCursors) {
		QTextCursor selectedCursor = cursor;
		selectedCursor.select(QTextCursor::LineUnderCursor);
		
		int endLinePos = selectedCursor.selectionEnd();

		selectedCursor.select(QTextCursor::BlockUnderCursor);
		bool isLastlineOfBlock = (endLinePos == selectedCursor.selectionEnd());
		
		if (isLastlineOfBlock && cursor.position() == endLinePos)
			return false;
			
		if (!isLastlineOfBlock && cursor.position() == (endLinePos-1))
			return false;
//		
//		
//		int lineLength = selectedCursor.selectionEnd() - selectedCursor.selectionStart();
//		if (maxLength == -1 || lineLength < maxLength)
//			maxLength = lineLength;
//
//		int pos = selectedCursor.selectionStart() + maxLength;
//		int lastLinePos = selectedCursor.selectionEnd();
//		
//		selectedCursor.select(QTextCursor::BlockUnderCursor);
//		bool isLastlineOfBlock = (lastLinePos == selectedCursor.selectionEnd());
//		
//		if (cursor.position() >= pos - (isLastlineOfBlock ? 0 : 1))
//			return false;
	}
	
	return true;
}

void MultilineState::moveLeft(QTextCursor::MoveMode op, int n) {
	QTextCursor toCheck = _multilineCursors.back();
	toCheck.setPosition(toCheck.position() - n, QTextCursor::KeepAnchor);

	QTextCursor endPositionCursor(editor()->document());

	bool hasSelection = (op == QTextCursor::KeepAnchor);
	
	if (toCheck.position() == endPositionCursor.position()) {
		if (hasSelection)
			createLinesFromCursor(toCheck);
		else {
			_multilineCursors.clear();
			_multilineCursors.append(endPositionCursor);
			_position = None;
		}
	}
	else {
		if (couldMoveLeft(hasSelection)) {
			int column = 0;
			for (int i = 0; i < _multilineCursors.size(); ++i) {
				QTextBlock block = _multilineCursors[i].block();
				int cursorPosition = _multilineCursors[i].position();
				_multilineCursors[i].setPosition(cursorPosition - n, op);
				if (i == 0)
					column = editor()->getColumn(block, cursorPosition - block.position());
				else {
					_multilineCursors[i].setPosition(cursorPosition - n, op);
					int newColumn = editor()->getColumn(block, _multilineCursors[i].position() - block.position());
					if (newColumn > column) {
						int newPos = editor()->getPosition(block, column) + block.position();
						n += _multilineCursors[i].position() - newPos;
						_multilineCursors[i].setPosition(newPos, op);
						newColumn = editor()->getColumn(block, newPos - block.position());
					}
					else if (newColumn < column) {
						normalizeCursors(_multilineCursors[i]);
						column = newColumn;
					}
				}
			}
		}
	}
}

void MultilineState::moveRight(QTextCursor::MoveMode op, int n) {
	QTextCursor toCheck = _multilineCursors.front();
	toCheck.setPosition(toCheck.position() + n, QTextCursor::KeepAnchor);
	
	QTextCursor endPositionCursor(editor()->document());
	endPositionCursor.movePosition(QTextCursor::End);
	
	bool hasSelection = (op == QTextCursor::KeepAnchor);
	
	if (toCheck.position() == endPositionCursor.position()) {
		if (hasSelection)
			createLinesFromCursor(toCheck);
		else {
			_multilineCursors.clear();
			_multilineCursors.append(endPositionCursor);
			_position = None;
		}
	}
	else {
		for (int i = 0 ; i < n; ++i) {
			if (couldMoveRight(hasSelection)) {
				int column = 0;
				for (int i = 0; i < _multilineCursors.size(); ++i) {
	
					QTextBlock block = _multilineCursors[i].block();
					int cursorPosition = _multilineCursors[i].position();
					_multilineCursors[i].setPosition(cursorPosition + 1, op);
					
					if (i == 0)
						column = editor()->getColumn(block, cursorPosition - block.position());
					else {
						int newColumn = editor()->getColumn(block, _multilineCursors[i].position() - block.position());
						if (newColumn < column) {
							int newPos = editor()->getPosition(block, column) + block.position();
							n += _multilineCursors[i].position() - newPos;
							_multilineCursors[i].setPosition(newPos, op);
							newColumn = editor()->getColumn(block, newPos - block.position());
						}
						else if (newColumn > column) {
							normalizeCursors(_multilineCursors[i]);
							column = newColumn;
						}
					}
				}
			}
			else
				break;
		}
	}
}

void MultilineState::selectDown() {
	if (_position == Top) { // Remove
		if (_multilineCursors.size() > 1)
			_multilineCursors.removeFirst();

		if (_multilineCursors.size() == 1)
			_position = None;
	}
	else {
		QTextBlock block = _multilineCursors.back().block();
		int firstColumn = editor()->getColumn(block, _multilineCursors.back().anchor() - block.position());
		int lastColumn = editor()->getColumn(block, _multilineCursors.back().position() - block.position());
		block = block.next();
		if (block.isValid() && block != _multilineCursors.back().block()) {
			_multilineCursors.append(addCursor(block, firstColumn, lastColumn));
			_position = Bottom;
		}
	}
}

void MultilineState::selectUp() {
	if (_position == Bottom) { // Remove
		if (_multilineCursors.size() > 1)
			_multilineCursors.removeLast();

		if (_multilineCursors.size() == 1)
			_position = None;
	}
	else {
		QTextBlock block = _multilineCursors.front().block();
		int firstColumn = editor()->getColumn(block, _multilineCursors.front().anchor() - block.position());
		int lastColumn = editor()->getColumn(block, _multilineCursors.front().position() - block.position());
		block = block.previous();
		if (block.isValid() && block != _multilineCursors.front().block()) {
			_multilineCursors.prepend(addCursor(block, firstColumn, lastColumn));
			_position = Top;
		}
	}
}

void MultilineState::insertText(QString text) {
	foreach(QTextCursor cursor, _multilineCursors) {
		if (editor()->overwriteMode()) {
			QTextBlock blk = cursor.block();
			int pos = cursor.position();
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, text.length());
			if (blk != cursor.block()) {
				cursor.setPosition(pos);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			}
			cursor.removeSelectedText();
		}
		cursor.insertText(text);
	}
	updateEditorCursor();
}

void MultilineState::removeChar(QTextCursor::MoveOperation op, int n) {
	foreach(QTextCursor cursor, _multilineCursors) {
		cursor.clearSelection();
		cursor.movePosition(op, QTextCursor::KeepAnchor, n);
		cursor.removeSelectedText();
	}
	updateEditorCursor();
}

void MultilineState::updateEditorCursor() {
	QTextCursor textCursor;
	if (_position == Top)
		textCursor = _multilineCursors.front();
	else
		textCursor = _multilineCursors.back();
		
	textCursor.clearSelection();
	editor()->setTextCursor(textCursor);
}

void MultilineState::createLinesFromCursor(QTextCursor cursor) {
	if (cursor.isNull())
		cursor = editor()->textCursor();
		
	_multilineCursors.clear();

	QTextBlock firstLine = editor()->document()->findBlock(cursor.anchor());
	QTextBlock lastLine  = cursor.block();
	int firstColumn      = editor()->getColumn(firstLine, cursor.anchor() - firstLine.position());
	int lastColumn       = editor()->getColumn(lastLine, cursor.position() - lastLine.position());

	if (cursor.anchor() < cursor.position()) {
		for (QTextBlock line = firstLine; line.isValid() && line != lastLine.next(); line = line.next()) {
			QTextCursor toAdd = addCursor(line, firstColumn, lastColumn);
			if (!_multilineCursors.contains(toAdd))
				_multilineCursors.append(toAdd);
		}

		_position = (_multilineCursors.size() > 1 ? Bottom : None);
	}
	else {
		for (QTextBlock line = firstLine; line.isValid() && line != lastLine.previous(); line = line.previous()) {
			QTextCursor toAdd = addCursor(line, firstColumn, lastColumn);
			if (!_multilineCursors.contains(toAdd))
				_multilineCursors.prepend(toAdd);
		}
		
		_position = (_multilineCursors.size() > 1 ? Top : None);
	}
	updateEditorCursor();
}

QTextCursor MultilineState::addCursor(QTextBlock line, int& firstColumn, int& lastColumn) {
	QTextCursor cursor(line);
	QString lineText = line.text();
	
	int firstPosition = editor()->getPosition(line, firstColumn);
	int lastPosition = editor()->getPosition(line, lastColumn);

	if (firstColumn < lastColumn) { // Left to Right
		if (lastPosition > 0 && lastPosition < lineText.length() && lineText[lastPosition - 1] == '\t' && editor()->tabSpaces(lastColumn - 1) != editor()->tabstop())
			--lastPosition;
	}
	else if (firstColumn > lastColumn) { // Right to Left
		if (firstPosition > 0 && firstPosition < lineText.length() && lineText[firstPosition - 1] == '\t' && editor()->tabSpaces(firstColumn - 1) != editor()->tabstop())
			--firstPosition;
	}
	
	cursor.setPosition(line.position() + firstPosition);
	if (firstPosition != lastPosition)
		cursor.setPosition(line.position() + lastPosition, QTextCursor::KeepAnchor);

	int newFirstColumn = editor()->getColumn(line, firstPosition);
	int newLastColumn = editor()->getColumn(line, lastPosition);

	if (newFirstColumn != firstColumn || newLastColumn != lastColumn) {
		firstColumn = newFirstColumn;
		lastColumn = newLastColumn;
		normalizeCursors(cursor);
	}

	return cursor;
}

void MultilineState::normalizeCursors(QTextCursor cursor) {
	int firstPosition = cursor.anchor() - cursor.block().position();
	int lastPosition  = cursor.position() - cursor.block().position();
	int firstColumn   = editor()->getColumn(cursor.block(), firstPosition);
	int lastColumn    = editor()->getColumn(cursor.block(), lastPosition);

	for (int i = 0; i < _multilineCursors.size() && _multilineCursors[i] != cursor; i++) {
		QTextBlock line = _multilineCursors[i].block();
		firstPosition = editor()->getPosition(line, firstColumn);
		lastPosition = editor()->getPosition(line, lastColumn);
		
		_multilineCursors[i].setPosition(line.position() + firstPosition);
		if (firstPosition < lastPosition)
			_multilineCursors[i].movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, lastPosition - firstPosition);
		else
			_multilineCursors[i].movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, firstPosition - lastPosition);
	}
}

void MultilineState::selectFromMultiLine() {
	QTextCursor cursor(editor()->document());

	if (_position == Top) {
		cursor.setPosition(_multilineCursors.back().anchor());
		cursor.setPosition(_multilineCursors.front().position(), QTextCursor::KeepAnchor);
	}
	else {
		cursor.setPosition(_multilineCursors.front().anchor());
		cursor.setPosition(_multilineCursors.back().position(), QTextCursor::KeepAnchor);
	}
	
	editor()->setTextCursor(cursor);
}
