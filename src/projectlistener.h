#ifndef PROJECTLISTENER_H
#define PROJECTLISTENER_H

class RGBx;
class Box;

// base class for things that want to be informed of
// changes to the Project.
//
class ProjectListener
{
public:
    virtual ~ProjectListener() {}
	virtual void OnDamaged( Box const& viewdmg ) = 0;
    virtual void OnPaletteChanged( int n, RGBx const& c ) = 0;
    virtual void OnPaletteReplaced() {}
    virtual void OnModifiedFlagChanged( bool ) {}
    virtual void OnPenChange() {}

    // called when undo or redo stacks are altered (ie project changed)
    virtual void OnUndoRedoChanged() {}
private:

};

#endif // PROJECTLISTENER_H

