// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QApplication>
#include <QMainWindow>
#include <QQuickWidget>
#include <QMdiArea>
#include <QLCDNumber>
#include <QMenuBar>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    QMdiArea *centralWidget = new QMdiArea;

    QLCDNumber *lcd = new QLCDNumber;
    lcd->display(1337);
    lcd->setMinimumSize(250, 100);
    centralWidget->addSubWindow(lcd);

    QQuickWidget *quickWidget = new QQuickWidget;
    quickWidget->setMinimumSize(300, 300);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl("qrc:/main.qml"));
    centralWidget->addSubWindow(quickWidget);

    mainWindow.setCentralWidget(centralWidget);

    QMenu *fileMenu = mainWindow.menuBar()->addMenu(QObject::tr("&File"));
    fileMenu->addAction(QObject::tr("E&xit"), qApp, &QCoreApplication::quit);

    mainWindow.resize(800, 600);
    mainWindow.show();

    return app.exec();
}
