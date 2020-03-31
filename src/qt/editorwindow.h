#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "../editor.h"
#include "../tool.h"   // for NUM_TOOLS
#include "../app.h" // for NUM_STD_BRUSHES
#include "../projectlistener.h"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>
#include <QStandardPaths>
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
class QSplitter;


struct EditorActions {
    QAction* Undo, Redo, GridOnOff, GridSettings, UseBrushPalette,
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

    void SetTime(uint64_t micros);
    uint64_t Time() const { return m_Time; }

    void SetFocus(NodePath const& focus);
    NodePath Focus() const { return m_Focus; };

    // Editor implementation
    virtual void GUIShowError( const char* msg );
    virtual void OnToolChanged();
    virtual void OnBrushChanged();
    virtual void UpdateMouseInfo( Point const& mousepos );
    virtual void SetMouseStyle( MouseStyle s );
    virtual void OnPenChanged();
    virtual void OnUndoRedoChanged() { update_menu_states(); }

    // projectlistener stuff
	virtual void OnDamaged(NodePath const& target, int frame, Box const& dmg);
    virtual void OnPaletteChanged(NodePath const& target, int frame, int index, Colour const& c);
    virtual void OnPaletteReplaced(NodePath const& target, int frame);
    virtual void OnModifiedFlagChanged(bool);
    virtual void OnFramesAdded(NodePath const& target, int first, int count);
    virtual void OnFramesRemoved(NodePath const& target, int first, int count);
    virtual void OnFramesBlatted(NodePath const& target, int first, int count);

    // Qt widget overrides
    virtual void closeEvent(QCloseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);
public slots:
    void magnifyButtonToggled(bool checked);
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
    void do_gridconfig();
    void do_resize();
    void do_changefmt();
    void do_new();
    void do_open();
    void do_save();
    void do_saveas();
    void do_loadpalette();
    void do_usebrushpalette();
    void do_xflipbrush();
    void do_yflipbrush();
    void do_scale2xbrush();
    void do_drawmodeChanged(QAction* act);

    void do_tospritesheet();
    void do_fromspritesheet();

    void do_addlayer();

    void do_addframe();
    void do_zapframe();
    void do_prevframe();
    void do_nextframe();

private:
    uint64_t m_Time;
    NodePath m_Focus;
    int m_Frame;

    QSplitter* m_ViewSplitter;      // container for main & magnified views.
    EditViewWidget* m_ViewWidget;   // main view
    EditViewWidget* m_MagView;      // magnified view (or null)
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

    QString ProjDir();

    QAbstractButton* FindButton( QButtonGroup* grp, const char* propname, QVariant const& val );

    PaletteEditor* m_PaletteEditor;
    AboutBox* m_AboutBox;
    HelpWindow* m_HelpWindow;

    // ptrs to any actions we need to be able to enable/disable
    QAction* m_ActionUndo;
    QAction* m_ActionRedo;
    QAction* m_ActionGridOnOff;
    QAction* m_ActionGridConfig;
    QAction* m_ActionUseBrushPalette;
    QAction* m_ActionScale2xBrush;
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


    // set current frame, tell things that need to know (views, widgets etc)
    void setFrame(int frame);
};



#endif // EDITORWINDOW_H

