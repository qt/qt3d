// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RAYCASTINGJOB_H
#define QT3DRENDER_RENDER_RAYCASTINGJOB_H

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

#include "abstractpickingjob_p.h"
#include <Qt3DRender/qpickevent.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace PickingUtils {
typedef std::vector<RayCasting::QCollisionQueryResult::Hit> HitList;
}

class RayCastingJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RayCastingJob : public AbstractPickingJob
{
public:
    RayCastingJob();

    void markCastersDirty();
    bool runHelper() override;

    QAbstractRayCaster::Hits pick(QAbstractRayCaster *rayCaster);
    bool pick(const QList<QPair<Entity *, RayCaster *>> &entities);

protected:
    void dispatchHits(RayCaster *rayCaster, const PickingUtils::HitList &sphereHits);

private:
    Q_DECLARE_PRIVATE(RayCastingJob)

    bool m_castersDirty;
    bool m_oneEnabledAtLeast;
};

typedef QSharedPointer<RayCastingJob> RayCastingJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RAYCASTINGJOB_H
