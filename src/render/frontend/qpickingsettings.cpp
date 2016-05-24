/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qpickingsettings.h"
#include "qpickingsettings_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QPickingSettings
 * \brief The QPickingSettings class specifies the current framegraph to be used
 * by the renderer and specifies whether rendering is always active or updated
 * only on changes.
 * \since 5.7
 * \inmodule Qt3DRender
 */

QPickingSettingsPrivate::QPickingSettingsPrivate()
    : Qt3DCore::QNodePrivate()
    , m_pickMethod(QPickingSettings::BoundingVolumePicking)
    , m_pickResultMode(QPickingSettings::NearestPick)
{
}

/*!
 * The constructor creates a new Qt3DRender::QPickingSettings
 * instance with the specified \a parent.
 * \param parent
 */
QPickingSettings::QPickingSettings(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QPickingSettingsPrivate, parent)
{
}

/*! \internal */
QPickingSettings::~QPickingSettings()
{
}

/*! \internal */
QPickingSettings::QPickingSettings(QPickingSettingsPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

/*!
 * \return the current pick method.
 */
QPickingSettings::PickMethod QPickingSettings::pickMethod() const
{
    Q_D(const QPickingSettings);
    return d->m_pickMethod;
}

/*!
 * \return the current pick result mode.
 */
QPickingSettings::PickResultMode QPickingSettings::pickResultMode() const
{
    Q_D(const QPickingSettings);
    return d->m_pickResultMode;
}

/*!
 * \enum Qt3DRender::QPickingSettings::PickMethod
 *
 * Specifies which parameters of Qt3DRender::QPickingSettings are used.
 *
 * \value BoundingVolumePicking
 * \value TrianglePicking
 */

/*!
 * Sets the pick method to \a pickMethod
 * \param pickMethod
 */
void QPickingSettings::setPickMethod(QPickingSettings::PickMethod pickMethod)
{
    Q_D(QPickingSettings);
    if (d->m_pickMethod == pickMethod)
        return;

    d->m_pickMethod = pickMethod;
    emit pickMethodChanged(pickMethod);
}

/*!
 * \enum Qt3DRender::QPickingSettings::PickResultMode
 *
 * Specifies which parameters of Qt3DRender::PickResultMode are used.
 *
 * \value NearestPick
 * \value AllPicks
 */

/*!
 * Sets the pick result mode to \a pickResultMode
 * \param pickResultMode
 */
void QPickingSettings::setPickResultMode(QPickingSettings::PickResultMode pickResultMode)
{
    Q_D(QPickingSettings);
    if (d->m_pickResultMode == pickResultMode)
        return;

    d->m_pickResultMode = pickResultMode;
    emit pickResultModeChanged(pickResultMode);
}

} // namespace Qt3Drender

QT_END_NAMESPACE
