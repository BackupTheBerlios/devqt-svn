#include "normalstate.h"
#include "editor.h"

#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QScrollBar>

#include <QPaintEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

NormalState::NormalState(Editor* context) : EditorState(context) {
	_dndFeedbackCursor = QTextCursor();
}

NormalState::~NormalState() {
}

void NormalState::initialize() {
	_dndFeedbackCursor = QTextCursor();
}

void NormalState::paintEvent(QPaintEvent *e) {
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
	
	bool singleLineSelection = true;
	if (cursor.hasSelection()) {
		QAbstractTextDocumentLayout::Selection selection;
		selection.cursor = cursor;
		selection.format.setBackground(editor()->palette().brush(QPalette::Highlight));
		selection.format.setForeground(editor()->palette().brush(QPalette::HighlightedText));
		ctx.selections.append(selection);
		paintSelectedLines(editor()->textCursor(), p, xOffset, yOffset, &singleLineSelection);
	}

	if (editor()->highlightCurrentLine() && singleLineSelection) {
		QRect line = getHighlightRect();
		line.translate(xOffset, yOffset);
		p.fillRect(line, editor()->currentLineBackground());
	}
	
	ctx.clip = r;
	editor()->document()->documentLayout()->draw(&p, ctx);

	if (editor()->isEnabled())
		drawCursor(p, cursor);
}

void NormalState::dragMoveEvent(QDragMoveEvent *e) {
	if (editor()->isReadOnly() || !canInsertFromMimeData(e->mimeData())) {
		e->ignore();
		return;
	}

	int cursorPos = editor()->document()->documentLayout()->hitTest(QPoint(e->pos().x() + editor()->horizontalScrollBar()->value(), e->pos().y() + editor()->verticalScrollBar()->value()), Qt::FuzzyHit);
	if (cursorPos != -1) {
		_dndFeedbackCursor = editor()->textCursor();
		_dndFeedbackCursor.setPosition(cursorPos);
	}

	editor()->updateView(true);
	e->acceptProposedAction();
}

void NormalState::setPersistentSelection(bool yes) {
	if (yes)
		changeState(Editor::Persistent);
}

bool NormalState::isPersistentSelection() {
	return false;
}

void NormalState::setMultilineEdit(bool yes) {
	if (yes)
		changeState(Editor::Multiline);
}

bool NormalState::isMultilineEdit() {
	return false;
}
