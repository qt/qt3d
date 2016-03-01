/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QRENDERASPECT_H
#define QT3DRENDER_QRENDERASPECT_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qabstractaspect.h>

QT_BEGIN_NAMESPACE

class QOpenGLContext;

namespace Qt3DRender {

namespace Render {
class Renderer;
}

class QRenderAspectPrivate;

class QT3DRENDERSHARED_EXPORT QRenderAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    enum RenderType {
        Synchronous,
        Threaded
    };

    explicit QRenderAspect(QObject *parent = Q_NULLPTR);
    explicit QRenderAspect(RenderType type, QObject *parent = Q_NULLPTR);

    void renderInitialize(QOpenGLContext *context);
    void renderSynchronous();
    void renderShutdown();

    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) Q_DECL_OVERRIDE;

protected:
    void registerBackendTypes();
    QRenderAspect(QRenderAspectPrivate &dd, QObject *parent);
    Q_DECLARE_PRIVATE(QRenderAspect)

    void onRootEntityChanged(Qt3DCore::QEntity *rootObject) Q_DECL_OVERRIDE;
    void onInitialize(const QVariantMap &data) Q_DECL_OVERRIDE;
    void onCleanup() Q_DECL_OVERRIDE;

    QVector<Qt3DCore::QAspectJobPtr> createRenderBufferJobs();
    QVector<Qt3DCore::QAspectJobPtr> createGeometryRendererJobs();

private:
    friend class Render::Renderer;
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERASPECT_H
