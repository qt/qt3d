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

#include "parameterpack.h"
#include "parameter.h"
#include "rendereraspect.h"
#include "renderer.h"

#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

ParameterPack::ParameterPack()
    : m_renderer(Q_NULLPTR)
{
}

void ParameterPack::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void ParameterPack::appendParameter(Parameter *param)
{
    if (!m_peers.contains(param)) {
        m_peers << param;
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        // Parameter needs to be a node
//        arbiter->registerObserver(this, );
        m_namedValues[param->name()] = param->value();
    }
}

void ParameterPack::removeParameter(Parameter *param)
{
    if (m_peers.contains(param)) {
        m_peers.removeOne(param);
        m_namedValues.remove(param->name());
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        // Parameter needs to be a node
//        arbiter->unregisterObserver(this, );
    }
}

void ParameterPack::clear()
{
    m_namedValues.clear();
    QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
    Q_FOREACH (Parameter *param, m_peers) {
        // Parameter needs to be a node
        //        arbiter->unregisterObserver(this, );
    }
}

void ParameterPack::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->m_type == ComponentUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QString propertyName = QString::fromLatin1(propertyChange->m_propertyName);
        QVariant propertyValue = propertyChange->m_value;
        if (m_namedValues.contains(propertyName))
            m_namedValues[propertyName] = propertyValue;
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
