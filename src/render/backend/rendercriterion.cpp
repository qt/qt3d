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

#include "rendercriterion.h"
#include "rendereraspect.h"
#include "renderer.h"
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderCriterion::RenderCriterion()
    : m_renderer(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
    , m_type(QTechniqueCriterion::Unknown)
{
}

RenderCriterion::~RenderCriterion()
{
    cleanup();
}

void RenderCriterion::cleanup()
{
    if (m_renderer != Q_NULLPTR && m_peer != Q_NULLPTR)
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_peer);
}

void RenderCriterion::setPeer(QTechniqueCriterion *criterion)
{
    if (m_peer != criterion) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_criterionUuid = QUuid();
        m_peer = criterion;
        if (m_peer) {
            arbiter->registerObserver(this, m_peer, ComponentUpdated);
            m_criterionUuid = criterion->uuid();
            m_type = m_peer->criterionType();
            m_value = m_peer->criterionValue();
            m_customType = m_peer->criterionCustomType();
        }
    }
}

void RenderCriterion::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

QVariant RenderCriterion::criterionValue() const
{
    return m_value;
}

QTechniqueCriterion::CriterionType RenderCriterion::criterionType() const
{
    return m_type;
}

QString RenderCriterion::customType() const
{
    return m_customType;
}

void RenderCriterion::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (propertyChange->propertyName() == QByteArrayLiteral("criterionType"))
        m_type = static_cast<QTechniqueCriterion::CriterionType>(propertyChange->value().toInt());
    else if (propertyChange->propertyName() == QByteArrayLiteral("criterionValue"))
        m_value = propertyChange->value();
    else if (propertyChange->propertyName() == QByteArrayLiteral("criterionCustomType"))
        m_customType = propertyChange->value().toString();
}

QUuid RenderCriterion::criterionUuid() const
{
    return m_criterionUuid;
}

bool RenderCriterion::operator ==(const RenderCriterion &other)
{
    if (&other == this)
        return true;
    if (other.criterionType() == criterionType()) {
        if ((criterionType() != QTechniqueCriterion::CustomType) || (other.criterionType() == QTechniqueCriterion::CustomType && other.customType() == customType()))
            return (other.criterionValue() == criterionValue());
    }
    return false;
}

bool RenderCriterion::operator !=(const RenderCriterion &other)
{
    return !operator ==(other);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
