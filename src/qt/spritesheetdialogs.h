#ifndef SPRITESHEETDIALOGS_H
#define SPRITESHEETDIALOGS_H

#include <QtWidgets/QDialog>

//class QDialogButtonBox;
class QLabel;
class QLineEdit;
//class QSlider;
class QSpinBox;

#include <vector>

#include "../box.h"
#include "../projectlistener.h"
#include "../project.h"
#include "../layer.h"


//---------------------------------------
// display a scaled-down sprite sheet layout
// for use as a preview
class SheetPreviewWidget : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY(int nwide READ getNWide WRITE setNWide)
//    Q_PROPERTY(int frames READ getFrames WRITE setFrames)
public:
    SheetPreviewWidget(QWidget* parent);
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;

    // set the frame layout, and the overall size of the containing image
    void setFrames(std::vector<Box> const& frames, Box const& contain);
protected:
     void paintEvent(QPaintEvent *event);
private:
    Box m_Contain;
    std::vector<Box> m_Frames; 
};

//---------------------------------------
// GUI for converting an anim into a single image spritesheet
//
class ToSpritesheetDialog : public QDialog, ProjectListener
{
    Q_OBJECT

public:
    ToSpritesheetDialog(QWidget *parent, Project& proj, NodePath const& targ);
    virtual ~ToSpritesheetDialog();

    // Return the number of columns chosen by the user.
    int Columns() const;

    // projectlistener implementation
    void OnFramesAdded(int /*first*/, int /*last*/);
    void OnFramesRemoved(int /*first*/, int /*last*/);

private:
    Project& m_Proj;
    NodePath m_Targ;
    QSpinBox *m_Width;
    SheetPreviewWidget *m_Preview;
    QLabel *m_Info;
    void rethinkPreview();

private slots:
    void widthChanged(int);
};



//---------------------------------------
// GUI for splitting up a single image spritesheet back into an anim
//
class FromSpritesheetDialog : public QDialog
{
    Q_OBJECT

public:
    FromSpritesheetDialog(QWidget *parent, Img const& srcImg);
    virtual ~FromSpritesheetDialog() {}

    int getNWide();
    int getNHigh();


private:
    Img const& mSrcImg;
    QSpinBox *mNWide;
    QSpinBox *mNHigh;
    SheetPreviewWidget *mPreview;

private slots:
    void rethinkPreview();
};

#endif

