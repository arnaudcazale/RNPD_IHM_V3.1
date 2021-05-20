#include "displaywindow.h"

#include <QScrollBar>

DisplayWindow::DisplayWindow()
{


}

void DisplayWindow::putData(QString &data)
{
    insertPlainText(data);
    insertPlainText("***************************\n");

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

