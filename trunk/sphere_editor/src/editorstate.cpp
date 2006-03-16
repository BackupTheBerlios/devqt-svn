#include "editorstate.h"
#include "editor.h"
#include "editorwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QFontMetrics>
#include <QPoint>
#include <QRegion>

EditorState::EditorState(QObject *parent) : QObject(parent) {
}

EditorState::~EditorState() {
}

void EditorState::changeState(int s) {
	EditorState* newState = editor()->state(s);
	editor()->changeState(newState);
}

bool EditorState::canInsertFromMimeData(const QMimeData* data) {
	return editor()->canInsertFromMimeData(data);
}

void EditorState::insertFromMimeData(const QMimeData* data) {
	editor()->insertFromMimeData(data);
}

void EditorState::paintEvent(QPaintEvent *event) {
	editor()->_outsideCall = true;
	editor()->paintEvent(event);
}

void EditorState::keyPressEvent(QKeyEvent *event) { 
	editor()->_outsideCall = true;
	editor()->keyPressEvent(event);
}

void EditorState::dragMoveEvent(QDragMoveEvent *event) { 
	editor()->_outsideCall = true;
	editor()->dragMoveEvent(event);
}

void EditorState::dropEvent(QDropEvent *event) {
	editor()->_outsideCall = true;
	editor()->dropEvent(event);
}

void EditorState::mousePressEvent(QMouseEvent *event) { 
	editor()->_outsideCall = true;
	editor()->mousePressEvent(event);
}

void EditorState::mouseReleaseEvent(QMouseEvent *event) { 
	editor()->_outsideCall = true;
	editor()->mouseReleaseEvent(event);
}

void EditorState::mouseDoubleClickEvent(QMouseEvent *event) { 
	editor()->_outsideCall = true;
	editor()->mouseDoubleClickEvent(event);
}

void EditorState::mouseMoveEvent(QMouseEvent *event) {
	editor()->_outsideCall = true;
	editor()->mouseMoveEvent(event);
}

QMimeData* EditorState::createMimeDataFromSelection() {
	editor()->_outsideCall = true;
	return editor()->createMimeDataFromSelection();
}

QRect EditorState::selectionRect() {
	QTextCursor cursor = editor()->textCursor();
	
	if (cursor.hasSelection()) {
		QTextCursor startCursor(editor()->document());
		QTextCursor endCursor(editor()->document());
		
		startCursor.setPosition(cursor.selectionStart());
		endCursor.setPosition(cursor.selectionEnd());
		
		QRect rect = editor()->cursorRect(startCursor).unite(editor()->cursorRect(endCursor));
		
		if (rect.height() > editor()->fontMetrics().height()) {
			rect.setX(0);
			rect.setWidth(editor()->width());
		}
		
		return rect;
	}
	
	return QRect();
}

bool EditorState::hasSelection() {
	return editor()->textCursor().hasSelection();
}

QRect EditorState::getHighlightRect() {
	return editor()->getHighlightRect();
}

EditorWidget* EditorState::parentEditor() {
	return qobject_cast<EditorWidget*>(editor()->parent());
}

void EditorState::paintSelectedLines(QTextCursor cursor, QPainter& p, int xOffset, int yOffset, bool *singleLineSelection) {
	QRect toPaint;
	QRect firstRect;
	QRect lastRect;
	QTextCursor first(editor()->document());
	QTextCursor last(editor()->document());
	int  start      = cursor.selectionStart();
	int  end        = cursor.selectionEnd();
	int  lineHeight = editor()->fontMetrics().height();
	bool notSingleSelection;

	first.setPosition(start);
	firstRect = editor()->cursorRect(first);
	last.setPosition(end);
	lastRect = editor()->cursorRect(last);
	
	notSingleSelection = (firstRect.unite(lastRect).height() > lineHeight);
	
	if (notSingleSelection) {
		toPaint = firstRect;
		toPaint.setHeight(lineHeight);
		toPaint.setWidth(editor()->viewport()->width() - toPaint.x());
		toPaint.translate(xOffset, yOffset);
		p.fillRect(toPaint, editor()->palette().brush(QPalette::Highlight));
	
		for (int y = firstRect.y() + lineHeight; y < lastRect.y(); y += lineHeight) {
			toPaint = QRect(0, y, editor()->viewport()->width(), lineHeight);
			toPaint.translate(xOffset, yOffset);
			p.fillRect(toPaint, editor()->palette().brush(QPalette::Highlight));
		}
	
		toPaint = lastRect;
		toPaint.setHeight(lineHeight);
		toPaint.setX(0);
		toPaint.setWidth(lastRect.x());
		toPaint.translate(xOffset, yOffset);
		p.fillRect(toPaint, editor()->palette().brush(QPalette::Highlight));
	}
	
	if (singleLineSelection)
		*singleLineSelection = !notSingleSelection;
}

Editor* EditorState::editor() {
	return qobject_cast<Editor*>(parent());
}

void EditorState::copy() {
    if (!hasSelection())
		return;
		
    QMimeData *data = createMimeDataFromSelection();
    QApplication::clipboard()->setMimeData(data);
}

void EditorState::deleteText() {
	if (editor()->textCursor().hasSelection())
		editor()->textCursor().removeSelectedText();
}

void EditorState::cut() {
	copy();
	deleteText();
}

void EditorState::paste() {
	editor()->insertFromMimeData(QApplication::clipboard()->mimeData(QClipboard::Clipboard));
}

void EditorState::selectAll() {
	editor()->_outsideCall = true;
	editor()->selectAll();
}

void EditorState::drawCursor(QPainter& p, QTextCursor cursor, bool drawUnderline) {
	QFontMetrics fm = editor()->fontMetrics();
	QRect curRect = editor()->cursorRect(cursor);
	curRect.setX(curRect.x() + (curRect.width() / 2));
	curRect.setWidth(fm.width(' '));

	if (editor()->isCursorOn()) {
		if (editor()->overwriteMode()) {
			p.fillRect(curRect, Qt::black);

			cursor.clearSelection();
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			QString sel = cursor.selectedText();
			if (sel.length() > 0 && sel[0].isPrint() && !sel[0].isSpace()) {
				QPen pen1 = p.pen();
				p.setPen(Qt::white);
				p.drawText(curRect.x(), (curRect.y() + fm.height()) - fm.descent() - 1, sel);
				p.setPen(pen1);
			}
		}
		else
			p.drawRect(QRect(curRect.x(), curRect.y(), 1, curRect.height()));
	}
	else {
		if (editor()->overwriteMode() && drawUnderline)
			p.drawRect(QRect(curRect.x(), curRect.height()+fm.descent(), curRect.width(), 1));
	}
}

void EditorState::setCursorOn() {
	editor()->_cursorOn = true;
}
