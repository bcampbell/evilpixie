project('evilpixie', 'cpp', default_options: ['cpp_std=c++20'])

qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])
#qt6_dep = dependency('qt6', modules: 'Widgets')

impy_dep = dependency('impy', static: true)

incdirs = include_directories('src')

ep_headers = [
	'src/app.h',
	'src/blit.h',
	'src/blit_keyed.h',
	'src/blit_matte.h',
	'src/blit_range.h',
	'src/blit_zoom.h',
	'src/box.h',
	'src/brush.h',
	'src/cmd_changefmt.h',
	'src/cmd_remap.h',
	'src/cmd.h',
	'src/colours.h',
	'src/draw.h',
	'src/editor.h',
	'src/editview.h',
	'src/exception.h',
	'src/file_load.h',
	'src/file_save.h',
	'src/file_type.h',
	'src/global.h',
	'src/img_convert.h',
	'src/img.h',
	'src/layer.h',
	'src/lexer.h',
	'src/mousestyle.h',
	'src/palette.h',
	'src/point.h',
	'src/project.h',
	'src/projectlistener.h',
	'src/quantise.h',
	'src/ranges.h',
	'src/scale2x.h',
	'src/sheet.h',
	'src/tool.h',
	'src/util.h',
	'src/version.h']

ep_sources = ['src/app.cpp',
	'src/blit.cpp',
	'src/blit_keyed.cpp',
	'src/blit_matte.cpp',
	'src/blit_range.cpp',
	'src/blit_zoom.cpp',
	'src/box.cpp',
	'src/brush.cpp',
	'src/cmd_changefmt.cpp',
	'src/cmd_remap.cpp',
	'src/cmd.cpp',
	'src/colours.cpp',
	'src/draw.cpp',
	'src/editor.cpp',
	'src/editview.cpp',
	'src/exception.cpp',
	'src/file_load.cpp',
	'src/file_save.cpp',
	'src/file_type.cpp',
	'src/img_convert.cpp',
	'src/img.cpp',
	'src/layer.cpp',
	'src/lexer.cpp',
	'src/palette.cpp',
	'src/palettesupport.cpp',
	'src/project.cpp',
	'src/quantise.cpp',
	'src/ranges.cpp',
	'src/scale2x.cpp',
	'src/sheet.cpp',
	'src/tool.cpp',
	'src/util.cpp']

if host_machine.system() == 'windows'
    ep_sources += import('windows').compile_resources('win32/evilpixie.rc' )
endif

ep_qt_headers = [
	'src/qt/changefmtdialog.h',
	'src/qt/editorwindow.h',
	'src/qt/editviewwidget.h',
	'src/qt/griddialog.h',
	'src/qt/guistuff.h',
	'src/qt/hsvwidget.h',
	'src/qt/layerswidget.h',
	'src/qt/miscwindows.h',
	'src/qt/newprojectdialog.h',
	'src/qt/paletteeditor.h',
	'src/qt/palettewidget.h',
	'src/qt/qtapp.h',
	'src/qt/rangeswidget.h',
	'src/qt/resizeprojectdialog.h',
	'src/qt/rgbpickerwidget.h',
	'src/qt/rgbwidget.h',
	'src/qt/spritesheetdialogs.h']

ep_qt_sources = [
	'src/qt/changefmtdialog.cpp',
	'src/qt/editorwindow.cpp',
	'src/qt/editviewwidget.cpp',
	'src/qt/griddialog.cpp',
	'src/qt/guistuff.cpp',
	'src/qt/hsvwidget.cpp',
	'src/qt/layerswidget.cpp',
	'src/qt/main.cpp',
	'src/qt/miscwindows.cpp',
	'src/qt/newprojectdialog.cpp',
	'src/qt/paletteeditor.cpp',
	'src/qt/palettewidget.cpp',
	'src/qt/qtapp.cpp',
	'src/qt/rangeswidget.cpp',
	'src/qt/resizeprojectdialog.cpp',
	'src/qt/rgbpickerwidget.cpp',
	'src/qt/rgbwidget.cpp',
	'src/qt/spritesheetdialogs.cpp']

ep_qt_resources = ['resources.qrc']

moc_files = qt6.preprocess(moc_headers: ep_qt_headers,
  qresources : ep_qt_resources,
  dependencies: qt6_dep)

# generate config.h
conf_data = configuration_data()
conf_data.set('evilpixie_data_dir',
  join_paths([ get_option('prefix'), get_option('datadir'), 'evilpixie']))
configure_file(input : 'src/config.h.in',
			   output : 'config.h',
			   configuration : conf_data)

executable('evilpixie',
  sources: [ep_sources, ep_qt_sources, moc_files],
  include_directories: incdirs,
  dependencies : [qt6_dep, impy_dep], #, png_dep, gif_dep, jpeg_dep],
  win_subsystem: 'windows',
  install : true)

install_subdir('data', install_dir : 'share/evilpixie', strip_directory : true)

install_data(['packaging/icons/evilpixie48.png', 'packaging/icons/evilpixie128.png'],
  rename: ['48x48/apps/evilpixie.png', '128x128/apps/evilpixie.png'],
  install_dir : 'share/icons/hicolor')
install_data('packaging/evilpixie.desktop', install_dir : 'share/applications')

