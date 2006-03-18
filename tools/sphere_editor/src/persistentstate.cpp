#include "persistentstate.h"
#include "editor.h"

#include <QApplication>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QScrollBar>

#include <QPaintEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QMimeData>

#include <QtDebug>

PersistentState::PersistentState(Editor *context) : EditorState(context) {
	_mousePressed = false;
	_mightStartDrag = false;
	
	_persistentCursor = QTextCursor();
	_dndFeedbackCursor = QTextCursor();
}

PersistentState::~PersistentState() {
}

void PersistentState::initialize() {
	_dndFeedbackCursor = QTextCursor();
	
	QTextCursor textCursor = editor()->textCursor();
	if (textCursor.hasSelection()) {
		_persistentCursor = textCursor;
		textCursor.clearSelection();
		editor()->setTextCursor(textCursor);
	}
	else
		_persistentCursor = QTextCursor();

	connect(editor(), SIGNAL(selectionChanged()), this, SLOT(editorSelectionChange()) );
}

void PersistentState::destroy() {
	disconnect(editor(), SIGNAL(selectionChanged()), this, SLOT(editorSelectionChange()) );
	
	if (_persistentCursor.position() == editor()->textCursor().position())
		editor()->setTextCursor(_persistentCursor);
	
	_persistentCursor = QTextCursor();
	editor()->updateView(true);
}

void PersistentState::paintEvent(QPaintEvent *e) {
	QPainter p(editor()->viewport());
	
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
	
	if (editor()->highlightCurrentLine()) {
		QRect line = getHighlightRect();
		line.translate(xOffset, yOffset);
		p.fillRect(line, editor()->currentLineBackground());
	}

	if (!_persistentCursor.isNull() && _persistentCursor.hasSelection()) {
		QAbstractTextDocumentLayout::Selection selection;
		selection.cursor = _persistentCursor;
		selection.format.setBackground(editor()->palette().brush(QPalette::Highlight));
		selection.format.setForeground(editor()->palette().brush(QPalette::HighlightedText));
		ctx.selections.append(selection);
		paintSelectedLines(_persistentCursor, p, xOffset, yOffset);
	}

	ctx.clip = r;
	editor()->document()->documentLayout()->draw(&p, ctx);

	if (editor()->isEnabled())
		drawCursor(p, cursor);
}

void PersistentState::mousePressEvent(QMouseEvent *e) {
	if (e->button() & Qt::LeftButton) {
		_persistentCursorState = editor()->cursorForPosition(e->pos());
		_mousePressed = true;
		_mightStartDrag = false;
		
		if (_persistentCursor.hasSelection() && 
		    _persistentCursorState.position() >= _persistentCursor.selectionStart() &&
		    _persistentCursorState.position() <= _persistentCursor.selectionEnd()) {
                _mightStartDrag = true;
                _dragStartTimer.start(QApplication::startDragTime(), this);
                return;
		}
	}
	else if (e->button() & Qt::MidButton && _persistentCursor.hasSelection()) {
		QTextCursor cursor = editor()->cursorForPosition(e->pos());
		cursor.insertText(_persistentCursor.selectedText());
		editor()->setTextCursor(cursor);
	}
}

void PersistentState::mouseReleaseEvent(QMouseEvent *e) {
	if (!_persistentCursorState.isNull()) {
		_lastCursor = editor()->textCursor();
		editor()->setTextCursor(_persistentCursorState);
		_persistentCursorState = QTextCursor();
	}
	else  {
		if (_persistentCursor.hasSelection())
			emit copyAvailable(true);
		else
			emit copyAvailable(false);
	}

    if (_dragStartTimer.isActive())
        _dragStartTimer.stop();
}

void PersistentState::mouseDoubleClickEvent(QMouseEvent *e) {
	_persistentCursor = editor()->cursorForPosition(e->pos());
	_persistentCursor.select(QTextCursor::WordUnderCursor);
	editor()->setTextCursor(_lastCursor);
	editor()->updateView(true);
	emit copyAvailable(true);
}

void PersistentState::mouseMoveEvent(QMouseEvent *e) {
	if (!_persistentCursorState.isNull() && _persistentCursorState != _persistentCursor) {
		_persistentCursor = _persistentCursorState;
		_persistentCursorState = QTextCursor();
	}
	
	if (!_persistentCursor.isNull()) {
		QTextCursor cursor = editor()->cursorForPosition(e->pos());
		_persistentCursor.setPosition(cursor.position(), QTextCursor::KeepAnchor);
	}
	
	if (_mousePressed) {
		editor()->updateView(true);
		_mousePressed = false;
	}
}

void PersistentState::dragMoveEvent(QDragMoveEvent *e) {
	if (editor()->isReadOnly() || !canInsertFromMimeData(e->mimeData())) {
		e->ignore();
		return;
	}

	int cursorPos = editor()->document()->documentLayout()->hitTest(QPoint(e->pos().x() + editor()->horizontalScrollBar()->value(), e->pos().y() + editor()->verticalScrollBar()->value()), Qt::FuzzyHit);
	if (cursorPos != -1) {
		_dndFeedbackCursor = _persistentCursor;
		_dndFeedbackCursor.setPosition(cursorPos);
	}

	editor()->updateView(true);
	e->acceptProposedAction();
}

QMimeData* PersistentState::createMimeDataFromSelection() {
	QMimeData* data = new QMimeData();
	data->setText(_persistentCursor.selectedText());
	return data;
}

QRect PersistentState::selectionRect() {
	if (!_persistentCursor.isNull() && _persistentCursor.hasSelection()) {
		QTextCursor startCursor(editor()->document());
		QTextCursor endCursor(editor()->document());
		
		startCursor.setPosition(_persistentCursor.selectionStart());
		endCursor.setPosition(_persistentCursor.selectionEnd());
		
		QRect rect = editor()->cursorRect(startCursor).unite(editor()->cursorRect(endCursor));
		
		if (rect.height() > editor()->fontMetrics().height()) {
			rect.setX(0);
			rect.setWidth(editor()->width());
		}
		
		return rect;
	}
	
	return QRect();
}

void PersistentState::setPersistentSelection(bool yes) {
	if (!yes)
		changeState(Editor::Normal);
}

bool PersistentState::isPersistentSelection() {
	return true;
}

void PersistentState::setMultilineEdit(bool yes) {
	if (yes)
		changeState(Editor::MultilinePersistent);
}

bool PersistentState::isMultilineEdit() {
	return false;
}

void PersistentState::timerEvent(QTimerEvent *e) {
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
		
		if (action == Qt::MoveAction && drag->target() != editor())
			_persistentCursor.removeSelectedText();

	}
}

void PersistentState::editorSelectionChange() {
	QTextCursor cursor = editor()->textCursor();
	if (cursor.hasSelection()) {
		if (cursor.anchor() == _persistentCursor.position())
			_persistentCursor.setPosition(cursor.position(), QTextCursor::KeepAnchor);
		else
			_persistentCursor = cursor;
			
		cursor.clearSelection();
		editor()->setTextCursor(cursor);
		editor()->update();
		emit copyAvailable(true);
	}
}

bool PersistentState::hasSelection() {
	return _persistentCursor.hasSelection();
}

void PersistentState::deleteText() {
	if (hasSelection())
		_persistentCursor.removeSelectedText();
}
