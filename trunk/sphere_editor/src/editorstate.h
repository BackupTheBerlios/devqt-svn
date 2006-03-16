#ifndef EDITORSTATE_H_
#define EDITORSTATE_H_

#include <QObject>
#include <QRect>
#include <QPainter>
#include <QTextCursor>

class Editor;
class EditorWidget;
class QPaintEvent;
class QKeyEvent;
class QDragMoveEvent;
class QDropEvent;
class QMouseEvent;
class QMimeData;

class EditorState : public QObject {
	Q_OBJECT
	
	public:
		EditorState(QObject *parent);
		virtual ~EditorState();
		
		virtual void initialize() { }
		virtual void destroy() { }
		
		virtual void paintEvent(QPaintEvent *event);
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void dragMoveEvent(QDragMoveEvent *e);
		virtual void dropEvent(QDropEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual QMimeData* createMimeDataFromSelection();
		virtual QRect selectionRect();
		virtual bool hasSelection();
		virtual void copy();
		virtual void deleteText();
		virtual void cut();
		virtual void paste();
		virtual void selectAll();
		
		virtual void setPersistentSelection(bool yes) = 0;
		virtual bool isPersistentSelection() = 0;
		virtual void setMultilineEdit(bool yes) = 0;
		virtual bool isMultilineEdit() = 0;
		
	signals:
		void copyAvailable(bool);
		
	protected:
		Editor* editor();
		void changeState(int s);
		bool canInsertFromMimeData(const QMimeData* data);
		void insertFromMimeData(const QMimeData* data);
		void paintSelectedLines(QTextCursor cursor, QPainter& p, int xOffset, int yOffset, bool *singleLineSelection = 0);
		void drawCursor(QPainter& p, QTextCursor cursor, bool drawUnderline = true);
		void setCursorOn();
		
		QRect getHighlightRect();
		EditorWidget* parentEditor();
};

#endif /*EDITORSTATE_H_*/
