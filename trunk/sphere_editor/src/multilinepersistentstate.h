#ifndef MULTILINEPERSISTENTSTATE_H_
#define MULTILINEPERSISTENTSTATE_H_

#include "multilinestate.h"

class MultilinePersistentState : public MultilineState {
	public:
		MultilinePersistentState(Editor* context);
		virtual ~MultilinePersistentState();

		void initialize();
		void destroy();

		void keyPressEvent(QKeyEvent *e);
		void paintEvent(QPaintEvent *event);
		void paste(const QMimeData *data);
		QRect selectionRect();

		void setPersistentSelection(bool yes);
		bool isPersistentSelection();
		void setMultilineEdit(bool yes);
		bool isMultilineEdit();

	protected:
		void updateEditorCursor();
		void insertText(QString text);
		void removeChar(QTextCursor::MoveOperation op, int n = 1);

	protected slots:
		void updateCursor();
		
	private:
		QList<QTextCursor> _multilineEditCursors;
		
};

#endif /*MULTILINEPERSISTENTSTATE_H_*/
