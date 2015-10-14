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

#include "annotation_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Annotation::Annotation()
    : QBackendNode()
{
}

Annotation::~Annotation()
{
    cleanup();
}

void Annotation::cleanup()
{
}

void Annotation::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAnnotation *criterion = static_cast<QAnnotation *>(peer);

    m_value = criterion->value();
    m_name = criterion->name();
}

QVariant Annotation::criterionValue() const
{
    return m_value;
}

QString Annotation::criterionName() const
{
    return m_name;
}

void Annotation::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (propertyChange->propertyName() == QByteArrayLiteral("value"))
        m_value = propertyChange->value();
    else if (propertyChange->propertyName() == QByteArrayLiteral("name"))
        m_name = propertyChange->value().toString();
}

bool Annotation::operator ==(const Annotation &other)
{
    if (&other == this)
        return true;
    return ((other.criterionName() == criterionName()) &&
            (other.criterionValue() == criterionValue()));
}

bool Annotation::operator !=(const Annotation &other)
{
    return !operator ==(other);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
