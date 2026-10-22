#include "ui/MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("FinanQt"));
    resize(1024, 680);

    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

    auto *welcomeLabel = new QLabel(QStringLiteral("FinanQt - Gestor de Despesas"), centralWidget);
    welcomeLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont = welcomeLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    welcomeLabel->setFont(titleFont);

    layout->addWidget(welcomeLabel);
    setCentralWidget(centralWidget);
}
