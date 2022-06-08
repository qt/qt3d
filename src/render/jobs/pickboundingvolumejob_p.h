// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
#define QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H

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
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qpickevent.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSharedPointer>

#include <vector>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QViewport;

namespace Render {
class PickBoundingVolumeJobPrivate;

namespace PickingUtils {
typedef std::vector<RayCasting::QCollisionQueryResult::Hit> HitList;
}

class Q_3DRENDERSHARED_PRIVATE_EXPORT PickBoundingVolumeJob : public AbstractPickingJob
{
public:
    PickBoundingVolumeJob();

    void setRoot(Entity *root);
    bool processMouseEvent(QObject* object, QMouseEvent *event);
    void markPickersDirty();
    bool pickersDirty() const { return m_pickersDirty; }

    // For unit tests
    inline HObjectPicker currentPicker() const { return m_currentPicker; }
    inline QList<HObjectPicker> hoveredPickers() const { return m_hoveredPickers; }
    bool runHelper() override;

protected:
    void processPickEvent(const PickingUtils::PickConfiguration &pickConfiguration, QObject *object, const QMouseEvent *event);
    void dispatchPickEvents(const QMouseEvent *event,
                            const PickingUtils::HitList &sphereHits,
                            QPickEvent::Buttons eventButton,
                            int eventButtons,
                            int eventModifiers,
                            bool allHitsRequested,
                            Qt3DCore::QNodeId viewportNodeId);

private:
    Q_DECLARE_PRIVATE(PickBoundingVolumeJob)

    void clearPreviouslyHoveredPickers();

    std::vector<std::pair<QObject*, std::unique_ptr<QMouseEvent>>> m_pendingMouseEvents;
    bool m_pickersDirty;
    bool m_oneHoverAtLeast;
    HObjectPicker m_currentPicker;
    Qt3DCore::QNodeId m_currentViewport;
    QList<HObjectPicker> m_hoveredPickers;
    QList<HObjectPicker> m_hoveredPickersToClear;
};

typedef QSharedPointer<PickBoundingVolumeJob> PickBoundingVolumeJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
