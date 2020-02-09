#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <QtWidgets/QDialog>
#include "../projectlistener.h"

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
    PaletteEditor( Editor& ed, QWidget* parent );
    virtual ~PaletteEditor();

    // projectlistener stuff
	virtual void OnDamaged( int frame, Box const& viewdmg );
    virtual void OnPaletteChanged( int n, Colour const& c );
    virtual void OnPaletteReplaced(ImgID const& id);

public slots:
    void colourPicked(int idx);
    void colourDropped(int idx, Colour const& c);
    void SetSelected(int idx);
private:
    Editor& m_Ed;
    Project& m_Proj;
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

