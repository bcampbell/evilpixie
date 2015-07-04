#ifndef MISCWINDOWS_H
#define MISCWINDOWS_H

#include <QtWidgets/QDialog>

class AboutBox : public QDialog
{
    Q_OBJECT;
public:
    AboutBox( QWidget* parent=0 );

signals:

protected:
private:
};


class HelpWindow : public QDialog
{
    Q_OBJECT;
public:
    HelpWindow();
signals:
protected:
private:
};

#endif

