// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_SENDBUFFERCAPTUREJOB_P_H
#define QT3DRENDER_SENDBUFFERCAPTUREJOB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <QMutex>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class NodeManagers;
class Entity;
class Renderer;
class Buffer;
class SendBufferCaptureJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT SendBufferCaptureJob : public Qt3DCore::QAspectJob
{
public:
    explicit SendBufferCaptureJob();
    ~SendBufferCaptureJob();

    void setManagers(NodeManagers *nodeManagers) { m_nodeManagers = nodeManagers; }
    void addRequest(QPair<Qt3DCore::QNodeId, QByteArray> request);
    bool hasRequests() const;

    void run() final;

private:
    Q_DECLARE_PRIVATE(SendBufferCaptureJob)
    NodeManagers *m_nodeManagers;
};

typedef QSharedPointer<SendBufferCaptureJob> SendBufferCaptureJobPtr;

} //Render

} //Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_SENDBUFFERCAPTUREJOB_P_H
