// Copyright (C) 2020 The Qt Company.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QScreen>
#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QPushButton>

void configureMainWindow(QMainWindow *w, QMdiArea *mdiArea, QPushButton *button)
{
    auto widget = new QWidget;
    auto layout = new QVBoxLayout;
    layout->addWidget(mdiArea);
    layout->addWidget(button);
    widget->setLayout(layout);
    w->setCentralWidget(widget);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow w1;
    w1.winId();
    w1.windowHandle()->setScreen(QGuiApplication::screens().at(0));
    auto mdiArea1 = new QMdiArea;
    auto button1 = new QPushButton("Switch to this window");
    configureMainWindow(&w1, mdiArea1, button1);
    w1.setGeometry(0, 0, 800, 800);
    w1.show();

    QMainWindow w2;
    w2.winId();
    const int secondScreenIdx = QGuiApplication::screens().size() > 1 ? 1 : 0;
    w2.windowHandle()->setScreen(QGuiApplication::screens().at(secondScreenIdx));
    auto mdiArea2 = new QMdiArea;
    auto button2 = new QPushButton("Switch to this window");
    configureMainWindow(&w2, mdiArea2, button2);
    w2.setGeometry(0, 0, 800, 800);
    w2.show();

    QMdiSubWindow* subWindow = new QMdiSubWindow();

    QQuickWidget *quickWidget = new QQuickWidget();
    quickWidget->resize(QSize(400, 400));
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl("qrc:/main.qml"));

    subWindow->setWidget(quickWidget);

    QObject::connect(button1, &QPushButton::clicked,
                     &app, [mdiArea1, mdiArea2, subWindow, button1, button2]() {
                         mdiArea2->removeSubWindow(subWindow);
                         mdiArea1->addSubWindow(subWindow);
                         subWindow->show();
                         button1->setEnabled(false);
                         button2->setEnabled(true);
                     });

    QObject::connect(button2, &QPushButton::clicked,
                     &app, [mdiArea1, mdiArea2, subWindow, button1, button2]() {
                         mdiArea1->removeSubWindow(subWindow);
                         mdiArea2->addSubWindow(subWindow);
                         subWindow->show();
                         button1->setEnabled(true);
                         button2->setEnabled(false);
                     });

    mdiArea2->addSubWindow(subWindow);
    button2->setEnabled(false);
    subWindow->show();

    return app.exec();
}
