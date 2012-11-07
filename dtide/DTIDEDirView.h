#ifndef DTIDEDIRVIEW_H
#define DTIDEDIRVIEW_H

#include <QTreeView>
#include <QFileSystemModel>

class DTIDEDirView: public QTreeView
{
    Q_OBJECT

public:
    DTIDEDirView(QString path, QWidget* parent = 0);

};

#endif
