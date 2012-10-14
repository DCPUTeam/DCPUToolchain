#include "dtidetabwidget.h"

DTIDETabWidget::DTIDETabWidget(QWidget* parent): QTabWidget(parent)
{

}

void DTIDETabWidget::goToNextTab()
{
    int cur = currentIndex() + 1;

    if(cur <= count() - 1)
        setCurrentIndex(cur);
    else
        setCurrentIndex(0);
}

void DTIDETabWidget::updateTitle(QString title)
{
    setTabText(currentIndex(), title);
}

void DTIDETabWidget::fileSave(QString path)
{
    if(path.isEmpty()) return;

    QFileInfo f(path);
    setTabText(currentIndex(), f.fileName());

    CodeEditor* ce = qobject_cast<CodeEditor*>(currentWidget());
    ce->saveFile(path, f.fileName());
}

