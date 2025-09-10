// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_UPDATEWORLDTRANSFORMJOB_H
#define QT3DRENDER_RENDER_UPDATEWORLDTRANSFORMJOB_H

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
#include <Qt3DRender/private/qt3drender_global_p.h>

#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class Entity;
class NodeManagers;
class UpdateWorldTransformJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT UpdateWorldTransformJob : public Qt3DCore::QAspectJob
{
public:
    UpdateWorldTransformJob();

    void setRoot(Entity *root);
    void setManagers(NodeManagers *manager);

    void run() override;

private:
    Entity *m_node;
    NodeManagers *m_manager;
    Q_DECLARE_PRIVATE(UpdateWorldTransformJob)
};

typedef QSharedPointer<UpdateWorldTransformJob> UpdateWorldTransformJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_UPDATEWORLDTRANSFORMJOB_H
