#ifndef SPRITESHEETDIALOGS_H
#define SPRITESHEETDIALOGS_H

#include <QtWidgets/QDialog>

//class QDialogButtonBox;
//class QLabel;
class QLineEdit;

#include "../projectlistener.h"

class Project;

class ToSpritesheetDialog : public QDialog, ProjectListener
{
    Q_OBJECT

public:
    ToSpritesheetDialog(QWidget *parent, Project* proj);
    virtual ~ToSpritesheetDialog();

    int NumAcross();


    // projectlistener implementation
    void OnFramesAdded(int /*first*/, int /*last*/);
    void OnFramesRemoved(int /*first*/, int /*last*/);
private:
    Project *m_Proj;
    QLineEdit *m_WidthEdit;
    QLineEdit *m_HeightEdit;
};

#endif

