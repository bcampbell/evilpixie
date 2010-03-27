TARGET = evilpixie
CONFIG += qt release
win32:INCLUDEPATH = /c/mingw/gnuwin32/include
win32:LIBS = -L/c/mingw/gnuwin32/lib -lgif -lXpm -lpng
unix:LIBS = -lgif -lXpm -lpng -lIL
SOURCES += src/app.cpp \
    src/box.cpp \
    src/brush.cpp \
    src/cmd.cpp \
    src/editor.cpp \
    src/editview.cpp \
    src/gifsupport.cpp \
    src/img.cpp \
    src/imgsupport.cpp \
    src/palette.cpp \
    src/pngsupport.cpp \
    src/project.cpp \
    src/tool.cpp \
    src/wobbly.cpp \
    src/util.cpp \
    src/xpmsupport.cpp \
    src/palettesupport.cpp \
    src/qt/editorwindow.cpp \
    src/qt/editviewwidget.cpp \
    src/qt/palettewidget.cpp \
    src/qt/paletteeditor.cpp \
    src/qt/newprojectdialog.cpp \
    src/qt/qtapp.cpp \
    src/qt/rgbwidget.cpp \
    src/qt/main.cpp

HEADERS += src/box.h \
    src/brush.h \
    src/cmd.h \
    src/colours.h \
    src/editor.h \
    src/editview.h \
    src/global.h \
    src/img.h \
    src/palette.h \
    src/point.h \
    src/project.h \
    src/tool.h \
    src/util.h \
    src/wobbly.h \
    src/qt/editorwindow.h \
    src/qt/palettewidget.h \
    src/qt/paletteeditor.h \
    src/qt/newprojectdialog.h \
    src/qt/rgbwidget.h \
    src/qt/editviewwidget.h
