#ifndef SPRITESHEETDIALOGS_H
#define SPRITESHEETDIALOGS_H

#include <QtWidgets/QDialog>

//class QDialogButtonBox;
//class QLabel;
//class QLineEdit;
class QSlider;

#include <vector>

#include "../box.h"
#include "../projectlistener.h"

class Project;



class SheetPreviewWidget : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY(int nwide READ getNWide WRITE setNWide)
//    Q_PROPERTY(int frames READ getFrames WRITE setFrames)
public:
    SheetPreviewWidget(QWidget* parent) :
        QWidget(parent)
        {}

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;

    void setFrames(std::vector<Box> const& frames);
protected:
     void paintEvent(QPaintEvent *event);
private:
    Box m_Extent;
    std::vector<Box> m_Frames; 
};

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
    QSlider *m_Width;
    SheetPreviewWidget *m_Preview;
    void rethinkPreview();

private slots:
    void widthChanged(int);
};

#endif

