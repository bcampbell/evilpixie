#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "../editor.h"
#include "../tool.h"   // for NUM_TOOLS
#include "../app.h" // for NUM_STD_BRUSHES
#include "../projectlistener.h"

#include <QWidget>
#include <QIcon>
#include <QColor>
#include <QToolButton>

class EditViewWidget;
class PaletteEditor;
class PaletteWidget;
class QLayout;
class QLabel;
class QMenuBar;
class QAction;


class CurrentColourWidget : public QWidget
{
    Q_OBJECT;
public:
    CurrentColourWidget( QWidget* parent=0 ) :
        QWidget(parent),
        m_FG(QColor(0,0,0)),
        m_BG(QColor(0,0,0) )
        {}

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    void setFGColour( QColor const& c ) { m_FG=c; update(); }
    void setBGColour( QColor const& c ) { m_BG=c; update(); }
signals:
    void left_clicked();
    void right_clicked();

protected:
     void paintEvent(QPaintEvent *event);
     void mousePressEvent(QMouseEvent *event);
private:
    QColor m_FG;
    QColor m_BG;
};


class EditorWindow : public QWidget, public Editor
{
    Q_OBJECT;
public:
    EditorWindow(Project* proj, QWidget *parent = 0);
    virtual ~EditorWindow();

    // Editor implementation
    virtual void GUIShowError( const char* msg );
    virtual void OnToolChanged();
    virtual void OnBrushChanged();
    virtual void UpdateMouseInfo( Point const& mousepos );
    virtual void SetMouseStyle( MouseStyle s );

    // projectlistener stuff
    virtual void OnPaletteChanged( int n, RGBx const& c );
    virtual void OnPaletteReplaced();
    virtual void OnModifiedFlagChanged( bool modified );
    virtual void OnPenChange();
    virtual void OnUndoRedoChanged() { update_menu_states(); }

    void closeEvent(QCloseEvent *event);
public slots:
    void toolclicked( QAbstractButton* b );
    void brushclicked( QAbstractButton* b );
    void fgColourPicked( int c );
    void bgColourPicked( int c );
    void togglepaletteeditor();
    void useeyedroppertool();
    void nextColour();
    void prevColour();

    void update_menu_states();
    void do_undo( bool checked );
    void do_redo( bool checked );
    void do_gridonoff( bool checked );
    void do_togglesavebgastransparent( bool checked );
    void do_new( bool checked );
    void do_open( bool checked );
    void do_save( bool checked );
    void do_saveas( bool checked );
    void do_loadpalette( bool checked );
    void do_usebrushpalette( bool checked );

private:
    EditViewWidget* m_ViewWidget;
    PaletteWidget* m_PaletteWidget;
    CurrentColourWidget* m_CurrentColourWidget;

    QButtonGroup* m_BrushButtons;
    QButtonGroup* m_ToolButtons;

    QLayout* CreateBrushButtons();
    QLayout* CreateToolButtons();
    QMenuBar* CreateMenuBar();

    QAbstractButton* FindButton( QButtonGroup* grp, const char* propname, QVariant const& val );

    PaletteEditor* m_PaletteEditor;

    // ptrs to any actions we need to be able to enable/disable
    QAction* m_ActionUndo;
    QAction* m_ActionRedo;
    QAction* m_ActionGridOnOff;
    QAction* m_ActionSaveBGAsTransparent;
    QAction* m_ActionUseBrushPalette;

    
    // status bar items
    QLabel* m_StatusViewInfo;

    QCursor* m_MouseCursors[MOUSESTYLE_NUM];

    void RethinkWindowTitle();
    bool CheckZappingOK();
};



#endif // EDITORWINDOW_H

