// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcoreaspect.h"
#include "qcoreaspect_p.h"
#include <Qt3DCore/qcoresettings.h>
#include <Qt3DCore/private/coresettings_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QCoreAspectPrivate::QCoreAspectPrivate()
    : Qt3DCore::QAbstractAspectPrivate()
    , m_boundingVolumesEnabled(true)
{

}

QCoreAspectPrivate::~QCoreAspectPrivate()
{

}

QCoreAspectPrivate *QCoreAspectPrivate::get(QCoreAspect *aspect)
{
    return aspect->d_func();
}

void QCoreAspectPrivate::jobsDone()
{

}

void QCoreAspectPrivate::frameDone()
{

}

QCoreAspect::QCoreAspect(QObject *parent)
    : Qt3DCore::QAbstractAspect(*new QCoreAspectPrivate, parent)
{

}

QCoreAspect::~QCoreAspect()
{

}

QAspectJobPtr QCoreAspect::calculateBoundingVolumeJob() const
{
    Q_D(const QCoreAspect);
    return d->m_calculateBoundingVolumeJob;
}

std::vector<QAspectJobPtr> QCoreAspect::jobsToExecute(qint64 time)
{
    Q_D(QCoreAspect);
    Q_UNUSED(time);

    std::vector<QAspectJobPtr> jobs;

    auto scene = d->m_aspectManager->scene();
    auto dirtyBits = scene->dirtyBits();

    if (d->m_boundingVolumesEnabled) {
        if (dirtyBits & QScene::GeometryDirty ||
            dirtyBits & QScene::BuffersDirty ||
            dirtyBits & QScene::EntityEnabledDirty) {
            jobs.push_back(d->m_calculateBoundingVolumeJob);
        }
    }

    return jobs;
}

QVariant QCoreAspect::executeCommand(const QStringList &args)
{
    Q_UNUSED(args);
    return {};
}

void QCoreAspect::onRegistered()
{
    Q_D(QCoreAspect);

    if (d->m_calculateBoundingVolumeJob.isNull())
        d->m_calculateBoundingVolumeJob = CalculateBoundingVolumeJobPtr::create(this);

    registerBackendType<QCoreSettings>(QSharedPointer<CoreSettingsFunctor>::create(this));
}

void QCoreAspect::onUnregistered()
{
    unregisterBackendType<Qt3DCore::QCoreSettings>();
}

void QCoreAspect::onEngineStartup()
{
    Q_D(QCoreAspect);

    Q_ASSERT(d->m_calculateBoundingVolumeJob);
    d->m_calculateBoundingVolumeJob->setRoot(d->m_root);
}

void QCoreAspect::frameDone()
{
    Q_D(QCoreAspect);
    auto scene = d->m_aspectManager->scene();
    scene->clearDirtyBits();
}

} // namespace Qt3DCore

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("core", QT_PREPEND_NAMESPACE(Qt3DCore), QCoreAspect)

#include "moc_qcoreaspect.cpp"
