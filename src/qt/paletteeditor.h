#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <QtWidgets/QDialog>
#include "../projectlistener.h"

class PaletteWidget;
class RGBWidget;
class Project;
class Editor;
class QPushButton;

class PaletteEditor : public QDialog, public ProjectListener
{
    Q_OBJECT
public:
    PaletteEditor( Editor& ed, QWidget* parent );
    virtual ~PaletteEditor();

    // projectlistener stuff
	virtual void OnDamaged( int frame, Box const& viewdmg );
    virtual void OnPaletteChanged( int n, Colour const& c );
    virtual void OnPaletteReplaced();

public slots:
    void colourPicked(int idx);
    void SetSelected(int idx);
private:
    Editor& m_Ed;
    Project& m_Proj;
    int m_Selected;

    PaletteWidget* m_PaletteWidget;
    RGBWidget* m_RGBWidget;
    QPushButton *m_SpreadButton;
private slots:
    void colourChanged();
    void paletteRangeAltered();
    void spreadColours();
};



#endif // PALETTEEDITOR_H

