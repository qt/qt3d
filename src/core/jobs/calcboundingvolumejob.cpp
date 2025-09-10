// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "calcboundingvolumejob_p.h"

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qboundingvolume.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/qcoreaspect.h>
#include <Qt3DCore/private/job_common_p.h>
#include <Qt3DCore/private/qcoreaspect_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qattribute_p.h>
#include <Qt3DCore/private/qboundingvolume_p.h>
#include <Qt3DCore/private/qbuffer_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DCore/private/qgeometryview_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qthreadpooler_p.h>

#include <QtCore/qmath.h>
#if QT_CONFIG(concurrent)
#include <QtConcurrent/QtConcurrent>
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace {

BoundingVolumeComputeData findBoundingVolumeComputeData(QGeometryView *node)
{
    if (!node->isEnabled())
        return {};

    if (node->primitiveType() == QGeometryView::Patches)
        return {};

    QGeometry *geom = node->geometry();
    QGeometryPrivate *dgeom = QGeometryPrivate::get(geom);
    if (!geom)
        return {};

    int drawVertexCount = node->vertexCount(); // may be 0, gets changed below if so

    QAttribute *positionAttribute = dgeom->m_boundingVolumePositionAttribute;
    const QList<Qt3DCore::QAttribute *> attributes = geom->attributes();

    // Use the default position attribute if attribute is null
    if (!positionAttribute) {
        for (QAttribute *attr : attributes) {
            if (attr->name() == QAttribute::defaultPositionAttributeName()) {
                positionAttribute = attr;
                break;
            }
        }
    }

    if (!positionAttribute
        || positionAttribute->attributeType() != QAttribute::VertexAttribute
        || positionAttribute->vertexBaseType() != QAttribute::Float
        || positionAttribute->vertexSize() < 3) {
        qWarning("findBoundingVolumeComputeData: Position attribute not suited for bounding volume computation");
        return {};
    }

    Qt3DCore::QBuffer *positionBuffer = positionAttribute->buffer();
    // No point in continuing if the positionAttribute doesn't have a suitable buffer
    if (!positionBuffer) {
        qWarning("findBoundingVolumeComputeData: Position attribute not referencing a valid buffer");
        return {};
    }

    // Check if there is an index attribute.
    QAttribute *indexAttribute = nullptr;
    Qt3DCore::QBuffer *indexBuffer = nullptr;

    for (const auto attr : attributes) {
        if (attr->attributeType() == QAttribute::IndexAttribute) {
            indexBuffer = attr->buffer();
            if (indexBuffer) {
                indexAttribute = attr;

                if (!drawVertexCount)
                    drawVertexCount = static_cast<int>(indexAttribute->count());

                static const QAttribute::VertexBaseType validIndexTypes[] = {
                    QAttribute::UnsignedShort,
                    QAttribute::UnsignedInt,
                    QAttribute::UnsignedByte
                };

                if (std::find(std::begin(validIndexTypes),
                              std::end(validIndexTypes),
                              indexAttribute->vertexBaseType()) == std::end(validIndexTypes)) {
                    qWarning() << "findBoundingVolumeComputeData: Unsupported index attribute type" << indexAttribute->name() << indexAttribute->vertexBaseType();
                    return {};
                }

                break;
            }
        }
    }

    if (!indexAttribute && !drawVertexCount)
        drawVertexCount = static_cast<int>(positionAttribute->count());

    return { nullptr, nullptr, positionAttribute, indexAttribute, drawVertexCount };
}

bool isTreeEnabled(QEntity *entity) {
    if (!entity->isEnabled())
        return false;

    QEntity *parent = entity->parentEntity();
    while (parent) {
        if (!parent->isEnabled())
            return false;
        parent = parent->parentEntity();
    }

    return true;
}

struct UpdateBoundFunctor
{
    // This define is required to work with QtConcurrent
    typedef std::vector<BoundingVolumeComputeResult> result_type;
    result_type operator ()(const BoundingVolumeComputeData &data)
    {
        return { data.compute() };
    }
};

struct ReduceUpdateBoundFunctor
{
    void operator ()(std::vector<BoundingVolumeComputeResult> &result, const std::vector<BoundingVolumeComputeResult> &values)
    {
        for (const auto &value : values) {
            if (value.valid()) {
                result.push_back(value);
            }
        }
    }
};

} // anonymous


BoundingVolumeComputeData BoundingVolumeComputeData::fromView(QGeometryView *view)
{
    return findBoundingVolumeComputeData(view);
}

BoundingVolumeComputeResult BoundingVolumeComputeData::compute() const
{
    BoundingVolumeCalculator calculator;
    if (calculator.apply(positionAttribute, indexAttribute, vertexCount,
                         provider->view()->primitiveRestartEnabled(),
                         provider->view()->restartIndexValue()))
        return {
            entity, provider, positionAttribute, indexAttribute,
            calculator.min(), calculator.max(),
            calculator.center(), calculator.radius()
        };
    return {};
}


CalculateBoundingVolumeJob::CalculateBoundingVolumeJob(QCoreAspect *aspect)
    : Qt3DCore::QAspectJob()
    , m_aspect(aspect)
    , m_root(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::CalcBoundingVolume, 0)
}

bool CalculateBoundingVolumeJob::isRequired()
{
    if (!m_aspect)
        return true;

    auto daspect = QCoreAspectPrivate::get(m_aspect);
    return daspect->m_boundingVolumesEnabled;
}

void CalculateBoundingVolumeJob::run()
{
    using QBufferPrivate = Qt3DCore::QBufferPrivate;

    // There's 2 bounding volume jobs, one here in Core, the other in Render.
    // - This one computes bounding volumes for entities that have QBoundingVolume
    //   components and use QGeometryViews.
    //   In that case the component is updated directly by this job (since core
    //   aspect does not maintain backend objects for the component)
    // - The one in render does 2 things:
    //   . Copy the results of this job to the backend object for entities that
    //     use QBoundingVolume and QGeometryView (computed results arrive one
    //     frame later, explicit results arrive on time)
    //   . Compute the BV for old style QGeometryRenderer which use a QGeometry
    //     directly without a QGeometryView
    //
    // (see more details in Qt3DRender::CalculateBoundingVolumeJob::run)

    m_results.clear();

    QHash<QEntity *, BoundingVolumeComputeData> dirtyEntities;
    QNodeVisitor visitor;
    visitor.traverse(m_root, [](QNode *) {}, [&dirtyEntities, this](QEntity *entity) {
        if (!isTreeEnabled(entity))
            return;

        const auto bvProviders = entity->componentsOfType<QBoundingVolume>();
        if (bvProviders.isEmpty())
            return;

        // we go through the list until be find a dirty provider,
        // or THE primary provider
        bool foundBV = false;
        for (auto bv: bvProviders) {
            auto dbv = QBoundingVolumePrivate::get(bv);
            if (foundBV && !dbv->m_primaryProvider)
                continue;

            BoundingVolumeComputeData bvdata;
            if (dbv->m_explicitPointsValid) {
                // we have data explicitly set by the user, pass it to the
                // watchers as computed data
                BoundingVolumeComputeResult r;
                r.entity = entity;
                r.provider = bv;
                r.m_min = dbv->m_minPoint;
                r.m_max = dbv->m_maxPoint;
                const auto diagonal = r.m_max - r.m_min;
                r.m_center = r.m_min + diagonal * .5f;
                r.m_radius = diagonal.length();

                for (auto w: m_watchers) {
                    auto wp = w.toStrongRef();
                    if (wp)
                        wp->process(r, false);
                }
                continue;
            } else if (bv->view()) {
                bvdata = findBoundingVolumeComputeData(bv->view());
                if (!bvdata.valid())
                    continue;
                bvdata.entity = entity;
                bvdata.provider = bv;
            } else {
                // no view, can't compute
                continue;
            }

            bool dirty = QEntityPrivate::get(entity)->m_dirty;
            dirty |= QGeometryViewPrivate::get(bv->view())->m_dirty;
            dirty |= QGeometryPrivate::get(bv->view()->geometry())->m_dirty;
            dirty |= QAttributePrivate::get(bvdata.positionAttribute)->m_dirty;
            dirty |= QBufferPrivate::get(bvdata.positionAttribute->buffer())->m_dirty;
            if (bvdata.indexAttribute) {
                dirty |= QAttributePrivate::get(bvdata.indexAttribute)->m_dirty;
                dirty |= QBufferPrivate::get(bvdata.indexAttribute->buffer())->m_dirty;
            }

            if (dbv->m_primaryProvider) {
                if (dirty)
                    dirtyEntities[entity] = bvdata;
                break;
            } else if (dirty) {
                dirtyEntities[entity] = bvdata;
                foundBV = true;
            }
        }
    });

#if QT_CONFIG(concurrent)
    if (dirtyEntities.size() > 1 && QAspectJobManager::idealThreadCount() > 1) {
        UpdateBoundFunctor functor;
        ReduceUpdateBoundFunctor reduceFunctor;
        m_results = QtConcurrent::blockingMappedReduced<decltype(m_results)>(dirtyEntities, functor, reduceFunctor);
    } else
#endif
    {
        for (auto it = dirtyEntities.begin(); it != dirtyEntities.end(); ++it) {
            auto res = it.value().compute();
            if (res.valid())
                m_results.push_back(res); // How do we push it to the backends????
        }
    }

    // pass the computed results to the watchers
    for (auto &watcher: m_watchers) {
        auto watcherPtr = watcher.toStrongRef();
        if (watcherPtr) {
            for (const auto &r: m_results)
                watcherPtr->process(r, true);
        }
    }
}

void CalculateBoundingVolumeJob::postFrame(QAspectEngine *aspectEngine)
{
    Q_UNUSED(aspectEngine);

    for (auto result: m_results) {
        // set the results
        QBoundingVolumePrivate::get(result.provider)->setImplicitBounds(result.m_min, result.m_max, result.m_center, result.m_radius);

        // reset dirty flags
        QEntityPrivate::get(result.entity)->m_dirty = false;
        QGeometryViewPrivate::get(result.provider->view())->m_dirty = false;
        QGeometryPrivate::get(result.provider->view()->geometry())->m_dirty = false;
        QAttributePrivate::get(result.positionAttribute)->m_dirty = false;
        QBufferPrivate::get(result.positionAttribute->buffer())->m_dirty = false;
        if (result.indexAttribute) {
            QAttributePrivate::get(result.indexAttribute)->m_dirty = false;
            QBufferPrivate::get(result.indexAttribute->buffer())->m_dirty = false;
        }
    }

    m_results.clear();
}

void CalculateBoundingVolumeJob::addWatcher(QWeakPointer<BoundingVolumeJobProcessor> watcher)
{
    m_watchers.push_back(watcher);
}

void CalculateBoundingVolumeJob::removeWatcher(QWeakPointer<BoundingVolumeJobProcessor> watcher)
{
    if (watcher.isNull()) {
        m_watchers.erase(std::remove_if(m_watchers.begin(), m_watchers.end(), [](const QWeakPointer<BoundingVolumeJobProcessor> &w) { return w.isNull(); }),
                         m_watchers.end());
    } else {
        m_watchers.erase(std::remove(m_watchers.begin(), m_watchers.end(), watcher),
                         m_watchers.end());
    }
}

} // namespace Qt3DCore

QT_END_NAMESPACE

