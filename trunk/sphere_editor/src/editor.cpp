#include "editor.h"
#include "normalstate.h"
#include "persistentstate.h"
#include "multilinestate.h"
#include "multilinepersistentstate.h"

#include <QApplication>
#include <QCursor>
#include <QStyle>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>

#include <QtDebug>

Editor::Editor(QWidget *parent) : QTextEdit(parent) {
	_outsideCall = false;
	_cursorOn = true;
	_highlightCurrentLine = true;
	_highlightCurrentBlock = false;
	_replaceTabWithSpaces = false;

	_currentLineBackground  = QColor(232, 242, 254);
	_currentBlockBackground = QColor(245, 245, 245);

	_allStates[Normal] = new NormalState(this);
	_allStates[Multiline] = new MultilineState(this);
	_allStates[Persistent] = new PersistentState(this);
	_allStates[MultilinePersistent] = new MultilinePersistentState(this);
	
	_state = _allStates[Normal];

	setTabstop(4);
	
	_cursorBlinkTimer.start(QApplication::cursorFlashTime() / 2, this);
	
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateView()) );

	connect(_allStates[Normal], SIGNAL(copyAvailable(bool)), this, SLOT(stateCopyAvailable(bool)) );
	connect(_allStates[Multiline], SIGNAL(copyAvailable(bool)), this, SLOT(stateCopyAvailable(bool)) );
	connect(_allStates[Persistent], SIGNAL(copyAvailable(bool)), this, SLOT(stateCopyAvailable(bool)) );
	connect(_allStates[MultilinePersistent], SIGNAL(copyAvailable(bool)), this, SLOT(stateCopyAvailable(bool)) );
	createActions();
	updateView();
}

Editor::~Editor() {
}

void Editor::changeState(EditorState* newState) {
	_state->destroy();
	_state = newState;
	_state->initialize();
}

EditorState* Editor::state(int s) {
	return _allStates[s];
}

void Editor::setPersistentSelection(bool yes) {
	_state->setPersistentSelection(yes);
}

bool Editor::isPersistentSelection() {
	return _state->isPersistentSelection();
}

void Editor::setMultilineEdit(bool yes) {
	_state->setMultilineEdit(yes);
}

bool Editor::isMultilineEdit() {
	return _state->isMultilineEdit();
}

bool Editor::isCursorOn() {
	return _cursorOn;
}

void Editor::ensureCursorVisible() {
	_cursorOn = true;
	QTextEdit::ensureCursorVisible();
}

bool Editor::highlightCurrentLine() {
	return _highlightCurrentLine;
}

QColor Editor::currentLineBackground() {
	return _currentLineBackground;
}

void Editor::setCurrentLineBackground(const QColor& c) {
	_currentLineBackground = c;
}

bool Editor::highlightCurrentBlock() {
	return _highlightCurrentBlock;
}

QColor Editor::currentBlockBackground() {
	return _currentBlockBackground;
}
	
void Editor::setCurrentBlockBackground(const QColor& c) {
	_currentBlockBackground = c;
}

void Editor::setHighlightCurrentLine(bool value)  {
	_highlightCurrentLine = value;
}

void Editor::setHighlightCurrentBlock(bool value) {
	_highlightCurrentBlock = value;
}

void Editor::paintEvent(QPaintEvent *event) {
	if (_outsideCall) {
		QTextEdit::paintEvent(event);
		_outsideCall = false;
	}
	else
		_state->paintEvent(event);
}

void Editor::keyPressEvent(QKeyEvent *event) {
	if (_outsideCall) {
		if (event->modifiers() == Qt::ControlModifier) {
	        switch( event->key() ) {
	        	case Qt::Key_C:
		        case Qt::Key_Insert:
		        case Qt::Key_F16: // Copy key on Sun keyboards
		            event->accept();
		            if (_state->hasSelection())
			            copy();
	        		break;
	
		        case Qt::Key_X:
		        case Qt::Key_F20:  // Cut key on Sun keyboards
		            event->accept();
		            if (_state->hasSelection())
			            cut();
		        	break;
		        	
		        case Qt::Key_V:
		            event->accept();
			        paste();
	        		break;

	        	default:
	        		QTextEdit::keyPressEvent(event);
	        }
		}
		else if (event->modifiers() == Qt::ShiftModifier) {
			switch( event->key() ) {
		        case Qt::Key_Insert:
		            event->accept();
			        paste();
	        		break;
	
		        case Qt::Key_Delete:
		            event->accept();
		            if (_state->hasSelection())
			            cut();
		        	break;
	        		
	        	default:
	        		QTextEdit::keyPressEvent(event);
	        }
		}
		else {
			if (!event->modifiers() && event->key() == Qt::Key_Insert) {
	            event->accept();
	            setOverwriteMode(!overwriteMode());
		       	_cursorOn = true;
				updateView();
			}
			else
	       		QTextEdit::keyPressEvent(event);
		}
       		
       	_cursorOn = true;
		_outsideCall = false;
	}
	else
		_state->keyPressEvent(event);
}

void Editor::dragMoveEvent(QDragMoveEvent *event) {
	if (_outsideCall) {
		QTextEdit::dragMoveEvent(event);
		_outsideCall = false;
	}
	else
		_state->dragMoveEvent(event);

	updateView();
}

void Editor::dropEvent(QDropEvent *event) {
	if (_outsideCall) {
		QTextEdit::dropEvent(event);
		_outsideCall = false;
	}
	else
		_state->dropEvent(event);

	updateView();
}

void Editor::mousePressEvent(QMouseEvent *event) {
	if (_outsideCall) {
		QTextCursor cursor = QTextEdit::cursorForPosition(event->pos());
		QRect rect = cursorRect(cursor);
		int pos = rect.x() + (int)((rect.width() - 1) * 0.6);
		
		if (event->pos().x() > pos) {
			QPoint newPoint = event->pos();
			newPoint.setX(newPoint.x() + (rect.width()-2));
			QMouseEvent event2(event->type(), newPoint, event->button(), event->buttons(), event->modifiers());
			QTextEdit::mousePressEvent(&event2);
		}
		else {
			QTextEdit::mousePressEvent(event);
		}
		
		_outsideCall = false;
	}
	else
		_state->mousePressEvent(event);
}

void Editor::mouseReleaseEvent(QMouseEvent *event) {
	if (_outsideCall) {
		QTextEdit::mouseReleaseEvent(event);
		_outsideCall = false;
	}
	else
		_state->mouseReleaseEvent(event);
}

void Editor::mouseDoubleClickEvent(QMouseEvent *event) {
	if (_outsideCall) {
		QTextEdit::mouseDoubleClickEvent(event);
		_outsideCall = false;
	}
	else
		_state->mouseDoubleClickEvent(event);
}

void Editor::mouseMoveEvent(QMouseEvent *event) {
	if (_outsideCall) {
		QTextEdit::mouseMoveEvent(event);
		_outsideCall = false;
	}
	else
		_state->mouseMoveEvent(event);

	updateView();
}

QMimeData* Editor::createMimeDataFromSelection() {
	if (_outsideCall) {
		_outsideCall = false;
		return QTextEdit::createMimeDataFromSelection();
	}

	return _state->createMimeDataFromSelection();
}

void Editor::timerEvent(QTimerEvent *event) {
	if (event->timerId() == _cursorBlinkTimer.timerId()) {
        _cursorOn = !_cursorOn;

        if (textCursor().hasSelection())
            _cursorOn &= (style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, 0, this) != 0);

		updateView();
	}
}

void Editor::updateView(bool fullUpdate) {
	QTextCursor currentCursor = textCursor();
	QRect newCursorRect(0, cursorRect().y(), viewport()->width(), fontMetrics().height());
	QRect newSelectionTotalSelection = _state->selectionRect();
	QRect toUpdate;
	
	if (fullUpdate)
		toUpdate = QRect(QPoint(0, 0), size());
	else {
		if (_currentCursorRect.isValid()) {
			toUpdate = newCursorRect.unite(_currentCursorRect);
			toUpdate.setHeight(toUpdate.height() + 1);
		}
		else
			toUpdate = newCursorRect;
			
		if (newSelectionTotalSelection.isValid())
			toUpdate |= newSelectionTotalSelection;
	}
	
	_currentCursorRect = newCursorRect;
	
	viewport()->update(toUpdate);
}

void Editor::resizeEvent(QResizeEvent *e) {
	QTextEdit::resizeEvent(e);
	updateView();
}

void Editor::scrollContentsBy(int dx, int dy) {
	QTextEdit::scrollContentsBy(dx, dy);
	updateView(true);
}

void Editor::gotoPosition(int line, int column) {
	QTextBlock block;
	int lineNumber = 1;
	for (block = document()->begin(); block.isValid() && lineNumber < line; block = block.next(), ++lineNumber);
	if (block.isValid()) {
		QTextCursor cursor(block);
		if (column > 0)
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, getPosition(block, column));
			
		setTextCursor(cursor);
	}
}

int Editor::lineNumber(QTextBlock block) {
	if (!block.isValid())
		block = textCursor().block();
		
	int lineNumber = 1;
	for (QTextBlock line = document()->begin(); line.isValid() && line != block; line = line.next(), ++lineNumber);
	if (block.isValid())
		return lineNumber;
		
	return -1;
}

int Editor::cursorColumn(QTextCursor cursor) {
	if (cursor.isNull())
		cursor = textCursor();
		
	return getColumn(cursor.block(), cursor.position() - cursor.block().position());
}

int Editor::getColumn(QTextBlock block, int pos) {
	QString line = block.text();
	int rColumn = 0;

	for (int i = 0; i < pos && i < line.length(); i++) {
		if (line[i] == '\t')
			rColumn += tabSpaces(rColumn);
		else
			rColumn++;
	}
	return rColumn + 1;
}

int Editor::getPosition(QTextBlock block, int column) {
	QString line = block.text();
	int result;
	int originalColumn = column;

	for (result = 0; column > 1 && result < line.length(); result++) {
		if (line[result] ==	'\t')
			column -= tabSpaces(originalColumn - column);
		else
			--column;
	}
	
	return result;
}

int Editor::tabSpaces(int column) {
	int usedChars = column % _tabstop;
	return (usedChars ? _tabstop - usedChars : _tabstop);
}

int Editor::tabstop() {
	return _tabstop;
}

void Editor::setReplaceTabWithSpaces(bool yes) {
	_replaceTabWithSpaces = yes;
	if (yes) {
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		int linesInText = lineNumber(document()->end());
		int lineProcessed = 1;
		// Min = 1, Max = linesInText
		for (QTextBlock line = document()->begin(); line.isValid(); line = line.next(), ++lineProcessed) {
			QTextCursor cursor(line);
			cursor.movePosition(QTextCursor::EndOfBlock);
			int lastPos = cursor.position();
			cursor.movePosition(QTextCursor::StartOfBlock);
			for (int i = 0; i < lastPos; i++) {
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
				if (cursor.selectedText() == "\t") {
					for (int j = tabstop(); j > 0; --j)
						cursor.insertText(" ");
				}
				cursor.clearSelection();
			}
			
			double percent = (100 * lineProcessed) / linesInText;
			qDebug() << percent << "% processado...";
			// signal to 1%
		}
		
		QApplication::restoreOverrideCursor();
	}
}

bool Editor::replaceTabWithSpaces() {
	return _replaceTabWithSpaces;
}

void Editor::setTabstop(int size) {
	_tabstop = size;
	QFontMetrics fm = fontMetrics();
	int width = fm.width(' ');
	int tabWidth = width * _tabstop;
	setTabStopWidth(tabWidth);
}

void Editor::setFont(const QFont &font) {
	QTextEdit::setFont(font);
	setTabstop(_tabstop);
}

QTextCursor Editor::cursorForPosition (const QPoint &p) const {
	QTextCursor cursor = QTextEdit::cursorForPosition(p);
	QRect rect = cursorRect(cursor);
	int pos = rect.x() + (int)((rect.width() - 1) * 0.6);
	
	if (p.x() > pos) {
		QPoint ajustedPos = p;
		ajustedPos.setX(p.x() + (rect.width()-2));
		cursor = QTextEdit::cursorForPosition(ajustedPos);
	}
	
	return cursor;
}

void Editor::emitSelectionChange(bool yes) {
    emit copyAvailable(yes);
    emit selectionChanged();
}

void Editor::stateCopyAvailable(bool yes) {
    emit copyAvailable(yes);
    emit selectionChanged();
}

void Editor::copy() {
	_state->copy();
}

void Editor::cut() {
	_state->cut();
}

void Editor::paste() {
	_state->paste();
}

void Editor::selectAll() {
	if (_outsideCall) {
		QTextEdit::selectAll();
		_outsideCall = false;
	}
	else
		_state->selectAll();
}

void Editor::createActions() {
	undoAct      = new QAction(QIcon(":/icons/UNDO"), tr("&Undo"), this);
	undoAct->setShortcut(tr("Ctrl+Z"));
	undoAct->setStatusTip(tr("Undo the last action made"));
	undoAct->setEnabled(false);
	connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
	connect(this, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)) );
	
	redoAct      = new QAction(QIcon(":/icons/REDO"), tr("&Redo"), this);
	redoAct->setShortcut(tr("Ctrl+Y"));
	redoAct->setStatusTip(tr("Redo the last action that was undo"));
	redoAct->setEnabled(false);
	connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));
	connect(this, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)) );

	copyAct      = new QAction(QIcon(":/icons/COPY"), tr("&Copy"), this);
	copyAct->setShortcut(tr("Ctrl+C"));
	copyAct->setStatusTip(tr("Copy selected text"));
	copyAct->setEnabled(_state->hasSelection());
	connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
	connect(this, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)) );

	cutAct       = new QAction(QIcon(":/icons/CUT"), tr("Cu&t"), this);
	cutAct->setShortcut(tr("Ctrl+X"));
	cutAct->setStatusTip(tr("Cut selected text"));
	cutAct->setEnabled(_state->hasSelection());
	connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));
	connect(this, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)) );

	pasteAct     = new QAction(QIcon(":/icons/PASTE"), tr("&Paste"), this);
	pasteAct->setShortcut(tr("Ctrl+V"));
	pasteAct->setStatusTip(tr("Paste clipboard content inside editor"));
	connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));
	
	deleteAct    = new QAction(QIcon(":/icons/DELETE"), tr("&Delete selected text"), this);
	deleteAct->setStatusTip(tr("Remove selected text"));
	deleteAct->setEnabled(_state->hasSelection());
	connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(this, SIGNAL(copyAvailable(bool)), deleteAct, SLOT(setEnabled(bool)) );

	selectAllAct = new QAction(tr("Select &All"), this);
	selectAllAct->setShortcut(tr("Ctrl+A"));
	selectAllAct->setStatusTip(tr("Select all content of editor"));
	connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAll()));
}

void Editor::contextMenuEvent(QContextMenuEvent * e) {
	QMenu *menu = createStandardContextMenu();
	menu->exec(e->globalPos());
	delete menu;
}

QMenu* Editor::createStandardContextMenu() {
	QMenu *menu = new QMenu(this);

	menu->addAction(undoAct);
	menu->addAction(redoAct);
	menu->addSeparator();
	menu->addAction(copyAct);
	menu->addAction(cutAct);
	menu->addAction(pasteAct);
	menu->addAction(deleteAct);
	menu->addSeparator();
	menu->addAction(selectAllAct);
	
	return menu;
}

void Editor::deleteSelected() {
	_state->deleteText();
}

