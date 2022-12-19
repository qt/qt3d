// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "pickboundingvolumejob_p.h"
#include "qpicktriangleevent.h"
#include "qpicklineevent.h"
#include "qpickpointevent.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/qobjectpicker_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/qpickevent_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>

#include <QSurface>
#include <QWindow>
#include <QOffscreenSurface>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
using namespace Qt3DRender::RayCasting;

namespace Render {

class PickBoundingVolumeJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    PickBoundingVolumeJobPrivate(PickBoundingVolumeJob *q) : q_ptr(q) { }
    ~PickBoundingVolumeJobPrivate() override = default;

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    enum CustomEventType {
        MouseButtonClick = QEvent::User,
    };

    struct EventDetails {
        Qt3DCore::QNodeId pickerId;
        int sourceEventType;
        QPickEventPtr resultingEvent;
        Qt3DCore::QNodeId viewportNodeId;
    };

    QList<EventDetails> dispatches;
    PickBoundingVolumeJob *q_ptr;
    Q_DECLARE_PUBLIC(PickBoundingVolumeJob)
};


bool PickBoundingVolumeJobPrivate::isRequired() const
{
    Q_Q(const PickBoundingVolumeJob);
    return !q->m_pendingMouseEvents.empty() || q->m_pickersDirty || q->m_oneEnabledAtLeast;
}

void PickBoundingVolumeJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    using namespace Qt3DCore;
    QNodeId previousId;
    QObjectPicker *node = nullptr;

    for (auto res: std::as_const(dispatches)) {
        if (previousId != res.pickerId) {
            node = qobject_cast<QObjectPicker *>(manager->lookupNode(res.pickerId));
            previousId = res.pickerId;
        }
        if (!node)
            continue;

        QObjectPickerPrivate *dnode = static_cast<QObjectPickerPrivate *>(QObjectPickerPrivate::get(node));

        // resolve front end details
        QPickEvent *pickEvent = res.resultingEvent.data();
        if (pickEvent) {
            QPickEventPrivate *dpickEvent = QPickEventPrivate::get(pickEvent);
            dpickEvent->m_viewport = static_cast<QViewport *>(manager->lookupNode(res.viewportNodeId));
            dpickEvent->m_entityPtr = static_cast<QEntity *>(manager->lookupNode(dpickEvent->m_entity));
        }

        // dispatch event
        switch (res.sourceEventType) {
        case QEvent::MouseButtonPress:
            dnode->pressedEvent(pickEvent);
            break;
        case QEvent::MouseButtonRelease:
            dnode->releasedEvent(pickEvent);
            break;
        case MouseButtonClick:
            dnode->clickedEvent(pickEvent);
            break;
        case QEvent::MouseMove:
            dnode->movedEvent(pickEvent);
            break;
        case QEvent::Enter:
            emit node->entered();
            dnode->setContainsMouse(true);
            break;
        case QEvent::Leave:
            dnode->setContainsMouse(false);
            emit node->exited();
            break;
        default: Q_UNREACHABLE();
        }
    }

    dispatches.clear();
}

namespace {

void setEventButtonAndModifiers(const QMouseEvent *event, QPickEvent::Buttons &eventButton, int &eventButtons, int &eventModifiers)
{
    switch (event->button()) {
    case Qt::LeftButton:
        eventButton = QPickEvent::LeftButton;
        break;
    case Qt::RightButton:
        eventButton = QPickEvent::RightButton;
        break;
    case Qt::MiddleButton:
        eventButton = QPickEvent::MiddleButton;
        break;
    case Qt::BackButton:
        eventButton = QPickEvent::BackButton;
        break;
    default:
        break;
    }

    if (event->buttons() & Qt::LeftButton)
        eventButtons |= QPickEvent::LeftButton;
    if (event->buttons() & Qt::RightButton)
        eventButtons |= QPickEvent::RightButton;
    if (event->buttons() & Qt::MiddleButton)
        eventButtons |= QPickEvent::MiddleButton;
    if (event->buttons() & Qt::BackButton)
        eventButtons |= QPickEvent::BackButton;
    if (event->modifiers() & Qt::ShiftModifier)
        eventModifiers |= QPickEvent::ShiftModifier;
    if (event->modifiers() & Qt::ControlModifier)
        eventModifiers |= QPickEvent::ControlModifier;
    if (event->modifiers() & Qt::AltModifier)
        eventModifiers |= QPickEvent::AltModifier;
    if (event->modifiers() & Qt::MetaModifier)
        eventModifiers |= QPickEvent::MetaModifier;
    if (event->modifiers() & Qt::KeypadModifier)
        eventModifiers |= QPickEvent::KeypadModifier;
}

} // anonymous

PickBoundingVolumeJob::PickBoundingVolumeJob()
    : AbstractPickingJob(*new PickBoundingVolumeJobPrivate(this))
    , m_pickersDirty(true)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::PickBoundingVolume, 0)
}

void PickBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

bool PickBoundingVolumeJob::processMouseEvent(QObject* object, QMouseEvent *event)
{
    m_pendingMouseEvents.emplace_back(object, std::unique_ptr<QMouseEvent>(static_cast<QMouseEvent *>(event->clone())));
    return false;
}

void PickBoundingVolumeJob::markPickersDirty()
{
    m_pickersDirty = true;
}

bool PickBoundingVolumeJob::runHelper()
{
    // Move to clear the events so that we don't process them several times
    // if run is called several times
    const auto mouseEvents = Qt3DCore::moveAndClear(m_pendingMouseEvents);

    // If we have no events return early
    if (mouseEvents.empty())
        return false;

    // Quickly look which picker settings we've got
    if (m_pickersDirty) {
        m_pickersDirty = false;
        m_oneEnabledAtLeast = false;
        m_oneHoverAtLeast = false;

        const auto activeHandles = m_manager->objectPickerManager()->activeHandles();
        for (const auto &handle : activeHandles) {
            auto picker = m_manager->objectPickerManager()->data(handle);
            m_oneEnabledAtLeast |= picker->isEnabled();
            m_oneHoverAtLeast |= picker->isHoverEnabled();
            if (m_oneEnabledAtLeast && m_oneHoverAtLeast)
                break;
        }
    }

    // bail out early if no picker is enabled
    if (!m_oneEnabledAtLeast)
        return false;

    bool hasMoveEvent = false;
    bool hasOtherEvent = false;
    // Quickly look which types of events we've got
    for (const auto &event : mouseEvents) {
        const bool isMove = (event.second->type() == QEvent::MouseMove);
        hasMoveEvent |= isMove;
        hasOtherEvent |= !isMove;
    }

    // In the case we have a move event, find if we actually have
    // an object picker that cares about these
    if (!hasOtherEvent) {
        // Retrieve the last used object picker
        ObjectPicker *lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);

        // The only way to set lastCurrentPicker is to click
        // so we can return since if we're there it means we
        // have only move events. But keep on if hover support
        // is needed
        if (lastCurrentPicker == nullptr && !m_oneHoverAtLeast)
            return false;

        const bool caresAboutMove = (hasMoveEvent &&
                                      (m_oneHoverAtLeast ||
                                        (lastCurrentPicker && lastCurrentPicker->isDragEnabled())));
        // Early return if the current object picker doesn't care about move events
        if (!caresAboutMove)
            return false;
    }

    const PickingUtils::PickConfiguration pickConfiguration(m_frameGraphRoot, m_renderSettings);
    if (pickConfiguration.vcaDetails.empty())
        return false;

    // TO DO:
    // If we have move or hover move events that someone cares about, we try to avoid expensive computations
    // by compressing them into a single one

    // For each mouse event
    for (const auto &event : mouseEvents)
        processPickEvent(pickConfiguration, event.first, event.second.get());

    // Clear Hovered elements that needs to be cleared
    // Send exit event to object pickers on which we
    // had set the hovered flag for a previous frame
    // and that aren't being hovered any longer
    clearPreviouslyHoveredPickers();
    return true;
}

void PickBoundingVolumeJob::processPickEvent(const PickingUtils::PickConfiguration &pickConfiguration, QObject *object, const QMouseEvent *event)
{
    m_hoveredPickersToClear = m_hoveredPickers;

    QPickEvent::Buttons eventButton = QPickEvent::NoButton;
    int eventButtons = 0;
    int eventModifiers = QPickEvent::NoModifier;

    setEventButtonAndModifiers(event, eventButton, eventButtons, eventModifiers);

    // For each Viewport / Camera and Area entry
    for (const PickingUtils::ViewportCameraAreaDetails &vca : pickConfiguration.vcaDetails) {
        PickingUtils::HitList sphereHits;
        QRay3D ray = rayForViewportAndCamera(vca, object, event->pos());
        if (!ray.isValid()) {
            // An invalid rays is when we've lost our surface or the mouse
            // has moved out of the viewport In case of a button released
            // outside of the viewport, we still want to notify the
            // lastCurrent entity about this.
            dispatchPickEvents(event, PickingUtils::HitList(), eventButton, eventButtons, eventModifiers, m_renderSettings->pickResultMode(),
                               vca.viewportNodeId);
            continue;
        }

        PickingUtils::HierarchicalEntityPicker entityPicker(ray);
        entityPicker.setLayerFilterIds(vca.layersFilters);

        if (entityPicker.collectHits(m_manager, m_node)) {
            if (pickConfiguration.trianglePickingRequested) {
                PickingUtils::TriangleCollisionGathererFunctor gathererFunctor;
                gathererFunctor.m_frontFaceRequested = pickConfiguration.frontFaceRequested;
                gathererFunctor.m_backFaceRequested = pickConfiguration.backFaceRequested;
                gathererFunctor.m_manager = m_manager;
                gathererFunctor.m_ray = ray;
                gathererFunctor.m_entityToPriorityTable = entityPicker.entityToPriorityTable();
                Qt3DCore::moveAtEnd(sphereHits, gathererFunctor.computeHits(entityPicker.entities(), m_renderSettings->pickResultMode()));
            }
            if (pickConfiguration.edgePickingRequested) {
                PickingUtils::LineCollisionGathererFunctor gathererFunctor;
                gathererFunctor.m_manager = m_manager;
                gathererFunctor.m_ray = ray;
                gathererFunctor.m_pickWorldSpaceTolerance = pickConfiguration.pickWorldSpaceTolerance;
                gathererFunctor.m_entityToPriorityTable = entityPicker.entityToPriorityTable();
                Qt3DCore::moveAtEnd(sphereHits, gathererFunctor.computeHits(entityPicker.entities(), m_renderSettings->pickResultMode()));
                PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
            }
            if (pickConfiguration.pointPickingRequested) {
                PickingUtils::PointCollisionGathererFunctor gathererFunctor;
                gathererFunctor.m_manager = m_manager;
                gathererFunctor.m_ray = ray;
                gathererFunctor.m_pickWorldSpaceTolerance = pickConfiguration.pickWorldSpaceTolerance;
                gathererFunctor.m_entityToPriorityTable = entityPicker.entityToPriorityTable();
                Qt3DCore::moveAtEnd(sphereHits, gathererFunctor.computeHits(entityPicker.entities(), m_renderSettings->pickResultMode()));
                PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
            }
            if (!pickConfiguration.primitivePickingRequested) {
                Qt3DCore::moveAtEnd(sphereHits, entityPicker.hits());
                PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                if (m_renderSettings->pickResultMode() != QPickingSettings::AllPicks)
                    sphereHits = { sphereHits.front() };
            }
        }

        // Dispatch events based on hit results
        dispatchPickEvents(event, sphereHits, eventButton, eventButtons, eventModifiers, m_renderSettings->pickResultMode(),
                           vca.viewportNodeId);
    }
}

void PickBoundingVolumeJob::dispatchPickEvents(const QMouseEvent *event,
                                               const PickingUtils::HitList &sphereHits,
                                               QPickEvent::Buttons eventButton,
                                               int eventButtons,
                                               int eventModifiers,
                                               bool allHitsRequested,
                                               Qt3DCore::QNodeId viewportNodeId)
{
    Q_D(PickBoundingVolumeJob);

    ObjectPicker *lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);
    // If we have hits
    if (!sphereHits.empty()) {
        // Note: how can we control that we want the first/last/all elements along the ray to be picked

        // How do we differentiate betwnee an Entity with no GeometryRenderer and one with one, both having
        // an ObjectPicker component when it comes

        for (const QCollisionQueryResult::Hit &hit : std::as_const(sphereHits)) {
            Entity *entity = m_manager->renderNodesManager()->lookupResource(hit.m_entityId);
            HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker>();

            // If the Entity which actually received the hit doesn't have
            // an object picker component, we need to check the parent if it has one ...
            while (objectPickerHandle.isNull() && entity != nullptr) {
                entity = entity->parent();
                if (entity != nullptr)
                    objectPickerHandle = entity->componentHandle<ObjectPicker>();
            }

            ObjectPicker *objectPicker = m_manager->objectPickerManager()->data(objectPickerHandle);
            if (objectPicker != nullptr && objectPicker->isEnabled()) {

                if (lastCurrentPicker && !allHitsRequested) {
                    // if there is a current picker, it will "grab" all events until released.
                    // Clients should test that entity is what they expect (or only use
                    // world coordinates)
                    objectPicker = lastCurrentPicker;
                }

                // Send the corresponding event
                Vector3D localIntersection = hit.m_intersection;
                if (entity && entity->worldTransform())
                    localIntersection = entity->worldTransform()->inverted().map(hit.m_intersection);

                QPickEventPtr pickEvent;
                const auto eventPos = event->position();
                switch (hit.m_type) {
                case QCollisionQueryResult::Hit::Triangle:
                    pickEvent.reset(new QPickTriangleEvent(eventPos,
                                                           convertToQVector3D(hit.m_intersection),
                                                           convertToQVector3D(localIntersection),
                                                           hit.m_distance,
                                                           hit.m_primitiveIndex,
                                                           hit.m_vertexIndex[0],
                                                           hit.m_vertexIndex[1],
                                                           hit.m_vertexIndex[2],
                                                           eventButton, eventButtons,
                                                           eventModifiers,
                                                           convertToQVector3D(hit.m_uvw)));
                    break;
                case QCollisionQueryResult::Hit::Edge:
                    pickEvent.reset(new QPickLineEvent(eventPos,
                                                       convertToQVector3D(hit.m_intersection),
                                                       convertToQVector3D(localIntersection),
                                                       hit.m_distance,
                                                       hit.m_primitiveIndex,
                                                       hit.m_vertexIndex[0], hit.m_vertexIndex[1],
                                                       eventButton, eventButtons, eventModifiers));
                    break;
                case QCollisionQueryResult::Hit::Point:
                    pickEvent.reset(new QPickPointEvent(eventPos,
                                                        convertToQVector3D(hit.m_intersection),
                                                        convertToQVector3D(localIntersection),
                                                        hit.m_distance,
                                                        hit.m_vertexIndex[0],
                                                        eventButton, eventButtons, eventModifiers));
                    break;
                case QCollisionQueryResult::Hit::Entity:
                    pickEvent.reset(new QPickEvent(eventPos,
                                                   convertToQVector3D(hit.m_intersection),
                                                   convertToQVector3D(localIntersection),
                                                   hit.m_distance,
                                                   eventButton, eventButtons, eventModifiers));
                    break;
                default:
                    Q_UNREACHABLE();
                }
                Qt3DRender::QPickEventPrivate::get(pickEvent.data())->m_entity = hit.m_entityId;
                switch (event->type()) {
                case QEvent::MouseButtonPress: {
                    // Store pressed object handle
                    m_currentPicker = objectPickerHandle;
                    m_currentViewport = viewportNodeId;
                    // Send pressed event to m_currentPicker
                    d->dispatches.push_back({objectPicker->peerId(), event->type(), pickEvent, viewportNodeId});
                    objectPicker->setPressed(true);
                    break;
                }

                case QEvent::MouseButtonRelease: {
                    // Only send the release event if it was pressed
                    if (objectPicker->isPressed()) {
                        d->dispatches.push_back({objectPicker->peerId(), event->type(), pickEvent, viewportNodeId});
                        objectPicker->setPressed(false);
                    }
                    if (lastCurrentPicker != nullptr && m_currentPicker == objectPickerHandle) {
                        d->dispatches.push_back({objectPicker->peerId(),
                                                 PickBoundingVolumeJobPrivate::MouseButtonClick,
                                                 pickEvent, viewportNodeId});
                        m_currentPicker = HObjectPicker();
                        m_currentViewport = {};
                    }
                    break;
                }
#if QT_CONFIG(gestures)
                case QEvent::Gesture: {
                    d->dispatches.push_back({objectPicker->peerId(),
                                             PickBoundingVolumeJobPrivate::MouseButtonClick,
                                             pickEvent, viewportNodeId});
                    break;
                }
#endif
                case QEvent::MouseMove: {
                    if ((objectPicker->isPressed() || objectPicker->isHoverEnabled()) && objectPicker->isDragEnabled())
                        d->dispatches.push_back({objectPicker->peerId(), event->type(), pickEvent, viewportNodeId});
                    Q_FALLTHROUGH(); // fallthrough
                }
                case QEvent::HoverMove: {
                    if (!m_hoveredPickers.contains(objectPickerHandle)) {
                        if (objectPicker->isHoverEnabled()) {
                            // Send entered event to objectPicker
                            d->dispatches.push_back({objectPicker->peerId(), QEvent::Enter, pickEvent, viewportNodeId});
                            // and save it in the hoveredPickers
                            m_hoveredPickers.push_back(objectPickerHandle);
                        }
                    }
                    break;
                }

                default:
                    break;
                }
            }

            // The ObjectPicker was hit -> it is still being hovered
            m_hoveredPickersToClear.removeAll(objectPickerHandle);

            lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);
        }

        // Otherwise no hits
    } else {
        switch (event->type()) {
        case QEvent::MouseButtonRelease: {
            // Send release event to m_currentPicker
            if (lastCurrentPicker != nullptr && m_currentViewport == viewportNodeId) {
                m_currentPicker = HObjectPicker();
                m_currentViewport = {};
                QPickEventPtr pickEvent(new QPickEvent);
                lastCurrentPicker->setPressed(false);
                d->dispatches.push_back({lastCurrentPicker->peerId(), event->type(), pickEvent, viewportNodeId});
            }
            break;
        }
        default:
            break;
        }
    }
}

void PickBoundingVolumeJob::clearPreviouslyHoveredPickers()
{
    Q_D(PickBoundingVolumeJob);

    for (const HObjectPicker &pickHandle : std::as_const(m_hoveredPickersToClear)) {
        ObjectPicker *pick = m_manager->objectPickerManager()->data(pickHandle);
        if (pick)
            d->dispatches.push_back({pick->peerId(), QEvent::Leave, {}, {}});
        m_hoveredPickers.removeAll(pickHandle);
    }

    m_hoveredPickersToClear.clear();
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
