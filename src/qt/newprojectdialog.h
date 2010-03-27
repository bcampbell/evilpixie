#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    NewProjectDialog(QWidget *parent = 0);

    QSize GetSize();
private:
    QLineEdit *m_WidthEdit;
    QLineEdit *m_HeightEdit;
};

#endif

