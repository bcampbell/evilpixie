#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>

#include "changefmtdialog.h"

struct modepreset {
    const char* name;
    PixelFormat fmt;
    int palette_cnt;
};

static modepreset presets[] = {
    {"RGBA",FMT_RGBA8,0},
    {"RGB",FMT_RGBX8,0},
    {"256 colour palette",FMT_I8,256},
    {"128 colour palette",FMT_I8,128},
    {"64 colour palette",FMT_I8,64},
    {"32 colour palette",FMT_I8,32},
    {"16 colour palette",FMT_I8,16},
    {"8 colour palette",FMT_I8,8},
    {"4 colour palette",FMT_I8,4},
    {"2 colour palette",FMT_I8,2},
};

const int N_PRESETS = sizeof(presets)/sizeof(modepreset);

// Find index of matching preset,
// Returns first preset (0) if none found. 
static int findPreset(PixelFormat fmt, int nColours) {
    for (int i = 0; i < N_PRESETS; ++i) {
        modepreset const& pre = presets[i];
        if(pre.fmt == fmt && pre.palette_cnt == nColours) {
            return i;
        }
    }
    return 0;
}


static std::string describeFmt(PixelFormat fmt, int nColours) {
    switch(fmt) {
        case FMT_RGBA8:
            return "RGBA";
        case FMT_RGBX8:
            return "RGB";
        case FMT_I8:
            {
                char buf[32];
                sprintf(buf, "%d colour palette", nColours);
                return std::string(buf);
            }
    }
    return "???";
}


ChangeFmtDialog::ChangeFmtDialog(QWidget *parent, PixelFormat currFmt, int currNumColours)
    : QDialog(parent)
{
    QFormLayout *l = new QFormLayout;

    {
        std::string desc = describeFmt(currFmt, currNumColours);
        QLabel* descLabel = new QLabel(desc.c_str());
        l->addRow("Current Format:", descLabel);
    }

    {
        QComboBox* w = new QComboBox(this);
        m_Format = w;
        int i;
        for(i=0;i<N_PRESETS;i++)
        {
            modepreset& pre = presets[i];
            w->addItem(pre.name,i);
        }

        num_colours = currNumColours;
        pixel_format = currFmt;
        int currPreset = findPreset(currFmt, currNumColours);
        w->setCurrentIndex(currPreset);

        connect(w, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
        l->addRow("Change to:", w);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    l->addRow(buttonBox);
    setLayout(l);
    setWindowTitle(tr("Change Image Format"));
}

void ChangeFmtDialog::formatChanged( int idx )
{
    int n = m_Format->itemData(idx).toInt();
    if( n<0 || n>=N_PRESETS) {
        return;
    }

    modepreset const& pre = presets[n];
    pixel_format = pre.fmt;
    num_colours = pre.palette_cnt;
}

