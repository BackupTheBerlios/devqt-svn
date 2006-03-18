#ifndef EDITOR_H_
#define EDITOR_H_

#include <QTextEdit>
#include <QColor>
#include <QTextBlock>
#include <QTextCursor>
#include <QFont>
#include <QPoint>
#include <QRect>
#include <QBasicTimer>

class EditorState;
class QPaintEvent;
class QKeyEvent;
class QDragMoveEvent;
class QDropEvent;
class QMouseEvent;
class QMimeData;
class QTimerEvent;
class QResizeEvent;
class QContextMenuEvent;
class QAction;
class QMenu;

class Editor : public QTextEdit {
	Q_OBJECT
	
	public:
		enum States {
			Normal = 0,
			Multiline = 1,
			Persistent = 2,
			MultilinePersistent = 3,
			MaxStates = 4
		};
	
	public:
		Editor(QWidget *parent = 0);
		virtual ~Editor();

		bool isPersistentSelection();
		bool isMultilineEdit();
		bool isCursorOn();

		bool highlightCurrentLine();
		QColor currentLineBackground();
		void setCurrentLineBackground(const QColor& c);
		
		bool highlightCurrentBlock();
		QColor currentBlockBackground();
		void setCurrentBlockBackground(const QColor& c);
		
		void gotoPosition(int line, int column = 0);
		int lineNumber(QTextBlock block = QTextBlock());
		int cursorColumn(QTextCursor cursor = QTextCursor());
		int getColumn(QTextBlock block, int pos);
		int getPosition(QTextBlock block, int column);
		int tabstop();
		int tabSpaces(int column);
		void setReplaceTabWithSpaces(bool yes);
		bool replaceTabWithSpaces();

		void ensureCursorVisible();
		void setFont(const QFont &font);
		QTextCursor cursorForPosition(const QPoint &p) const;
		QMenu* createStandardContextMenu();
		
	public slots:
		void copy();
		void cut();
		void deleteSelected();
		void paste();
		void selectAll();
		
		void setPersistentSelection(bool yes);
		void setMultilineEdit(bool yes);
		void setHighlightCurrentLine(bool value);
		void setHighlightCurrentBlock(bool value);
		void updateView(bool fullUpdate = false);
		void setTabstop(int size);
		
		void stateCopyAvailable(bool yes);
		
	protected:
		friend class EditorState;
		
		void changeState(EditorState* newState);
		EditorState* state(int s);

		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void dragMoveEvent(QDragMoveEvent *event);
		void dropEvent(QDropEvent* event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		QMimeData* createMimeDataFromSelection();
		
		void timerEvent(QTimerEvent *e);
		void resizeEvent(QResizeEvent *e);
		void scrollContentsBy(int dx, int dy);
		void contextMenuEvent(QContextMenuEvent *e);
		
		void emitSelectionChange(bool yes);
		QRect getHighlightRect() { return _currentCursorRect; }
		
	private:
		void createActions();
		
	private:
		QRect _currentCursorRect;
		
		QColor _currentLineBackground;
		QColor _currentBlockBackground;
		
		QBasicTimer _cursorBlinkTimer;
		
		EditorState* _allStates[MaxStates];
		EditorState* _state;
		
		QAction *undoAct;
		QAction *redoAct;
		QAction *copyAct;
		QAction *cutAct;
		QAction *pasteAct;
		QAction *deleteAct;
		QAction *selectAllAct;
		
		bool _outsideCall;
		bool _cursorOn;
		bool _highlightCurrentLine;
		bool _highlightCurrentBlock;
		bool _replaceTabWithSpaces;
		
		int _tabstop;
};

#endif /*EDITOR_H_*/
