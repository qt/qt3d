// Copyright (C) 2022 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>

#include <Qt3DCore/qentity.h>
#include <QtGui/QScreen>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include <QMainWindow>
#include <QQuickWidget>

int main(int argc, char **argv)
{
    qputenv("QT3D_RENDERER", "opengl");
    qputenv("QSG_RHI_BACKEND", "opengl");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QSurfaceFormat format(QSurfaceFormat::defaultFormat());
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapInterval(0);   // 0 = vsync off
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    QMainWindow mainWindow;

    QQuickWidget *widget1 = new QQuickWidget();
    widget1->resize(QSize(400, 400));
    widget1->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget1->setSource(QUrl("qrc:/main.qml"));

    QQuickWidget *widget2 = new QQuickWidget();
    widget2->resize(QSize(400, 400));
    widget2->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget2->setSource(QUrl("qrc:/main2.qml"));

    auto centralWidget = new QWidget;
    auto layout = new QHBoxLayout;
    layout->addWidget(widget1);
    layout->addWidget(widget2);
    centralWidget->setLayout(layout);
    mainWindow.setCentralWidget(centralWidget);

    mainWindow.show();

    return app.exec();
}
