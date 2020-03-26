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
    PaletteEditor(QWidget* parent, Editor& ed, NodePath const& focus);
    virtual ~PaletteEditor();

    // ProjectListener stuff
	virtual void OnDamaged(NodePath const& target, Box const& dmg);
    virtual void OnPaletteChanged(NodePath const& owner, int index, Colour const& c);
    virtual void OnPaletteReplaced(NodePath const& owner);
    virtual void OnModifiedFlagChanged(bool changed);
    virtual void OnFramesAdded(NodePath const& first, int count);
    virtual void OnFramesRemoved(NodePath const& first, int count);
    virtual void OnFramesBlatted(NodePath const& first, int count);

    // Set the frame whose palette we're editing
    // TODO
//    void SetFocus(NodePath const& targ);

public slots:
    void colourPicked(int idx);
    void colourDropped(int idx, Colour const& c);
    void SetSelected(int idx);
private:
    Editor& m_Ed;
    Project& m_Proj;
    NodePath m_Focus;

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

