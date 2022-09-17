# Changelog

## Unreleased

- New panel to define colour ranges
- New drawmode: range
- Palette: show little black/white triangles on currently-selected colours.
- Change image format (with colour quantisation if needed)
- Brush scale2x (TODO: check usage in RGB/RGBA)
- Option to change grid settings.
- Palette editor improvements
    - Add hex widget (#rrggbb)
    - Add HSV sliders
    - Improved "spread" function using HSV
    - drag & drop colours (internally, and between other apps)
- Magnify view (M to split screen)
- "Use brush palette"/"load palette" now offer to remap image to new colours.
- Add "Remap Brush", to remap brush into current image palette.
- Bugfix: don't crash when using brushes with different palette size.
- Switch build system from cmake to meson
    - add icons/.desktop files to linux install
- Fix next/prev pen shortcut keys ('[' and ']').
- Sync colour picking between palette editor and main editor window.
- Stash spritesheet info in PNG metadata to ease reloading
- Stash grid settings in PNG metadata
- Add spare 'scratch' frame (toggle with 'j' key).
- Add "Save Palette".

## v0.2.1 (Jan 2021)

- Fix build error with Qt 5.15+

## v0.2 (March 2017)

- Support for 24 bit images and alpha (RGB and RGBA)
- Can now undo drawing which spans multiple animation frames
- Palette modifications are now undoable
- Convert anim to/from spritesheet
- Palette colours can now have transparency
- Add "colour" drawmode - brush paints with current pen
- Update from Qt4 to Qt5 (thanks, caiwan!)
- Add support for loading IFF anim5 animations
- Allow dragging files onto window to open them
- File dialogs now default to user home directory
- Reduced file format support (just png,bmp,gif,pcx,iff for now).
- Mac and Windows builds (App bundle & .msi respectively)


## v0.1 (May 2011)

- first release. Paletted images only.

