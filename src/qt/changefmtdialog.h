#ifndef CHANGEFMTDIALOG_H
#define CHANGEFMTDIALOG_H

#include <QDialog>
#include <QtWidgets/QDialog>
#include "../colours.h"

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;
class QString;
class QComboBox;

// Dialog box to select params for changing to a different image format.
class ChangeFmtDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeFmtDialog(QWidget *parent = 0);

    PixelFormat pixel_format;
    int num_colours;
private slots:
    void formatChanged( int idx );
private:
    QComboBox *m_Format;
};

#endif

