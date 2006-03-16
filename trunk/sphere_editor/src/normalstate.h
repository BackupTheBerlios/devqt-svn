#ifndef NORMALSTATE_H_
#define NORMALSTATE_H_

#include "editorstate.h"

#include <QTextCursor>

class NormalState : public EditorState {
	Q_OBJECT
	
	public:
		NormalState(Editor* context);
		virtual ~NormalState();

		void initialize();
		
		void paintEvent(QPaintEvent *event);
		void dragMoveEvent(QDragMoveEvent *e);
		
		void setPersistentSelection(bool yes);
		bool isPersistentSelection();
		void setMultilineEdit(bool yes);
		bool isMultilineEdit();
		
	private:
		QTextCursor _dndFeedbackCursor;
};

#endif /*NORMALSTATE_H_*/
