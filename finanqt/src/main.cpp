#include <QApplication>
#include <QLocale>

#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setOrganizationName(QStringLiteral("FinanQt"));
    QApplication::setApplicationName(QStringLiteral("FinanQt"));
    QApplication::setApplicationDisplayName(QStringLiteral("FinanQt"));

    // Localidade padrao para formatacao monetaria e datas em portugues brasileiro
    QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
