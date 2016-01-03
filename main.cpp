/* Sample Qt project. */

#include <QApplication>

#include "hexviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HexViewer fs;
    fs.show();

    return a.exec();
}
