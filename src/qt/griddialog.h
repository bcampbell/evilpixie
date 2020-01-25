#ifndef GRIDDIALOG_H
#define GRIDDIALOG_H

#include <QtWidgets/QDialog>
#include "../box.h"

class QSpinBox;

class GridDialog : public QDialog
{
    Q_OBJECT
public:
    GridDialog(QWidget *parent, Box const& initial);
    Box Grid();
    // TODO: emit signal upon changes...
private:
    QSpinBox *m_W;
    QSpinBox *m_H;
    QSpinBox *m_X;
    QSpinBox *m_Y;
};

#endif

