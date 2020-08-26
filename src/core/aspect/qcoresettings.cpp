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

#include "qcoresettings.h"
#include "qcoresettings_p.h"
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

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
   \instantiates Qt3DCore::QCoreSettings

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
\property QCoreSettings::boundingVolumesEnabled

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
