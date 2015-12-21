/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qinputdeviceintegration.h"
#include "qinputdeviceintegration_p.h"

#include <Qt3DInput/QInputAspect>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QInputDeviceIntegrationPrivate::QInputDeviceIntegrationPrivate()
    : QObjectPrivate()
    , m_aspect(Q_NULLPTR)
{
}

QInputDeviceIntegration::QInputDeviceIntegration(QObject *parent)
    : QObject(*new QInputDeviceIntegrationPrivate, parent)
{
}

QInputDeviceIntegration::QInputDeviceIntegration(QInputDeviceIntegrationPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

void QInputDeviceIntegration::registerBackendType(const QMetaObject &metaObject, const Qt3DCore::QBackendNodeFunctorPtr &functor)
{
    Q_D(QInputDeviceIntegration);
    d->m_aspect->registerBackendType(metaObject, functor);
}

void QInputDeviceIntegration::initialize(QInputAspect *aspect)
{
    Q_D(QInputDeviceIntegration);
    d->m_aspect = aspect;
    onInitialize();
}

QInputAspect *QInputDeviceIntegration::inputAspect() const
{
    Q_D(const QInputDeviceIntegration);
    return d->m_aspect;
}

} // namespace Qt3DInput

QT_END_NAMESPACE
