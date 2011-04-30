#include "paletteeditor.h"
#include "palettewidget.h"
#include "rgbwidget.h"
#include "../colours.h"
#include "../palette.h"
#include "../project.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


PaletteEditor::PaletteEditor( Project& proj, QWidget* parent ) :
    QDialog( parent ),
    m_Proj( proj),
    m_Selected( 1 )
{
    m_RGBWidget = new RGBWidget();
    m_PaletteWidget = new PaletteWidget();
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


    int n;
    for( n=0; n<=255; ++n )
    {
        RGBx c( m_Proj.GetColour( n ) );
        m_PaletteWidget->SetColour( n, QColor( c.r, c.g, c.b ) );
    }

    {
        RGBx c( m_Proj.GetColour( m_Selected ) );
        m_RGBWidget->setColour( QColor( c.r, c.g, c.b ) );
    }
    m_PaletteWidget->SetLeftSelected( proj.FGPen() );
    m_PaletteWidget->SetRightSelected( proj.BGPen() );

    connect( m_RGBWidget, SIGNAL( colourChanged() ), this, SLOT( colourChanged() ) );
    connect( m_PaletteWidget, SIGNAL( pickedLeftButton(int) ), this, SLOT( setLeftSelected(int) ) );
    connect( m_PaletteWidget, SIGNAL( pickedRightButton(int) ), this, SLOT( setRightSelected(int) ) );
    connect( m_PaletteWidget, SIGNAL( rangeAltered() ), this, SLOT( paletteRangeAltered() ) );
    connect( m_SpreadButton, SIGNAL( clicked() ), this, SLOT( spreadColours() ) );

    m_SpreadButton->setEnabled( m_PaletteWidget->RangeValid() );

    m_Proj.AddListener( this );
}

PaletteEditor::~PaletteEditor()
{
    m_Proj.RemoveListener( this );
}


void PaletteEditor::OnDamaged( Box const& )
{
    // don't care about image changes.
}

void PaletteEditor::OnPaletteChanged( int n, RGBx const& c )
{
    m_PaletteWidget->SetColour( n, QColor( c.r, c.g, c.b ) );
}

void PaletteEditor::OnPenChange()
{
    m_Selected = m_Proj.FGPen();
    RGBx c( m_Proj.GetColour( m_Selected ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b ) );
    m_PaletteWidget->SetLeftSelected( m_Proj.FGPen() );
    m_PaletteWidget->SetRightSelected( m_Proj.BGPen() );
}

void PaletteEditor::OnPaletteReplaced()
{
    int n;
    for( n=0; n<=255; ++n )
    {
        RGBx c = m_Proj.GetColour( n );
        m_PaletteWidget->SetColour( n, QColor( c.r, c.g, c.b ) );
    }
}

void PaletteEditor::colourChanged()
{
    QColor qc = m_RGBWidget->colour();

    m_Proj.PaletteChange_Begin();
    m_Proj.PaletteChange_Alter(m_Selected, RGBx( qc.red(), qc.green(), qc.blue() ) );
    m_Proj.PaletteChange_Commit();
}

void PaletteEditor::setLeftSelected(int n)
    { m_Proj.SetFGPen( n ); }

void PaletteEditor::setRightSelected(int n)
    { m_Proj.SetBGPen( n ); }

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

