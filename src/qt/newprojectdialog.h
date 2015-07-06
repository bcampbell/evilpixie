#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QtWidgets/QDialog>
#include "../colours.h"

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;
class QString;
class QComboBox;

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    NewProjectDialog(QWidget *parent = 0);

    QSize GetSize();
    PixelFormat pixel_format;
    int num_colours;
    int num_frames;
private slots:
    void formatChanged( int idx );
    void framesChanged(int n) {num_frames=n;}
private:
    QLineEdit *m_WidthEdit;
    QLineEdit *m_HeightEdit;
    QComboBox *m_Format;
};

#endif

