#ifndef PERSISTENTSTATE_H_
#define PERSISTENTSTATE_H_

#include "editorstate.h"

#include <QTextCursor>
#include <QBasicTimer>
#include <QRect>

class PersistentState : public EditorState {
	Q_OBJECT
	
	public:
		PersistentState(Editor* context);
		virtual ~PersistentState();

		void initialize();
		void destroy();
		
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseDoubleClickEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void dragMoveEvent(QDragMoveEvent *e);
		QMimeData* createMimeDataFromSelection();
		QRect selectionRect();
		bool hasSelection();
		void deleteText();
		
		void setPersistentSelection(bool yes);
		bool isPersistentSelection();
		void setMultilineEdit(bool yes);
		bool isMultilineEdit();

	protected:
		void timerEvent(QTimerEvent *e);
		
	private slots:
		void editorSelectionChange();
		
	private:
		QTextCursor _persistentCursorState;
		QTextCursor _persistentCursor;
		QTextCursor _lastCursor;
		QTextCursor _dndFeedbackCursor;
		
		QBasicTimer _dragStartTimer;
		
		bool _mousePressed;
		bool _mightStartDrag;
};

#endif /*PERSISTENTSTATE_H_*/
