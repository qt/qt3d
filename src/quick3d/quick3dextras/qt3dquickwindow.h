// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QUICK_QT3DQUICKWINDOW_H
#define QT3DEXTRAS_QUICK_QT3DQUICKWINDOW_H

#include <Qt3DQuickExtras/qt3dquickextras_global.h>
#include <QtCore/QUrl>
#include <QtCore/qpointer.h>
#include <QtQuick/QQuickWindow>

QT_BEGIN_NAMESPACE

class QQmlIncubationController;

namespace Qt3DCore {
class QAbstractAspect;
namespace Quick {
class QQmlAspectEngine;
}
}

namespace Qt3DRender {
class QRenderAspect;
class QCamera;
}

namespace Qt3DInput {
class QInputAspect;
}

namespace Qt3DLogic {
class QLogicAspect;
}

namespace Qt3DExtras {

namespace Quick {

class Qt3DQuickWindowPrivate;

class Q_3DQUICKEXTRASSHARED_EXPORT Qt3DQuickWindow : public QWindow
{
    Q_OBJECT
    Q_PROPERTY(CameraAspectRatioMode cameraAspectRatioMode READ cameraAspectRatioMode WRITE setCameraAspectRatioMode NOTIFY cameraAspectRatioModeChanged)

public:
    explicit Qt3DQuickWindow(QWindow *parent = nullptr);
    ~Qt3DQuickWindow();

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setSource(const QUrl &source);
    Qt3DCore::Quick::QQmlAspectEngine *engine() const;

    enum CameraAspectRatioMode {
        AutomaticAspectRatio,
        UserAspectRatio
    };
    Q_ENUM(CameraAspectRatioMode); // LCOV_EXCL_LINE

    void setCameraAspectRatioMode(CameraAspectRatioMode mode);
    CameraAspectRatioMode cameraAspectRatioMode() const;

Q_SIGNALS:
    void cameraAspectRatioModeChanged(CameraAspectRatioMode mode);

protected:
    void showEvent(QShowEvent *e) override;

private:
    void onSceneCreated(QObject *rootObject);
    void setWindowSurface(QObject *rootObject);
    void setCameraAspectModeHelper();
    void updateCameraAspectRatio();

    Q_DECLARE_PRIVATE(Qt3DQuickWindow)
};

} // Quick

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QUICK_QT3DQUICKWINDOW_H
