TARGET = evilpixie
CONFIG += qt debug
QT += widgets

win32 {
    INCLUDEPATH = /c/mingw/gnuwin32/include
    LIBS = -L/c/mingw/gnuwin32/lib -lgif -lXpm -lDevIL
# to make sure we don't compile a console app:
    CONFIG += windows
    RC_FILE = win32/evilpixie.rc
}

unix:LIBS = -lgif -lXpm -lDevIL
#unix:LIBS = -L/usr/local/lib -lgif -lXpm -lDevIL


SOURCES += src/anim.cpp \
    src/app.cpp \
    src/box.cpp \
    src/brush.cpp \
    src/cmd.cpp \
    src/draw.cpp \
    src/editor.cpp \
    src/editview.cpp \
    src/img.cpp \
    src/palette.cpp \
    src/project.cpp \
    src/sheet.cpp \
    src/tool.cpp \
    src/exception.cpp \
    src/util.cpp \
    src/qt/editorwindow.cpp \
    src/qt/editviewwidget.cpp \
    src/qt/palettewidget.cpp \
    src/qt/paletteeditor.cpp \
    src/qt/miscwindows.cpp \
    src/qt/newprojectdialog.cpp \
    src/qt/rgbwidget.cpp \
    src/qt/rgbpickerwidget.cpp \
    src/qt/resizeprojectdialog.cpp \
    src/qt/qtapp.cpp \
    src/qt/guistuff.cpp \
    src/qt/main.cpp

HEADERS += src/anim.h \
    src/box.h \
    src/brush.h \
    src/cmd.h \
    src/colours.h \
    src/draw.h \
    src/editor.h \
    src/editview.h \
    src/global.h \
    src/img.h \
    src/palette.h \
    src/point.h \
    src/project.h \
    src/tool.h \
    src/util.h \
    src/exception.h \
    src/qt/editorwindow.h \
    src/qt/palettewidget.h \
    src/qt/paletteeditor.h \
    src/qt/miscwindows.h \
    src/qt/newprojectdialog.h \
    src/qt/rgbwidget.h \
    src/qt/rgbpickerwidget.h \
    src/qt/resizeprojectdialog.h \
    src/qt/editviewwidget.h \
    src/qt/guistuff.h

!win32 {
    isEmpty( PREFIX ) {
      PREFIX = /usr/local
    }

    DATA_DIR = $${PREFIX}/share
    EVILPIXIE_DATA_DIR = $${DATA_DIR}/$${TARGET}
    DEFINES += EVILPIXIE_DATA_DIR=\\\"$${EVILPIXIE_DATA_DIR}\\\"

    datafiles.files = data/help.html data/default.gpl data/examples data/icons
    datafiles.path = $$EVILPIXIE_DATA_DIR

    target.path = $${PREFIX}/bin
    INSTALLS += target datafiles

}

RESOURCES += \
    resources.qrc


