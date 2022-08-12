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
#include "../sheet.h"


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
    ToSpritesheetDialog(QWidget *parent, SpriteGrid const& initialGrid, Project& proj, NodePath const& targ);
    virtual ~ToSpritesheetDialog();

    // Return the chosen layout.
    SpriteGrid const& getGrid() const
        {return mGrid;}

    // projectlistener implementation
    void OnFramesAdded(int /*first*/, int /*last*/);
    void OnFramesRemoved(int /*first*/, int /*last*/);

private:
    Project& mProj;
    NodePath mTarg;
    SpriteGrid mGrid;

    QSpinBox *mWidth;
    SheetPreviewWidget *mPreview;
    QLabel *mInfo;

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
    FromSpritesheetDialog(QWidget *parent, Img const& srcImg, SpriteGrid const& initialGrid);
    virtual ~FromSpritesheetDialog() {}

    SpriteGrid const& getGrid() const
        {return mGrid;}

private:
    Img const& mSrcImg;
    SpriteGrid mGrid;
    QSpinBox *mNWide;
    QSpinBox *mNHigh;
    SheetPreviewWidget *mPreview;

private slots:
    void rethink();
};

#endif

