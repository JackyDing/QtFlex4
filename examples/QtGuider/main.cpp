#include "QtGuider.h"
#include "QtFlexStyle.h"
#include <time.h>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    qsrand(clock());
    QApplication::setStyle(new FlexStyle());
    QApplication a(argc, argv);
    QPixmap pixmap(":/Resources/guider.png");

    QIcon guider;
    guider.addPixmap(pixmap.copy(0, 0, 24, 24), QIcon::Normal, QIcon::Off);
    guider.addPixmap(pixmap.copy(0, 24, 24, 24), QIcon::Normal, QIcon::On);
    guider.addPixmap(pixmap.copy(24, 0, 24, 24), QIcon::Active, QIcon::On);
    guider.addPixmap(pixmap.copy(24, 24, 24, 24), QIcon::Active, QIcon::Off);

    QApplication::setWindowIcon(guider);

    QtGuider w;
    w.show();

    return a.exec();
}
