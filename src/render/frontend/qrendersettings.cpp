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

#include "qrendersettings.h"
#include "qrendersettings_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QRenderSettingsPrivate::QRenderSettingsPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_pickMethod(QRenderSettings::BoundingVolumePicking)
    , m_pickResultMode(QRenderSettings::NearestPick)
{
}

QRenderSettings::QRenderSettings(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QRenderSettingsPrivate, parent)
{
}

QRenderSettings::QRenderSettings(QRenderSettingsPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

QRenderSettings::~QRenderSettings()
{
    QNode::cleanup();
}

QRenderSettings::PickMethod QRenderSettings::pickMethod() const
{
    Q_D(const QRenderSettings);
    return d->m_pickMethod;
}

QRenderSettings::PickResultMode QRenderSettings::pickResultMode() const
{
    Q_D(const QRenderSettings);
    return d->m_pickResultMode;
}

void QRenderSettings::setPickMethod(QRenderSettings::PickMethod pickMethod)
{
    Q_D(QRenderSettings);
    if (d->m_pickMethod == pickMethod)
        return;

    d->m_pickMethod = pickMethod;
    emit pickMethodChanged(pickMethod);
}

void QRenderSettings::setPickResultMode(QRenderSettings::PickResultMode pickResultMode)
{
    Q_D(QRenderSettings);
    if (d->m_pickResultMode == pickResultMode)
        return;

    d->m_pickResultMode = pickResultMode;
    emit pickResultModeChanged(pickResultMode);
}

void QRenderSettings::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QRenderSettings *object = static_cast<const QRenderSettings *>(ref);
    d_func()->m_pickMethod = object->d_func()->m_pickMethod;
    d_func()->m_pickResultMode = object->d_func()->m_pickResultMode;
}

} // namespace Qt3Drender

QT_END_NAMESPACE
