#include "DTIDEDirView.h"
#include <QDebug>

DTIDEDirView::DTIDEDirView(QString path, QWidget* parent): QTreeView(parent)
{
    QFileSystemModel* model = new QFileSystemModel();
//    model->setFilter(QDir::NoDotAndDotDot);

    setModel(model);
    setRootIndex(model->setRootPath(path));
    hideColumn(1);
    hideColumn(2);
    hideColumn(3);
    
    resize(0, 50);
}
