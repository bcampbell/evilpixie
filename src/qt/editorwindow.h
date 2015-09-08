#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "../editor.h"
#include "../tool.h"   // for NUM_TOOLS
#include "../app.h" // for NUM_STD_BRUSHES
#include "../projectlistener.h"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>
#include <QIcon>
#include <QColor>

class EditViewWidget;
class PaletteEditor;
class PaletteWidget;
class RGBPickerWidget;
class HelpWindow;
class AboutBox;
class QLayout;
class QLabel;
class QMenuBar;
class QAction;
class QTabWidget;


struct EditorActions {
    QAction* Undo, Redo, GridOnOff, UseBrushPalette,
        ZapFrame, PrevFrame, NextFrame,
        DrawmodeNormal, DrawmodeColour, DrawmodeReplace;
};


class CurrentColourWidget : public QWidget
{
    Q_OBJECT
public:
    CurrentColourWidget(QWidget* parent=0 ) :
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
    virtual void OnPenChanged();
    virtual void OnUndoRedoChanged() { update_menu_states(); }

    // projectlistener stuff
    virtual void OnPaletteChanged( int n, Colour const& c );
    virtual void OnPaletteReplaced();
    virtual void OnModifiedFlagChanged( bool modified );
    virtual void OnFramesAdded(int /*first*/, int /*last*/) { RethinkWindowTitle(); }
    virtual void OnFramesRemoved(int /*first*/, int /*last*/) { RethinkWindowTitle(); }

    void closeEvent(QCloseEvent *event);
public slots:
    void toolclicked( QAbstractButton* b );
    void brushclicked( QAbstractButton* b );
    void fgColourPicked( int c );
    void bgColourPicked( int c );
    void fgColourPickedRGB( Colour c );
    void bgColourPickedRGB( Colour c );
    void togglepaletteeditor();
    void useeyedroppertool();
    void nextColour();
    void prevColour();
    void showHelp();
    void showAbout();

    void update_menu_states();
    void do_undo();
    void do_redo();
    void do_gridonoff( bool checked );
    void do_resize();
    void do_new();
    void do_open();
    void do_save();
    void do_saveas();
    void do_loadpalette();
    void do_usebrushpalette();
    void do_xflipbrush();
    void do_yflipbrush();
    void do_drawmodeChanged(QAction* act);

    void do_tospritesheet();
    void do_fromspritesheet();

    void do_addframe();
    void do_zapframe();
    void do_prevframe();
    void do_nextframe();

private:
    EditViewWidget* m_ViewWidget;
    PaletteWidget* m_PaletteWidget;
    RGBPickerWidget* m_RGBPicker;
    CurrentColourWidget* m_CurrentColourWidget;
    QTabWidget* m_ColourTab;

    QButtonGroup* m_BrushButtons;
    QButtonGroup* m_ToolButtons;

    QLayout* CreateBrushButtons();
    QLayout* CreateToolButtons();
    QMenuBar* CreateMenuBar();
    void CreateActions();


    QAbstractButton* FindButton( QButtonGroup* grp, const char* propname, QVariant const& val );

    PaletteEditor* m_PaletteEditor;
    AboutBox* m_AboutBox;
    HelpWindow* m_HelpWindow;

    // ptrs to any actions we need to be able to enable/disable
    QAction* m_ActionUndo;
    QAction* m_ActionRedo;
    QAction* m_ActionGridOnOff;
    QAction* m_ActionUseBrushPalette;
    QAction* m_ActionZapFrame;
    QAction* m_ActionPrevFrame;
    QAction* m_ActionNextFrame;

    QAction* m_ActionToSpritesheet;
    QAction* m_ActionFromSpritesheet;

    QAction* m_ActionDrawmodeNormal;
    QAction* m_ActionDrawmodeColour;
    QAction* m_ActionDrawmodeReplace;
 
    // status bar items
    QLabel* m_StatusViewInfo;

    QCursor* m_MouseCursors[MOUSESTYLE_NUM];

    void RethinkWindowTitle();
    bool CheckZappingOK();
};



#endif // EDITORWINDOW_H

