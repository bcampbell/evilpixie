#ifndef LAYERSWIDGET_H
#define LAYERSWIDGET_H

#include <QtWidgets/QWidget>
//#include <QtWidgets/QListWidget>
#include <QListWidget>


class Project;

// Widget for showing list of layers, with a selected layer
// and buttons to toggle visibility etc...
class LayersWidget : public QListWidget
{
    Q_OBJECT
public:
    LayersWidget(Project *targ, QWidget* parent = nullptr);

signals:
//    void focusChanged();

private:
    int m_Focus;
    Project& m_Proj;
private slots:
};

#endif // LAYERSWIDGET_H

