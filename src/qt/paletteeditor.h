#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <QtWidgets/QDialog>
#include "../projectlistener.h"

class PaletteWidget;
class RGBWidget;
class Project;
class QPushButton;

class PaletteEditor : public QDialog, public ProjectListener
{
    Q_OBJECT
public:
    PaletteEditor( Project& proj, QWidget* parent );
    virtual ~PaletteEditor();

    // projectlistener stuff
	virtual void OnDamaged( Box const& viewdmg );
    virtual void OnPaletteChanged( int n, RGBx const& c );
    virtual void OnPaletteReplaced();
    virtual void OnPenChange();
private:
    Project& m_Proj;
    int m_Selected;

    PaletteWidget* m_PaletteWidget;
    RGBWidget* m_RGBWidget;
    QPushButton *m_SpreadButton;
private slots:
    void colourChanged();
    void setLeftSelected(int n);
    void setRightSelected(int n);
    void paletteRangeAltered();
    void spreadColours();
};



#endif // PALETTEEDITOR_H

