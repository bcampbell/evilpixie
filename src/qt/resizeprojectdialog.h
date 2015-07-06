#ifndef RESIZEPROJECTDIALOG_H
#define RESIZEPROJECTDIALOG_H

#include <QtWidgets/QDialog>

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;


// crappy resize project dialog
class ResizeProjectDialog : public QDialog
{
    Q_OBJECT

public:
    ResizeProjectDialog(QWidget *parent, QRect const& initial);

    QRect GetArea();
private:
    QLineEdit *m_WidthEdit;
    QLineEdit *m_HeightEdit;
};

#endif

