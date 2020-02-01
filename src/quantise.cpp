#include <algorithm>
#include <queue>
#include <map>
#include "quantise.h"
#include "colours.h"
#include "img.h"
#include "palette.h"

#include <cstdio>


// Holds a single colour and a count.
struct Ent {
    uint8_t r,g,b,a;
    int n;
};

// Comparators for sorting Ents by r,g,b or alpha.
static bool cmp_r(Ent const& c1, Ent const& c2) { return c1.r < c2.r; }
static bool cmp_g(Ent const& c1, Ent const& c2) { return c1.g < c2.g; }
static bool cmp_b(Ent const& c1, Ent const& c2) { return c1.b < c2.b; }
static bool cmp_a(Ent const& c1, Ent const& c2) { return c1.a < c2.a; }


// A bucket of Ents. Supports std::span-style interface for iteration
// and slicing. Doesn't own it's data - just a view of a larger array.
struct Bucket {
    Bucket(Ent* data, size_t n) :  data(data), cnt(n) {
        // calculate derived values
        extentMin = Colour(255,255,255,255);
        extentMax = Colour(0,0,0,0);
        numPixels = 0;
        for (Ent e : *this) {
            extentMin.r = std::min(extentMin.r, e.r);
            extentMin.g = std::min(extentMin.g, e.g);
            extentMin.b = std::min(extentMin.b, e.b);
            extentMin.a = std::min(extentMin.a, e.a);
            extentMax.r = std::max(extentMax.r, e.r);
            extentMax.g = std::max(extentMax.g, e.g);
            extentMax.b = std::max(extentMax.b, e.b);
            extentMax.a = std::max(extentMax.a, e.a);
            numPixels += e.n;
        }

        //printf("box: %d ents, %d pixels\n",size(), numPixels);
    }

    Ent* data;
    size_t cnt;

    // std::span-like support
    Ent* begin() const { return data; }
    Ent* end() const { return data + cnt; }
    size_t size() const { return cnt; }
    bool empty() const { return cnt==0; }
    Bucket subspan( std::size_t offset, std::size_t count) const {
        return Bucket(data + offset, count);
    }

    // Values derived from content.
    // Would be nice to ditch these, then could eventually switch to
    // std::span when support is available.
    int numPixels;
    Colour extentMin;
    Colour extentMax;

    Colour averageColour() const {
        // TODO: possible overflow errors on 32bit?
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 0;
        int n = 0;  // total number of pixels
        for (Ent e : *this) {
            r += (int)e.r * e.n;
            g += (int)e.g * e.n;
            b += (int)e.b * e.n;
            a += (int)e.a * e.n;
            n += e.n;
        }
        return Colour((uint8_t)(r/n), (uint8_t)(g/n), (uint8_t)(b/n), (uint8_t)(a/n));
    }

    void dbug() const {
        printf("bucket: ");
        for(Ent e : *this) {
            printf("#%02x%02x%02x%02x (%d),", e.r, e.g, e.b, e.a, e.n);
        }
        printf("\n");
    }
};

static bool operator<(Bucket const& lhs, Bucket const& rhs) {
    return lhs.size() < rhs.size();
}

static void medianCut(Bucket all, std::vector<Colour>& out, int numColours);



//
void CalculatePalette(Img const& srcImg, std::vector<Colour>& out, int nColours)
{
    out.clear();
    out.reserve(nColours);

    // build histogram
    std::map<Colour, int> hist;
    int y;
    for (y = 0; y < srcImg.H(); ++y) {
        switch (srcImg.Fmt()) {
            case FMT_RGBX8:
                {
                    int x;
                    RGBX8 const* src = srcImg.PtrConst_RGBX8(0,y);
                    for (x = 0; x < srcImg.W(); ++x) {
                        Colour c(*src++);
                        hist[c]++;
                    }
                }
                break;
            case FMT_RGBA8:
                {
                    int x;
                    RGBA8 const* src = srcImg.PtrConst_RGBA8(0,y);
                    for (x = 0; x < srcImg.W(); ++x) {
                        Colour c(*src++);
                        hist[c]++;
                    }
                }
                break;
            default:
                // TODO: handle indexed images ;-)
                assert(false);  // not supported...
                break;
        }
    }

    if (hist.size() <= (size_t)nColours) {
        // no colour reduction needed!
        for (auto const& dat : hist) {
            out.push_back(dat.first);
        }
        return;
    }

    std::vector<Ent> ents;
    for (auto const& dat : hist) {
        Colour const& c = dat.first;
        Ent ent = {c.r, c.g, c.b, c.a, dat.second };
        ents.push_back(ent);
    }
    // Done with histogram
    hist.clear();

    // Pick a set of colours
    Bucket all(&ents.front(), ents.size());
    medianCut(all, out, nColours);
}



static void medianCut(Bucket all, std::vector<Colour>& out, int numColours) {
    assert(all.size() >= (size_t)numColours);
    std::priority_queue<Bucket> buckets;
    buckets.push(all);
    while (buckets.size() < (size_t)numColours) {
        Bucket b = buckets.top();

        // find major axis
        Colour const& minVal = b.extentMin;
        Colour const& maxVal = b.extentMax;
        Colour r(maxVal.r - minVal.r, maxVal.g - minVal.g, maxVal.b - minVal.b,
            maxVal.a - minVal.a);

        // Sort bucket contents by the component with the largest range.
        if (r.r > r.g && r.r > r.b && r.r > r.a) {
            std::sort(b.begin(), b.end(), cmp_r);
        } else if (r.g > r.r && r.g > r.b && r.g > r.a) {
            std::sort(b.begin(), b.end(), cmp_g);
        } else if (r.b > r.r && r.b > r.g && r.b > r.a) {
            std::sort(b.begin(), b.end(), cmp_b);
        } else if (r.a > r.r && r.a > r.g && r.a > r.b) {
            std::sort(b.begin(), b.end(), cmp_a);
        }

        // Split bucket.
        size_t half = b.size() / 2;
        //printf("Split. %d => %d:%d\n", b.size(), half, b.size()-half);
        buckets.pop();
        buckets.push(b.subspan(0,half));
        buckets.push(b.subspan(half,b.size()-half));

        // dump
#if 0
        std::priority_queue<Bucket> tmp = buckets;
        while (!tmp.empty()) {
            Bucket b = tmp.top();
            tmp.pop();
            b.dbug();
        }
        printf("----\n");
#endif
    }

    while(!buckets.empty()) {
        Colour c = buckets.top().averageColour();
        buckets.pop();
        out.push_back(c);
    }
}

#if 0
int main() {
    Ent fook[7] = {
        {1,0,0,255, 1},
        {2,0,0,255, 1},
        {3,40,0,255, 2},
        {4,0,0,255, 1},
        {0,100,0,255, 1},
        {0,20,0,255, 1},
        {0,0,0,255, 100},
    };

    std::vector<Colour> out;
    Bucket all(fook, 7);
    medianCut(all, out, 4);
    for( auto c : out) {
        printf("%d %d %d %d\n",c.r, c.g, c.b, c.a);
    }
    return 0;
}
#endif

