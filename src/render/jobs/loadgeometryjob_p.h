// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LOADGEOMETRYJOB_H
#define QT3DRENDER_RENDER_LOADGEOMETRYJOB_H

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

#include <QSharedPointer>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class NodeManagers;
class LoadGeometryJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT LoadGeometryJob : public Qt3DCore::QAspectJob
{
public:
    explicit LoadGeometryJob(const HGeometryRenderer &handle);
    ~LoadGeometryJob();

    void setNodeManagers(NodeManagers *nodeManagers) { m_nodeManagers = nodeManagers; }

protected:
    void run() override;
    HGeometryRenderer m_handle;
    NodeManagers *m_nodeManagers;

private:
    Q_DECLARE_PRIVATE(LoadGeometryJob)
};

typedef QSharedPointer<LoadGeometryJob> LoadGeometryJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LOADGEOMETRYJOB_H
