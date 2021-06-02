/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcoreaspect.h"
#include "qcoreaspect_p.h"
#include <Qt3DCore/qcoresettings.h>
#include <Qt3DCore/private/coresettings_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

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

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("core", QT_PREPEND_NAMESPACE(Qt3DCore), QCoreAspect)
