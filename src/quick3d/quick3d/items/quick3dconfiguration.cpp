/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "quick3dconfiguration.h"
#include <QDebug>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnode_p.h>

#include <Qt3DInput/QInputAspect>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

/*!
    \qmltype Configuration
    \inqmlmodule Qt3D
    \since 5.5
*/
Quick3DConfiguration::Quick3DConfiguration(QObject *parent)
    : QObject(parent)
    , m_camera(Q_NULLPTR)
{
}

void Quick3DConfiguration::setControlledCamera(QCamera *camera)
{
    if (m_camera != camera) {
        m_camera = camera;
        emit controlledCameraChanged();
        QMetaObject::invokeMethod(this, "applyControlledCameraChange", Qt::QueuedConnection);
    }
}

/*!
    \qmlproperty Camera Qt3D::Configuration::controlledCamera
*/
QCamera *Quick3DConfiguration::controlledCamera() const
{
    return m_camera;
}

void Quick3DConfiguration::applyControlledCameraChange()
{
    QScene *scene = QNodePrivate::get(m_camera)->scene();

    // Too early, let's try again later
    if (!scene) {
        QMetaObject::invokeMethod(this, "applyControlledCameraChange", Qt::QueuedConnection);
        return;
    }

    Q_FOREACH (QAbstractAspect *aspect, scene->engine()->aspects()) {
        if (aspect->property("camera").isValid()) {
            aspect->setProperty("camera", QVariant::fromValue(m_camera));
            break;
        }
    }
}

} // Quick

} // Qt3D

QT_END_NAMESPACE
