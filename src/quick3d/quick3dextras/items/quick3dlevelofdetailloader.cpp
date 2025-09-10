// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3dlevelofdetailloader_p_p.h"
#include <Qt3DRender/qlevelofdetailboundingsphere.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DQuick/private/quick3dentityloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {
namespace Extras {
namespace Quick {

Quick3DLevelOfDetailLoaderPrivate::Quick3DLevelOfDetailLoaderPrivate()
    : QEntityPrivate()
    , m_loader(new Qt3DCore::Quick::Quick3DEntityLoader)
    , m_lod(new Qt3DRender::QLevelOfDetail)
{
}

Quick3DLevelOfDetailLoader::Quick3DLevelOfDetailLoader(QNode *parent)
    : QEntity(*new Quick3DLevelOfDetailLoaderPrivate, parent)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_loader->setParent(this);
    d->m_loader->addComponent(d->m_lod);

    connect(d->m_lod, &Qt3DRender::QLevelOfDetail::cameraChanged,
            this, &Quick3DLevelOfDetailLoader::cameraChanged);
    connect(d->m_lod, &Qt3DRender::QLevelOfDetail::currentIndexChanged,
            this, &Quick3DLevelOfDetailLoader::currentIndexChanged);
    connect(d->m_lod, &Qt3DRender::QLevelOfDetail::thresholdTypeChanged,
            this, &Quick3DLevelOfDetailLoader::thresholdTypeChanged);
    connect(d->m_lod, &Qt3DRender::QLevelOfDetail::thresholdsChanged,
            this, &Quick3DLevelOfDetailLoader::thresholdsChanged);
    connect(d->m_lod, &Qt3DRender::QLevelOfDetail::volumeOverrideChanged,
            this, &Quick3DLevelOfDetailLoader::volumeOverrideChanged);
    connect(d->m_loader, &Qt3DCore::Quick::Quick3DEntityLoader::entityChanged,
            this, &Quick3DLevelOfDetailLoader::entityChanged);
    connect(d->m_loader, &Qt3DCore::Quick::Quick3DEntityLoader::sourceChanged,
            this, &Quick3DLevelOfDetailLoader::sourceChanged);

    connect(this, &Quick3DLevelOfDetailLoader::enabledChanged,
            d->m_lod, &Qt3DRender::QLevelOfDetail::setEnabled);

    auto applyCurrentSource = [this] {
        Q_D(Quick3DLevelOfDetailLoader);
        const auto index = currentIndex();
        if (index >= 0 && index < d->m_sources.size())
            d->m_loader->setSource(d->m_sources.at(index).toUrl());
    };

    connect(this, &Quick3DLevelOfDetailLoader::sourcesChanged,
            this, applyCurrentSource);
    connect(this, &Quick3DLevelOfDetailLoader::currentIndexChanged,
            this, applyCurrentSource);
}

QVariantList Quick3DLevelOfDetailLoader::sources() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_sources;
}

void Quick3DLevelOfDetailLoader::setSources(const QVariantList &sources)
{
    Q_D(Quick3DLevelOfDetailLoader);
    if (d->m_sources != sources) {
        d->m_sources = sources;
        emit sourcesChanged();
    }
}

Qt3DRender::QCamera *Quick3DLevelOfDetailLoader::camera() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_lod->camera();
}

void Quick3DLevelOfDetailLoader::setCamera(Qt3DRender::QCamera *camera)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_lod->setCamera(camera);
}

int Quick3DLevelOfDetailLoader::currentIndex() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_lod->currentIndex();
}

void Quick3DLevelOfDetailLoader::setCurrentIndex(int currentIndex)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_lod->setCurrentIndex(currentIndex);
}

Qt3DRender::QLevelOfDetail::ThresholdType Quick3DLevelOfDetailLoader::thresholdType() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_lod->thresholdType();
}

void Quick3DLevelOfDetailLoader::setThresholdType(Qt3DRender::QLevelOfDetail::ThresholdType thresholdType)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_lod->setThresholdType(thresholdType);
}

QList<qreal> Quick3DLevelOfDetailLoader::thresholds() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_lod->thresholds();
}

void Quick3DLevelOfDetailLoader::setThresholds(const QList<qreal> &thresholds)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_lod->setThresholds(thresholds);
}

Qt3DRender::QLevelOfDetailBoundingSphere Quick3DLevelOfDetailLoader::createBoundingSphere(const QVector3D &center, float radius)
{
    return Qt3DRender::QLevelOfDetailBoundingSphere(center, radius);
}

Qt3DRender::QLevelOfDetailBoundingSphere Quick3DLevelOfDetailLoader::volumeOverride() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_lod->volumeOverride();
}

void Quick3DLevelOfDetailLoader::setVolumeOverride(const Qt3DRender::QLevelOfDetailBoundingSphere &volumeOverride)
{
    Q_D(Quick3DLevelOfDetailLoader);
    d->m_lod->setVolumeOverride(volumeOverride);
}

QObject *Quick3DLevelOfDetailLoader::entity() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_loader->entity();
}

QUrl Quick3DLevelOfDetailLoader::source() const
{
    Q_D(const Quick3DLevelOfDetailLoader);
    return d->m_loader->source();
}

} // namespace Quick
} // namespace Extras
} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_quick3dlevelofdetailloader_p.cpp"


