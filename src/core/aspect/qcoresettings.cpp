// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcoresettings.h"
#include "qcoresettings_p.h"
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

/*!
   \class Qt3DCore::QCoreSettings
   \brief The QCoreSettings class holds settings related to core data handling process.
   \since 6.0
   \inmodule Qt3DCore
   \inherits Qt3DCore::QComponent

    The QCoreSettings component should be set as a component of the scene root entity
    (although it could be anywhere in the scene graph). There should be a single instance.

    It can be used to control some of Qt 3D's behavior.
*/

/*!
   \qmltype CoreSettings
   \brief The CoreSettings class holds settings related to core data handling process.
   \since 6.0
   \inqmlmodule Qt3D.Core
   \nativetype Qt3DCore::QCoreSettings

    The CoreSettings component should be set as a component of the scene root entity
    (although it could be anywhere in the scene graph). There should be a single instance.

    It can be used to control some of Qt 3D's behavior.
*/

namespace Qt3DCore {

QCoreSettingsPrivate::QCoreSettingsPrivate()
    : QComponentPrivate()
    , m_boundingVolumesEnabled(true)
{
}

QCoreSettingsPrivate *QCoreSettingsPrivate::get(QCoreSettings *q)
{
    return q->d_func();
}

/*!
 * Constructs a new QCoreSettings with \a parent.
 */
QCoreSettings::QCoreSettings(QNode *parent)
    : QComponent(*new QCoreSettingsPrivate(), parent)
{
}

/*!
 * \internal
 */
QCoreSettings::~QCoreSettings()
{
}

/*!
\qmlproperty bool CoreSettings::boundingVolumesEnabled

Holds whether bounding volumes handling is enabled. This is true by
default. Disabling this allows to reduce the amount of computations
performed each frame. If you are using picking or frustum culling you
should keep this enabled (even when providing explicit bounding volumes
sizes using BoundingVolume).
*/
/*!
\property Qt3DCore::QCoreSettings::boundingVolumesEnabled

Holds whether bounding volumes handling is enabled. This is true by
default. Disabling this allows to reduce the amount of computations
performed each frame. If you are using picking or frustum culling you
should keep this enabled (even when providing explicit bounding volumes
sizes using QBoundingVolume).
*/
bool QCoreSettings::boundingVolumesEnabled() const
{
    Q_D(const QCoreSettings);
    return d->m_boundingVolumesEnabled;
}

void QCoreSettings::setBoundingVolumesEnabled(bool boundingVolumesEnabled)
{
    Q_D(QCoreSettings);
    if (d->m_boundingVolumesEnabled == boundingVolumesEnabled)
        return;
    d->m_boundingVolumesEnabled = boundingVolumesEnabled;
    emit boundingVolumesEnabledChanged(boundingVolumesEnabled);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qcoresettings.cpp"
