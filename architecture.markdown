# EvilPixie Architecture


EvilPixie is split up into two layers: the core and the GUI.
The GUI uses the core layer, but the core knows nothing about the GUI.


## Notable Core classes (below the GUI level)

(note: this isn't 100% accurate currently, but it's what I'm aiming for ;-)

### Project

The core store of data.
ProjectListeners can be registered with the project.
The project can be directly modified, but the thing doing the modifying
needs to call appropriate notification functions to let any listeners know
what has been changed (to keep the display up to date, for example).
Usually, all project modifications are performed within a Cmd class
(see below). These are held by the Editor on the undo stack to implement
undo/redo.

### ProjectListener

An interface for concrete listeners to derive from.
Defines functions which are called when the project is
modified (eg drawing, palette changes, frame insertion/deletion...)

### Editor

The editor coordinates the user interaction with a project.
It has an undo stack which holds Cmds.
It keeps track of the grid and other settings.

The Editor can have multiple views.

### EditView

An editview is a viewport upon the project.

The offset and zoom level can be modified.

It handles some of the user interaction - drawing in particular -
coordinating with the Editor and using Tools.

### Cmd

A Cmd encapsulates a reversable operation upon a Project.

The base Cmd class defines Do() and Undo() methods which
must be implemented.

A Cmd should be used for any Undoable operation the user might
want to perform upon a project - drawing, resizing, adding/removing
frames, editing palettes...

Cmd-based classes are usually the only ones which actually modify the
Project data.

### Tool

Tools are owned by the editor and handle interactive operations
upon a project.

They are responsible for drawing visual feedback to the editview
(eg rubber-banding).

Some tools (eg PencilTool) generate Cmds and apply them to
the project.

Other tools fetch information for the Editor (eg EyeDropperTool).

