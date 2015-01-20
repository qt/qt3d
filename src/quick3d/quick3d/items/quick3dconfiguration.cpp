/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quick3dconfiguration.h"
#include <QDebug>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QScene>
#include <Qt3DCore/private/qnode_p.h>

#include <Qt3DInput/QInputAspect>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

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

QCamera *Quick3DConfiguration::controlledCamera() const
{
    return m_camera;
}

void Quick3DConfiguration::applyControlledCameraChange()
{
    QScene *scene = static_cast<QScene*>(QNodePrivate::get(m_camera)->scene());

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
