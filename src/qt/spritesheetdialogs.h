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

class Project;
class Layer;

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
    ToSpritesheetDialog(QWidget *parent, Layer const* layer);
    virtual ~ToSpritesheetDialog();

    // Return the number of columns chosen by the user.
    int Columns() const;

    // projectlistener implementation
    void OnFramesAdded(int /*first*/, int /*last*/);
    void OnFramesRemoved(int /*first*/, int /*last*/);

private:
    Layer const* m_Layer;
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
class FromSpritesheetDialog : public QDialog, ProjectListener
{
    Q_OBJECT

public:
    FromSpritesheetDialog(QWidget *parent, Project* proj);
    virtual ~FromSpritesheetDialog();

    int getNWide();
    int getNHigh();


    // projectlistener implementation
    void OnFramesAdded(int /*first*/, int /*last*/);
    void OnFramesRemoved(int /*first*/, int /*last*/);

private:
    Project *m_Proj;
    QSpinBox *m_NWide;
    QSpinBox *m_NHigh;
    SheetPreviewWidget *m_Preview;

private slots:
    void rethinkPreview();
};

#endif

