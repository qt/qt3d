// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef QT3DWINDOW_H
#define QT3DWINDOW_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qrenderapi.h>
#include <QtGui/QWindow>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAspectEngine;
class QAbstractAspect;
class QEntity;
}

namespace Qt3DRender {
class QCamera;
class QFrameGraphNode;
class QRenderAspect;
class QRenderSettings;
}

namespace Qt3DExtras {
class QForwardRenderer;
}

namespace Qt3DInput {
class QInputAspect;
class QInputSettings;
}

namespace Qt3DLogic {
class QLogicAspect;
}

namespace Qt3DExtras {

class Qt3DWindowPrivate;

class Q_3DEXTRASSHARED_EXPORT Qt3DWindow : public QWindow
{
    Q_OBJECT
public:
    Qt3DWindow(QScreen *screen = nullptr, Qt3DRender::API = Qt3DRender::API::RHI);
    ~Qt3DWindow();

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setRootEntity(Qt3DCore::QEntity *root);

    void setActiveFrameGraph(Qt3DRender::QFrameGraphNode *activeFrameGraph);
    Qt3DRender::QFrameGraphNode *activeFrameGraph() const;
    Qt3DExtras::QForwardRenderer *defaultFrameGraph() const;

    Qt3DRender::QCamera *camera() const;
    Qt3DRender::QRenderSettings *renderSettings() const;

public Q_SLOTS:

Q_SIGNALS:

protected:
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *e) override;

private:
    Q_DECLARE_PRIVATE(Qt3DWindow)
};

Q_3DEXTRASSHARED_EXPORT
void setupWindowSurface(QWindow* window, Qt3DRender::API) noexcept;

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DWINDOW_H
