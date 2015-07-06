#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QtWidgets/QDialog>

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;
class QString;

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    NewProjectDialog(QWidget *parent = 0);

    QSize GetSize();
    int num_colours;
    int num_frames;
private slots:
    void numcoloursChanged( QString const& txt );
    void framesChanged(int n) {num_frames=n;}
private:
    QLineEdit *m_WidthEdit;
    QLineEdit *m_HeightEdit;
};

#endif

