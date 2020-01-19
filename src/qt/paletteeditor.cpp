#include "paletteeditor.h"
#include "palettewidget.h"
#include "hsvwidget.h"
#include "rgbwidget.h"
//#include "rgbpickerwidget.h"
#include "../colours.h"
#include "../cmd.h"
#include "../palette.h"
#include "../project.h"
#include "../editor.h"
#include "../hsv.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QLineEdit>
#include <QFontDatabase>

PaletteEditor::PaletteEditor( Editor& ed, QWidget* parent ) :
    QDialog( parent ),
    m_Ed(ed),
    m_Proj( ed.Proj()),
    m_Selected( 1 ),
    m_Applying(false)
{
    m_HexEntry = new QLineEdit();
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_HexEntry->setFont(fixedFont);
    m_RGBWidget = new RGBWidget();
    m_HSVWidget = new HSVWidget();
    m_PaletteWidget = new PaletteWidget(m_Proj.PaletteConst());
    m_PaletteWidget->EnableRangePicking( true );
    m_SpreadButton = new QPushButton("Spread");

    resize( QSize(500,400) );

    QHBoxLayout* h = new QHBoxLayout();
        QVBoxLayout* v = new QVBoxLayout();
        QTabWidget* tab = new QTabWidget(this);
        tab->setTabShape(QTabWidget::Triangular);
        tab->setTabPosition(QTabWidget::South);
        tab->setDocumentMode(true);
        tab->addTab(m_RGBWidget, "RGB");
        tab->addTab(m_HSVWidget, "HSV");

        v->addWidget(m_HexEntry);
        v->addWidget(tab);
        v->addWidget(m_SpreadButton);
    h->addLayout( v );
    h->setStretchFactor(v,0);
    h->addWidget( m_PaletteWidget );
    h->setStretchFactor(m_PaletteWidget,1);
    setLayout( h );

    {
        Colour c( m_Proj.GetColour( m_Selected ) );
        showColour(c);
    }
    connect(m_RGBWidget, SIGNAL(colourChanged()), this, SLOT(rgbChanged()));
    connect(m_HSVWidget, SIGNAL(colourChanged()), this, SLOT(hsvChanged()));
    connect(m_HexEntry, &QLineEdit::editingFinished, this, &PaletteEditor::hexChanged);
//    connect(m_HexEntry, SIGNAL(textEdited(const QString &text)), this, SLOT(hexChanged(const QString &text)));
    connect(m_PaletteWidget, SIGNAL(rangeAltered()), this, SLOT(paletteRangeAltered()));
    connect(m_PaletteWidget, SIGNAL(pickedLeftButton(int)), this, SLOT(colourPicked(int)));
    connect(m_SpreadButton, SIGNAL(clicked()), this, SLOT(spreadColours()));

    m_SpreadButton->setEnabled(m_PaletteWidget->RangeValid());

    m_Proj.AddListener(this);
}

PaletteEditor::~PaletteEditor()
{
    m_Proj.RemoveListener( this );
}


void PaletteEditor::colourPicked(int idx)
{
    m_Selected = idx;
    Colour c( m_Proj.GetColour( idx ) );
    showColour(c);
}

void PaletteEditor::SetSelected(int idx)
{
    if( m_Selected==idx )
        return;
    m_Selected = idx;
    m_PaletteWidget->SetLeftSelected(idx);
    Colour c( m_Proj.GetColour( idx ) );
    showColour(c);
}

void PaletteEditor::OnDamaged(int /*frame*/, Box const& )
{
    // don't care about image changes.
}

void PaletteEditor::OnPaletteChanged( int n, Colour const& c )
{
    m_PaletteWidget->SetColour(n,c);
    if (m_Applying) {
        return;
    }
    if(m_Selected == n) {
        showColour(c);
    }
}


void PaletteEditor::OnPaletteReplaced()
{
    m_PaletteWidget->SetPalette(m_Proj.PaletteConst());
    if (m_Applying) {
        return;
    }
    Colour c( m_Proj.GetColour( m_Selected ) );
    showColour(c);
}


void PaletteEditor::showColourInRGB(Colour const& c)
{
    m_RGBWidget->setColour( QColor( c.r, c.g, c.b, c.a ) );
}

void PaletteEditor::showColourInHSV(Colour const& c)
{
    float r = ((float)c.r)/255.0f;
    float g = ((float)c.g)/255.0f;
    float b = ((float)c.b)/255.0f;
    float a = ((float)c.a)/255.0f;
    float h, s, v;
    RGBToHSV(r, g, b, h, s, v);
    m_HSVWidget->setHSVA(h, s, v, a);
}

void PaletteEditor::showColourInHex(Colour const& c)
{
    char buf[10];
//    if (c.a == 255) {
//        snprintf(buf, sizeof(buf), "#%02x%02x%02x", c.r, c.g, c.b);
    snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x", c.r, c.g, c.b, c.a);
    m_HexEntry->setText(buf);
}


void PaletteEditor::hexChanged()
{
    Colour c;
    if (!ParseHexColour(m_HexEntry->text().toUtf8().constData(), c)) {
        // could indicate invalid value?
        return;
    }
    Colour existing( m_Proj.GetColour( m_Selected ) );
    if (c==existing) {
        return;
    }

    // Show the change in the other widgets.
    showColourInRGB(c);
    showColourInHSV(c);

    // apply edit to project
    applyEdit(c);
}

// The rgb sliders have been twiddled.
void PaletteEditor::rgbChanged()
{
    QColor qc = m_RGBWidget->colour();
    Colour c( qc.red(), qc.green(), qc.blue(), qc.alpha() );
    Colour existing( m_Proj.GetColour( m_Selected ) );
    if (c==existing) {
        return;
    }

    // Show the change in the other widgets.
    showColourInHSV(c);
    showColourInHex(c);

    // Apply the edit to the project.
    applyEdit(c);
}

// The HSV sliders have been twiddled.
void PaletteEditor::hsvChanged()
{
    float h, s, v, a;
    m_HSVWidget->getHSVA(h, s, v, a);
    float r, g, b;
    HSVToRGB(h, s, v, r, g, b);
    Colour c((uint8_t)(r * 255.0f),
        (uint8_t)(g * 255.0f),
        (uint8_t)(b * 255.0f),
        (uint8_t)(a * 255.0f));

    Colour existing( m_Proj.GetColour( m_Selected ) );
    if (c==existing) {
        return;
    }

    // Show the change in the other widgets.
    showColourInRGB(c);
    showColourInHex(c);

    // apply edit to project
    applyEdit(c);
}


void PaletteEditor::applyEdit(Colour const &c) {
    // if the last cmd was a modification of the same colour, we'll just amend it!
    // this lets us keep the project up-to-date as user twiddles the colour,
    // but also avoids clogging up the undo stack with insane numbers of operations.
    m_Applying = true;
    Cmd* cmd = m_Ed.TopCmd();
    if (cmd)
    {
        Cmd_PaletteModify* mod = cmd->ToPaletteModify();
        if (mod)
        {
            if (mod->Merge(m_Selected, c))
            {
                m_Applying = false;
                return;
            }
        }
    }
    
    // if we get this far we need a fresh cmd.
    cmd = new Cmd_PaletteModify(m_Proj, m_Selected, 1, &c);
    m_Ed.AddCmd(cmd);
    m_Applying = false;
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

    newpalette.SpreadHSV( n0, newpalette.GetColour(n0), n1, newpalette.GetColour(n1) );

    Cmd* cmd = new Cmd_PaletteModify(m_Proj, n0, n1-n0, &newpalette.Colours[n0]);
    m_Ed.AddCmd(cmd);
}

void PaletteEditor::showColour(Colour const& c) {
    showColourInRGB(c);
    showColourInHSV(c);
    showColourInHex(c);
}


