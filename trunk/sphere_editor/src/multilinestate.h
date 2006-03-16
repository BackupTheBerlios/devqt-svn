#ifndef MULTILINESTATE_H_
#define MULTILINESTATE_H_

#include "editorstate.h"

#include <QTextCursor>
#include <QBasicTimer>
#include <QList>
#include <QRect>

class MultilineState : public EditorState {
	Q_OBJECT
	
	public:
		MultilineState(Editor* context);
		virtual ~MultilineState();
		
		void initialize();
		void destroy();
		
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseDoubleClickEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		QMimeData* createMimeDataFromSelection();
		QRect selectionRect();
		bool hasSelection();
		void deleteText();
		void deleteText(QTextCursor* masterCursor);
		void paste();
		virtual void paste(const QMimeData *data);
		void selectAll();

		void setPersistentSelection(bool yes);
		bool isPersistentSelection();
		void setMultilineEdit(bool yes);
		bool isMultilineEdit();

	protected:
		enum Positions { None, Top, Bottom, Left, Right, Up, Down, Horizontal, Vertical };
		
		void keyPressEvent(QKeyEvent *e);
		void timerEvent(QTimerEvent *e);

		bool couldMoveLeft(bool selection = false);
		bool couldMoveRight(bool selection = false);
		void moveLeft(QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, int n = 1);
		void moveRight(QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, int n = 1);
		void dragMoveEvent(QDragMoveEvent *e);
		void dropEvent(QDropEvent *e);
		void selectDown();
		void selectUp();
		virtual void insertText(QString text);
		virtual void removeChar(QTextCursor::MoveOperation op, int n = 1);
		virtual void updateEditorCursor();
		void createLinesFromCursor(QTextCursor cursor = QTextCursor());
		QTextCursor addCursor(QTextBlock line, int& firstColumn, int& lastColumn);
		void normalizeCursors(QTextCursor cursor);
		void selectFromMultiLine();
		
	protected slots:
		void updateCursor();

	protected:
		QList<QTextCursor> _multilineCursors;
		QTextCursor _dndFeedbackCursor;
		QTextCursor _clickedPosition;
		QPoint _oldPosition;
		
		QBasicTimer _dragStartTimer;
		
		bool _mousePressed;
		bool _mightStartDrag;
		bool _doNothing;
		
		Positions _position;
		Positions _movement;
};

#endif /*MULTILINESTATE_H_*/
