#include "hsvwidget.h"
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <cstdio>


#define KSCALE 1024

HSVWidget::HSVWidget( QWidget* parent ) :
    QWidget(parent)
{
    const char* nm[4] = { "H","S","V","A" };
    QGridLayout *l = new QGridLayout();
    int i;
    for( i=0; i<4; ++i )
    {
            l->addWidget( new QLabel(nm[i]),0,i );

            QSlider*s = new QSlider();
            m_Sliders[i] = s;
            s->setOrientation( Qt::Vertical );
            s->setRange( 0, KSCALE );
            s->setTracking(true);
            s->setSingleStep(KSCALE/256);
            s->setTickPosition( QSlider::TicksBothSides );
            s->setTickInterval(16*KSCALE/256 );
            l->addWidget(s,1,i);

            //m_Labels[i] = new QLabel();
            //l->addWidget( m_Labels[i], 2,i );

    }
    connect( m_Sliders[0], SIGNAL( valueChanged(int) ), this, SLOT(hChanged(int) ) );
    connect( m_Sliders[1], SIGNAL( valueChanged(int) ), this, SLOT(sChanged(int) ) );
    connect( m_Sliders[2], SIGNAL( valueChanged(int) ), this, SLOT(vChanged(int) ) );
    connect( m_Sliders[3], SIGNAL( valueChanged(int) ), this, SLOT(aChanged(int) ) );
    setLayout( l );
}

void HSVWidget::setHSVA(float h, float s, float v, float a) {

    int val[4] = {
        (int)(h * KSCALE) / 360,
        (int)(s * KSCALE),
        (int)(v * KSCALE),
        (int)(a * KSCALE),
    };
    //float f[4] = {h,s,v,a};

    int i;
    for( i=0; i<4; ++i)
    {
        bool old = m_Sliders[i]->blockSignals(true);
        m_Sliders[i]->setValue(val[i]);
//        QString s;
//        s.setNum(f[i]);
//        m_Labels[i]->setText( s );
        m_Sliders[i]->blockSignals(old);
    }
}

void HSVWidget::getHSVA(float& h, float& s, float& v, float& a) const {
    h = 360.0f * ((float)m_Sliders[0]->value()) / (float)KSCALE;
    s = 1.0f * ((float)m_Sliders[1]->value()) / (float)KSCALE;
    v = 1.0f * ((float)m_Sliders[2]->value()) / (float)KSCALE;
    a = 1.0f * ((float)m_Sliders[3]->value()) / (float)KSCALE;
}


void HSVWidget::hChanged(int)
{
/*    QString s;
    float f = 360.0f * ((float)m_Sliders[0]->value()) / (float)KSCALE;
    s.setNum(f);
    m_Labels[0]->setText(s);
*/
    emit colourChanged();
}
void HSVWidget::sChanged(int)
{
/*
    QString s;
    float f = 1.0f * ((float)m_Sliders[1]->value()) / (float)KSCALE;
    s.setNum(f);
    m_Labels[1]->setText(s);
*/
    emit colourChanged();
}
void HSVWidget::vChanged(int)
{
/*
    QString s;
    float f = 1.0f * ((float)m_Sliders[2]->value()) / (float)KSCALE;
    s.setNum(f);
    m_Labels[2]->setText(s);
*/
    emit colourChanged();
}

void HSVWidget::aChanged(int)
{
/*
    QString s;
    float f = 1.0f * ((float)m_Sliders[3]->value()) / (float)KSCALE;
    s.setNum(f);
    m_Labels[3]->setText(s);
*/
    emit colourChanged();
}


