#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <algorithm>
#include <string>
#include "palette.h"
#include "colours.h"
#include "point.h"

class Img;
struct Box;
class Layer;
class Stack;


//
struct NodePath {
    enum {SEL_NULL=0, SEL_MAIN, SEL_SPARE} sel;
    std::vector<int> path;
    int frame;  // frame within target (if layer)

    NodePath() : sel(SEL_NULL), frame(0) {}

    bool IsNull() const {
        return sel == SEL_NULL;
    }

    bool operator==(NodePath const& other) const {
        return (sel == other.sel &&
            path == other.path &&
            frame == other.frame);
    }
    bool operator!=(NodePath const& other) const {
        return (!(*this == other));
    }

    void dump() {
        printf("%d:", sel);
        for( auto i : path) {
            printf("%d/",i);
        }
        printf("f%d\n", frame);
    }
};


// Base class for anything that can be part of the layer stack.
// Think of it as like a DOM node in a web browser - it has
// a parent and child nodes. The root's parent is null.
class BaseNode {
public:
    // The data:
    std::string name;
    Point offset;
    BaseNode* parent;  // root stack has null parent
    std::vector<BaseNode*> children;
    // opacity, visibility, composite-op?

    BaseNode() : offset(0,0), parent(nullptr) {
    }

    virtual ~BaseNode() {
        assert(!parent);    // still attached!
        for (auto child : children) {
            child->parent = nullptr;
            delete child;
        }
    }

    // cheesy rtti
    virtual Layer* ToLayer() {return nullptr;}
    virtual Stack* ToStack() {return nullptr;}

    // transfers ownership
    void AddChild(BaseNode* n) {
        assert(!n->parent);
        n->parent = this;
        children.push_back(n);
    }

    // detach from parent
    void Detach() {
        assert(parent);
        auto it = std::find(parent->children.begin(), parent->children.end(), this);
        assert(it != parent->children.end());
        parent->children.erase(it);
        parent = nullptr;
    }
};



class Stack : public BaseNode {
public:
    virtual Stack* ToStack() {return this;}
};



class Frame {
public:
    int mTime;  // time, in milliseconds

    // frame owns the Img object
    Img* mImg;

    // Palette mPalette;    // per-frame palette
    Frame() : mTime(0), mImg(nullptr) {
    }

    ~Frame() {
        delete mImg;
    }
};



class Layer : public BaseNode
{
public:
    Layer();
    ~Layer();

    virtual Layer* ToLayer() {return this;}

    int NumFrames() const { return mFrames.size(); }
    Img& GetImg(int n) {
        assert(n >= 0 && n < (int)mFrames.size());
        return *mFrames[n]->mImg;
    }
    Img const& GetImgConst(int n) const {
        assert(n >= 0 && n < (int)mFrames.size());
        return *mFrames[n]->mImg;
    }
    // TODO: account for frames...
    Palette& GetPalette() { return m_Palette; }
    void SetPalette(Palette const& pal) { m_Palette=pal; }
    Palette const& GetPaletteConst() const { return m_Palette; }

    void Load(const char* filename);
    void Save(const char* filename);
    void Append(Img* img) {
        Frame* f = new Frame();
        f->mTime = 0;   // TODO
        f->mImg = img;
        mFrames.push_back(f);
    }
    void Zap();

    // transfer frames in range [srcfirst, srclast) to another Layer
    void TransferFrames(int srcfirst, int srclast, Layer& dest, int destfirst);
    // work out bounds of selection of anim (ie union of frames)
    void CalcBounds(Box& bound, int first, int last) const;

    // frame rate control (in fps)
    int FPS() const { return m_FPS; }
    void SetFPS(int fps) { m_FPS=fps; }

    void Dump() const;

    PixelFormat Fmt() const;


    // DATA

    std::vector<Frame*> mFrames;

    int m_FPS;
    Palette m_Palette;
};

#endif // LAYER_H


