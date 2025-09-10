// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCAMERASELECTOR_H
#define QT3DRENDER_QCAMERASELECTOR_H

#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QCameraSelectorPrivate;

class Q_3DRENDERSHARED_EXPORT QCameraSelector : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)

public:
    explicit QCameraSelector(Qt3DCore::QNode *parent = nullptr);
    ~QCameraSelector();

    Qt3DCore::QEntity *camera() const;

public Q_SLOTS:
    void setCamera(Qt3DCore::QEntity *camera);

Q_SIGNALS:
    void cameraChanged(Qt3DCore::QEntity *camera);

protected:
    explicit QCameraSelector(QCameraSelectorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QCameraSelector)
};

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QCAMERASELECTOR_H
