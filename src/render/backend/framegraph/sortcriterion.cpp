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

#include "sortcriterion_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

SortCriterion::SortCriterion()
    : QBackendNode()
    , m_type(QSortCriterion::StateChangeCost)
{
}

void SortCriterion::cleanup()
{
}

void SortCriterion::updateFromPeer(QNode *peer)
{
    QSortCriterion *criterion = static_cast<QSortCriterion *>(peer);
    m_type = criterion->sort();
}

QSortCriterion::SortType SortCriterion::sortType() const
{
    return m_type;
}

void SortCriterion::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeUpdated && propertyChange->propertyName() == QByteArrayLiteral("sort")) {
        m_type = static_cast<QSortCriterion::SortType>(propertyChange->value().toInt());
    }
}

}

} // Qt3D

QT_END_NAMESPACE
