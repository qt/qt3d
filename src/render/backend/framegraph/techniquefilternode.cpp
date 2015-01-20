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

#include "techniquefilternode_p.h"
#include "qannotation.h"
#include "qtechniquefilter.h"
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

TechniqueFilter::TechniqueFilter()
    : FrameGraphNode(FrameGraphNode::TechniqueFilter)
{
}

void TechniqueFilter::updateFromPeer(QNode *peer)
{
    QTechniqueFilter *filter = static_cast<QTechniqueFilter *>(peer);
    m_filters.clear();
    Q_FOREACH (QAnnotation *criterion, filter->criteria())
        appendFilter(criterion);
    setEnabled(filter->isEnabled());
}

QList<QNodeId> TechniqueFilter::filters() const
{
    return m_filters;
}

void TechniqueFilter::appendFilter(QAnnotation *criterion)
{
    if (!m_filters.contains(criterion->id()))
        m_filters.append(criterion->id());
}

void TechniqueFilter::removeFilter(const QNodeId &criterionId)
{
    m_filters.removeOne(criterionId);
}

void TechniqueFilter::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
    case NodeUpdated: {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            setEnabled(propertyChange->value().toBool());
    }
        break;

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("require"))
            appendFilter(propertyChange->value().value<QAnnotation *>());
    }
        break;
    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("require"))
            removeFilter(propertyChange->value().value<QNodeId>());
    }
        break;
    default:
        break;
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
