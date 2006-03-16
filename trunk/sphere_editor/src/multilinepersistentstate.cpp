#include "multilinepersistentstate.h"
#include "editor.h"

#include <QMimeData>
#include <QScrollBar>
#include <QPaintEvent>
#include <QAbstractTextDocumentLayout>
#include <QPalette>
#include <QPen>
#include <QApplication>
#include <QClipboard>

#include <QtDebug>

MultilinePersistentState::MultilinePersistentState(Editor* context) : MultilineState(context) {
}

MultilinePersistentState::~MultilinePersistentState() {
}

void MultilinePersistentState::initialize() {
	QTextCursor cursor = editor()->textCursor();
	if (cursor.hasSelection()) {
		createLinesFromCursor();
		_multilineEditCursors = _multilineCursors;
		for (int i = 0; i < _multilineEditCursors.size(); ++i)
			_multilineEditCursors[i].clearSelection();
	}
	else
		_multilineEditCursors.append(cursor);
		
	editor()->updateView(true);
}

void MultilinePersistentState::destroy() {
	selectFromMultiLine();
	_multilineCursors.clear();
	_multilineEditCursors.clear();
}

void MultilinePersistentState::setPersistentSelection(bool yes) {
	if (yes)
		changeState(Editor::Persistent);
}

bool MultilinePersistentState::isPersistentSelection() {
	return true;
}

void MultilinePersistentState::setMultilineEdit(bool yes) {
	if (yes)
		changeState(Editor::Multiline);
}

bool MultilinePersistentState::isMultilineEdit() {
	return true;
}

void MultilinePersistentState::paintEvent(QPaintEvent *e) {
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
		foreach(QTextCursor drawedCursor, _multilineEditCursors)
			drawCursor(p, drawedCursor, false);
	}
}

void MultilinePersistentState::paste(const QMimeData *data) {
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
	
//	if (hasSelection())
//		deleteText(&masterCursor);
	
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

void MultilinePersistentState::updateEditorCursor() {
	QTextCursor textCursor;
	if (_position == Top)
		textCursor = _multilineEditCursors.front();
	else
		textCursor = _multilineEditCursors.back();
		
	textCursor.clearSelection();
	editor()->setTextCursor(textCursor);
}

void MultilinePersistentState::keyPressEvent(QKeyEvent *e) {
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
			if (couldMoveLeft())
				removeChar(QTextCursor::PreviousCharacter);
				
			setCursorOn();
				
			break;
			
		case Qt::Key_Delete:
			if (couldMoveRight())
				removeChar(QTextCursor::NextCharacter);

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

void MultilinePersistentState::insertText(QString text) {
	foreach(QTextCursor cursor, _multilineEditCursors) {
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

void MultilinePersistentState::removeChar(QTextCursor::MoveOperation op, int n) {
	foreach(QTextCursor cursor, _multilineEditCursors) {
		cursor.clearSelection();
		cursor.movePosition(op, QTextCursor::KeepAnchor, n);
		cursor.removeSelectedText();
	}
	updateEditorCursor();
}

QRect MultilinePersistentState::selectionRect() {
	QTextCursor c1 = _multilineEditCursors.front();
	c1.setPosition(c1.selectionStart());
	QTextCursor c2 = _multilineEditCursors.back();
	c2.setPosition(c2.selectionEnd());
	QRect editRect = editor()->cursorRect(c1).unite(editor()->cursorRect(c2));
	
	if (!_multilineCursors.isEmpty())
		editRect |= MultilineState::selectionRect();
	
	return editRect;
}

void MultilinePersistentState::updateCursor() {
	QList<QTextCursor> backup = _multilineCursors;
	_multilineCursors = _multilineEditCursors;
	
	MultilineState::updateCursor();
	
	_multilineEditCursors = _multilineCursors;
	if (!hasSelection())
		_multilineCursors = backup;
	else {
		QTextCursor mergeCursor(editor()->document());
		if (!backup.isEmpty() && _position == Bottom && _multilineCursors.front().anchor() == backup.front().position()) {
			mergeCursor.setPosition(backup.front().anchor());
			mergeCursor.setPosition(_multilineCursors.back().position());
			createLinesFromCursor(mergeCursor);
		}
		else if (!backup.isEmpty() && _position == Top && _multilineCursors.back().anchor() == backup.back().position()) {
			mergeCursor.setPosition(backup.back().anchor());
			mergeCursor.setPosition(_multilineCursors.front().position());
			createLinesFromCursor(mergeCursor);
		}
		
		for (int i = 0; i < _multilineEditCursors.size(); ++i)
			_multilineEditCursors[i].clearSelection();
	}
	
	updateEditorCursor();
	if (hasSelection())
		emit copyAvailable(true);
	else
		emit copyAvailable(false);	
}
