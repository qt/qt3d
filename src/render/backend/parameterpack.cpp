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

#include "parameterpack_p.h"
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/private/renderer_p.h>

#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

ParameterPack::ParameterPack()
    : m_renderer(Q_NULLPTR)
{
}

ParameterPack::~ParameterPack()
{
    if (m_renderer != Q_NULLPTR && !m_peers.empty()) {
//        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
//        Q_FOREACH (const QUuid peer, m_peers)
//            arbiter->unregisterObserver(this, peer);
    }
}

void ParameterPack::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void ParameterPack::appendParameter(QParameter *param)
{
    if (!m_peers.contains(param->uuid())) {
        m_peers << param->uuid();
//        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
//        arbiter->registerObserver(this, param->uuid(), NodeUpdated);
        m_namedValues[param->name()] = param->value();
    }
}

void ParameterPack::removeParameter(QParameter *param)
{
    if (m_peers.contains(param->uuid())) {
        m_peers.removeOne(param->uuid());
        m_namedValues.remove(param->name());
//        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
//        arbiter->unregisterObserver(this, param->uuid());
    }
}

void ParameterPack::clear()
{
    m_namedValues.clear();
//    QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
//    Q_FOREACH (const QUuid &peerId, m_peers) {
//        arbiter->unregisterObserver(this, peerId);
//    }
}

void ParameterPack::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QString propertyName = QString::fromLatin1(propertyChange->propertyName());
        QVariant propertyValue = propertyChange->value();
        if (m_namedValues.contains(propertyName))
            m_namedValues[propertyName] = propertyValue;
    }
}

const QHash<QString, QVariant> ParameterPack::namedValues() const
{
    return m_namedValues;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
