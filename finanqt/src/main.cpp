#include <QApplication>
#include <QLocale>

#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName(QStringLiteral("FinanQt"));
    QApplication::setApplicationDisplayName(QStringLiteral("FinanQt"));
    QApplication::setOrganizationName(QStringLiteral("FinanQt"));

    // Define localidade padrao para formato monetario e datas em portugues brasileiro
    QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
