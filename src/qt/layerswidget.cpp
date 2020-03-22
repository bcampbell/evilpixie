#include "layerswidget.h"

#include "../project.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <cstdio>

LayersWidget::LayersWidget(Project *targ, QWidget* parent ) : QListWidget(parent),
    m_Proj(*targ)
{
#if 0
    int i;
    for( i=0; i<m_Proj.NumLayers(); ++i )
    {
        QListWidgetItem* item = new QListWidgetItem();

        QWidget* widget = new QWidget();
        char buf[32];
        sprintf(buf, "layer %d",i);
        QLabel* name = new QLabel(buf);
        QPushButton* butt = new QPushButton("Bing");
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(name);
        layout->addWidget(butt);
        layout->addStretch();

        // layout.setSizeConstraint(SetFixedSize);
        widget->setLayout(layout);
        item->setSizeHint(widget->sizeHint());
        addItem(item);
        setItemWidget(item, widget);
    }
#endif
}

