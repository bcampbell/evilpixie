#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <QtWidgets/QDialog>
#include "../projectlistener.h"
#include "../project.h"

class PaletteWidget;
class RGBWidget;
class HSVWidget;
class Project;
class Editor;
class QPushButton;
class QLineEdit;

class PaletteEditor : public QDialog, public ProjectListener
{
    Q_OBJECT
public:
    PaletteEditor(QWidget* parent, Editor& ed, NodePath const& focus, int frame);
    virtual ~PaletteEditor();

    // ProjectListener stuff
	virtual void OnDamaged(NodePath const& target, int frame, Box const& dmg);
    virtual void OnPaletteChanged(NodePath const& target, int frame, int index, Colour const& c);
    virtual void OnPaletteReplaced(NodePath const& target, int frame);
    virtual void OnModifiedFlagChanged(bool changed);
    virtual void OnFramesAdded(NodePath const& target, int first, int count);
    virtual void OnFramesRemoved(NodePath const& target, int first, int count);
    virtual void OnFramesBlatted(NodePath const& target, int first, int count);

    // Set the frame whose palette we're editing
    // TODO
//    void SetFocus(NodePath const& targ);

    void SetSelectedColour(PenID slot, int idx);

private slots:
    void fgColourPicked(int idx);
    void bgColourPicked(int idx);
    void colourDropped(int idx, Colour const& c);
private:
    Editor& m_Ed;
    Project& m_Proj;
    NodePath m_Focus;
    int m_Frame;

    int m_Selected;

    bool m_Applying;    // Latch to prevent feedback during edits.

    PaletteWidget* m_PaletteWidget;
    RGBWidget* m_RGBWidget;
    HSVWidget* m_HSVWidget;
    QLineEdit* m_HexEntry;
    QPushButton *m_SpreadButton;
    void showColour(Colour const& c);
    void showColourInRGB(Colour const& c);
    void showColourInHSV(Colour const& c);
    void showColourInHex(Colour const& c);
    void applyEdit(Colour const &c);
private slots:
    void hexChanged();
    void rgbChanged();
    void hsvChanged();
    void paletteRangeAltered();
    void spreadColours();
};



#endif // PALETTEEDITOR_H

