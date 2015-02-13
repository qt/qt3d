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

#include "loadscenejob_p.h"
#include <private/renderer_p.h>
#include <private/scenemanager_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRenderer/private/abstractsceneparser_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

LoadSceneJob::LoadSceneJob(const QUrl &source, const QNodeId &m_sceneComponent)
    : QAspectJob()
    , m_renderer(Q_NULLPTR)
    , m_source(source)
    , m_sceneComponent(m_sceneComponent)
{
}

void LoadSceneJob::run()
{
    QEntity *sceneTree = m_renderer->sceneManager()->sceneTreeFromSource(m_source);
    if (sceneTree == Q_NULLPTR) {
        Q_FOREACH (AbstractSceneParser *parser, m_renderer->sceneParsers()) {
            if (parser->isExtensionSupported(m_source)) {
                parser->setSource(m_source);
                sceneTree = parser->scene();
                m_renderer->sceneManager()->addLoadedSceneTree(m_source, sceneTree);
            }
        }
    }
    // set clone of sceneTree in sceneComponent
    RenderScene *scene = m_renderer->sceneManager()->lookupResource(m_sceneComponent);
    scene->setSceneSubtree(sceneTree);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
