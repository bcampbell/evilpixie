#include "rgbwidget.h"
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <cstdio>


RGBWidget::RGBWidget( QWidget* parent ) :
    QWidget(parent)
{
    const char* nm[3] = { "red","green","blue" };
    QGridLayout *l = new QGridLayout();
    int i;
    for( i=0; i<3; ++i )
    {
            l->addWidget( new QLabel(nm[i]),0,i );

            QSlider*s = new QSlider();
            m_Sliders[i] = s;
            s->setOrientation( Qt::Vertical );
            s->setRange( 0,255 );
            s->setTracking(true);
            s->setSingleStep(1);
            s->setTickPosition( QSlider::TicksBothSides );
            s->setTickInterval( 16 );
            l->addWidget(s,1,i);

            m_Labels[i] = new QLabel();
            l->addWidget( m_Labels[i], 2,i );

    }
    connect( m_Sliders[0], SIGNAL( valueChanged(int) ), this, SLOT(redChanged(int) ) );
    connect( m_Sliders[1], SIGNAL( valueChanged(int) ), this, SLOT(greenChanged(int) ) );
    connect( m_Sliders[2], SIGNAL( valueChanged(int) ), this, SLOT(blueChanged(int) ) );
    setLayout( l );
}


QColor RGBWidget::colour() const
{
    return QColor( m_Sliders[0]->value(), m_Sliders[1]->value(), m_Sliders[2]->value() );
}

void RGBWidget::setColour( QColor const& c )
{
    int rgb[3];
    c.getRgb(&rgb[0], &rgb[1], &rgb[2]);

    int i;
    for( i=0; i<3; ++i)
    {
        bool old = m_Sliders[i]->blockSignals(true);
        m_Sliders[i]->setValue(rgb[i]);
        QString s;
        s.setNum(m_Sliders[i]->value());
        m_Labels[i]->setText( s );
        m_Sliders[i]->blockSignals(old);
    }
}

void RGBWidget::redChanged(int)
{
    QString s;
    s.setNum(m_Sliders[0]->value());
    m_Labels[0]->setText( s );
    emit colourChanged();
}
void RGBWidget::greenChanged(int)
{
    QString s;
    s.setNum(m_Sliders[1]->value());
    m_Labels[1]->setText( s );
    emit colourChanged();
}
void RGBWidget::blueChanged(int)
{
    QString s;
    s.setNum(m_Sliders[2]->value());
    m_Labels[2]->setText( s );
    emit colourChanged();
}

