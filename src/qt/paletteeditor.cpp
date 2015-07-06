#include "paletteeditor.h"
#include "palettewidget.h"
#include "rgbwidget.h"
#include "../colours.h"
#include "../palette.h"
#include "../project.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>


PaletteEditor::PaletteEditor( Project& proj, QWidget* parent ) :
    QDialog( parent ),
    m_Proj( proj),
    m_Selected( 1 )
{
    m_RGBWidget = new RGBWidget();
    m_PaletteWidget = new PaletteWidget(proj.PaletteConst());
    m_PaletteWidget->EnableRangePicking( true );
    resize( QSize(500,400) );

    QHBoxLayout* h = new QHBoxLayout();
        QVBoxLayout* v = new QVBoxLayout();
        v->addWidget( m_RGBWidget );
        m_SpreadButton = new QPushButton("Spread");
        v->addWidget( m_SpreadButton );
//        v->addStretch();
    h->addLayout( v );
    h->setStretchFactor(v,0);
    h->addWidget( m_PaletteWidget );
    h->setStretchFactor(m_PaletteWidget,1);
    setLayout( h );

    {
        RGBx c( m_Proj.GetColour( m_Selected ) );
        m_RGBWidget->setColour( QColor( c.r, c.g, c.b ) );
    }
    connect( m_RGBWidget, SIGNAL( colourChanged() ), this, SLOT( colourChanged() ) );
    connect( m_PaletteWidget, SIGNAL( rangeAltered() ), this, SLOT( paletteRangeAltered() ) );
    connect( m_PaletteWidget, SIGNAL( pickedLeftButton(int) ), this, SLOT( colourPicked(int) ) );
    connect( m_SpreadButton, SIGNAL( clicked() ), this, SLOT( spreadColours() ) );

    m_SpreadButton->setEnabled( m_PaletteWidget->RangeValid() );

    m_Proj.AddListener( this );
}

PaletteEditor::~PaletteEditor()
{
    m_Proj.RemoveListener( this );
}


void PaletteEditor::colourPicked(int idx)
{
    m_Selected = idx;
    RGBx c( m_Proj.GetColour( idx ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b ) );
}

void PaletteEditor::SetSelected(int idx)
{
    if( m_Selected==idx )
        return;
    m_Selected = idx;
    m_PaletteWidget->SetLeftSelected(idx);
    RGBx c( m_Proj.GetColour( idx ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b ) );
}

void PaletteEditor::OnDamaged( Box const& )
{
    // don't care about image changes.
}

void PaletteEditor::OnPaletteChanged( int n, RGBx const& c )
{
    m_PaletteWidget->SetColour(n,c);
}


void PaletteEditor::OnPaletteReplaced()
{
    m_PaletteWidget->SetPalette(m_Proj.PaletteConst());
}

void PaletteEditor::colourChanged()
{
    QColor qc = m_RGBWidget->colour();

    m_Proj.PaletteChange_Begin();
    RGBx c( qc.red(), qc.green(), qc.blue() );
    m_Proj.PaletteChange_Alter(m_Selected, c );
    m_Proj.PaletteChange_Commit();
}

void PaletteEditor::paletteRangeAltered()
{
    m_SpreadButton->setEnabled( m_PaletteWidget->RangeValid() );
}

void PaletteEditor::spreadColours()
{
    if( !m_PaletteWidget->RangeValid() )
        return;

    Palette newpalette( m_Proj.PaletteConst() );

    int n0 = m_PaletteWidget->RangeFirst();
    int n1 = m_PaletteWidget->RangeLast();

    newpalette.LerpRange( n0, newpalette.GetColour(n0), n1, newpalette.GetColour(n1) );

    m_Proj.PaletteChange_Begin();
    m_Proj.PaletteChange_Replace( newpalette );
    m_Proj.PaletteChange_Commit();
}

