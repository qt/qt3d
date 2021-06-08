/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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
****************************************************************************/

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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QViewport;

namespace Render {
class PickBoundingVolumeJobPrivate;

namespace PickingUtils {
typedef QVector<RayCasting::QCollisionQueryResult::Hit> HitList;
}

class Q_3DRENDERSHARED_PRIVATE_EXPORT PickBoundingVolumeJob : public AbstractPickingJob
{
public:
    PickBoundingVolumeJob();

    void setRoot(Entity *root);
    void setMouseEvents(const QList<QPair<QObject*, QMouseEvent>> &pendingEvents);
    void setKeyEvents(const QList<QKeyEvent> &pendingEvents);
    void markPickersDirty();
    bool pickersDirty() const { return m_pickersDirty; }

    // For unit tests
    inline HObjectPicker currentPicker() const { return m_currentPicker; }
    inline QVector<HObjectPicker> hoveredPickers() const { return m_hoveredPickers; }
    bool runHelper() override;

protected:
    void dispatchPickEvents(const QMouseEvent &event,
                            const PickingUtils::HitList &sphereHits,
                            QPickEvent::Buttons eventButton,
                            int eventButtons,
                            int eventModifiers,
                            bool allHitsRequested,
                            Qt3DCore::QNodeId viewportNodeId);

private:
    Q_DECLARE_PRIVATE(PickBoundingVolumeJob)

    void clearPreviouslyHoveredPickers();

    QList<QPair<QObject*, QMouseEvent>> m_pendingMouseEvents;
    QList<QKeyEvent> m_pendingKeyEvents;
    bool m_pickersDirty;
    bool m_oneHoverAtLeast;
    HObjectPicker m_currentPicker;
    Qt3DCore::QNodeId m_currentViewport;
    QVector<HObjectPicker> m_hoveredPickers;
    QVector<HObjectPicker> m_hoveredPickersToClear;
};

typedef QSharedPointer<PickBoundingVolumeJob> PickBoundingVolumeJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
