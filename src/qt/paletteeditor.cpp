#include "paletteeditor.h"
#include "palettewidget.h"
#include "rgbwidget.h"
#include "../colours.h"
#include "../cmd.h"
#include "../palette.h"
#include "../project.h"
#include "../editor.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>


PaletteEditor::PaletteEditor( Editor& ed, QWidget* parent ) :
    QDialog( parent ),
    m_Ed(ed),
    m_Proj( ed.Proj()),
    m_Selected( 1 )
{
    m_RGBWidget = new RGBWidget();
    m_PaletteWidget = new PaletteWidget(m_Proj.PaletteConst());
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
        Colour c( m_Proj.GetColour( m_Selected ) );
        m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
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
    Colour c( m_Proj.GetColour( idx ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
}

void PaletteEditor::SetSelected(int idx)
{
    if( m_Selected==idx )
        return;
    m_Selected = idx;
    m_PaletteWidget->SetLeftSelected(idx);
    Colour c( m_Proj.GetColour( idx ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
}

void PaletteEditor::OnDamaged(int /*frame*/, Box const& )
{
    // don't care about image changes.
}

void PaletteEditor::OnPaletteChanged( int n, Colour const& c )
{
    m_PaletteWidget->SetColour(n,c);
    if(m_Selected == n)
    {
        m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
    }
}


void PaletteEditor::OnPaletteReplaced()
{
    m_PaletteWidget->SetPalette(m_Proj.PaletteConst());
    Colour c( m_Proj.GetColour( m_Selected ) );
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
}

void PaletteEditor::colourChanged()
{
    QColor qc = m_RGBWidget->colour();

    Colour c( qc.red(), qc.green(), qc.blue(), qc.alpha() );

    Colour existing( m_Proj.GetColour( m_Selected ) );

    if (c==existing) {
        return;
    }


    // if the last cmd was a modification of the same colour, we'll just amend it!
    // this lets us keep the project up-to-date as user twiddles the colour,
    // but also avoids clogging up the undo stack with insane numbers of operations.
    Cmd* cmd = m_Ed.TopCmd();
    if (cmd)
    {
        Cmd_PaletteModify* mod = cmd->ToPaletteModify();
        if (mod)
        {
            if (mod->Merge(m_Selected, c))
            {
                return;
            }
        }
    }
    
    // if we get this far we need a fresh cmd.
    cmd = new Cmd_PaletteModify(m_Proj, m_Selected, 1, &c);
    m_Ed.AddCmd(cmd);
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

    Cmd* cmd = new Cmd_PaletteModify(m_Proj, n0, n1-n0, &newpalette.Colours[n0]);
    m_Ed.AddCmd(cmd);
}

