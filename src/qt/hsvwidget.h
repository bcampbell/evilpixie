#ifndef HSVWIDGET_H
#define HSVWIDGET_H

#include <QtWidgets/QWidget>
#include <QColor>

class QSlider;
class QLabel;


// HSV-twiddling widget, with slider bars for HSV+alpha components.
class HSVWidget : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY( QColor colour READ colour WRITE setColour)
public:
    HSVWidget( QWidget* parent=0 );

    // hue: [0..360], s,v,a: [0..1]
    void getHSVA(float& h, float& s, float& v, float& a) const;
    void setHSVA(float h, float s, float v, float a);

signals:
    void colourChanged();

private:
    QSlider* m_Sliders[4];
//    QLabel* m_Labels[4];

private slots:
    void hChanged(int);
    void sChanged(int);
    void vChanged(int);
    void aChanged(int);
};

#endif // HSVWIDGET_H

