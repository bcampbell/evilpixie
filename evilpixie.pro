TARGET = evilpixie
CONFIG += qt debug
win32:INCLUDEPATH = /c/mingw/gnuwin32/include
win32:LIBS = -L/c/mingw/gnuwin32/lib -lgif -lXpm -lDevIL
unix:LIBS = -lgif -lXpm -lIL
SOURCES += src/anim.cpp \
    src/app.cpp \
    src/box.cpp \
    src/brush.cpp \
    src/cmd.cpp \
    src/editor.cpp \
    src/editview.cpp \
    src/gifsupport.cpp \
    src/img.cpp \
    src/imgsupport.cpp \
    src/palette.cpp \
    src/project.cpp \
    src/tool.cpp \
    src/wobbly.cpp \
    src/util.cpp \
    src/xpmsupport.cpp \
    src/qt/editorwindow.cpp \
    src/qt/editviewwidget.cpp \
    src/qt/palettewidget.cpp \
    src/qt/paletteeditor.cpp \
    src/qt/miscwindows.cpp \
    src/qt/newprojectdialog.cpp \
    src/qt/resizeprojectdialog.cpp \
    src/qt/qtapp.cpp \
    src/qt/rgbwidget.cpp \
    src/qt/main.cpp

HEADERS += src/anim.h \
    src/box.h \
    src/brush.h \
    src/cmd.h \
    src/colours.h \
    src/editor.h \
    src/editview.h \
    src/global.h \
    src/img.h \
    src/imgsupport.h \
    src/palette.h \
    src/point.h \
    src/project.h \
    src/tool.h \
    src/util.h \
    src/wobbly.h \
    src/qt/editorwindow.h \
    src/qt/palettewidget.h \
    src/qt/paletteeditor.h \
    src/qt/miscwindows.h \
    src/qt/newprojectdialog.h \
    src/qt/resizeprojectdialog.h \
    src/qt/rgbwidget.h \
    src/qt/editviewwidget.h
