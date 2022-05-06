/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
