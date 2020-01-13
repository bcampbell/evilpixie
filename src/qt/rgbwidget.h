#ifndef RGBWIDGET_H
#define RGBWIDGET_H

#include <QtWidgets/QWidget>
#include <QColor>

class QSlider;
class QLabel;


// RGB-twiddling widget, with slider bars for RGB+alpha components.
class RGBWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QColor colour READ colour WRITE setColour)
public:
    RGBWidget( QWidget* parent=0 );

    QColor colour() const;
    void setColour( QColor const& c );

signals:
    void colourChanged();
private:
    QSlider* m_Sliders[4];
    QLabel* m_Labels[4];
private slots:
    void redChanged(int);
    void greenChanged(int);
    void blueChanged(int);
    void alphaChanged(int);

};

#endif // RGBWIDGET_H
