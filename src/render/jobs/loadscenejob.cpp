/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "loadscenejob_p.h"
#include <private/renderer_p.h>
#include <private/nodemanagers_p.h>
#include <private/scenemanager_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qabstractsceneparser.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

LoadSceneJob::LoadSceneJob(const QUrl &source, Qt3DCore::QNodeId m_sceneComponent)
    : QAspectJob()
    , m_source(source)
    , m_sceneComponent(m_sceneComponent)
    , m_managers(Q_NULLPTR)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadScene, 0);
}

void LoadSceneJob::run()
{
    Qt3DCore::QEntity *sceneTree = m_managers->sceneManager()->sceneTreeFromSource(m_source);
    if (sceneTree == Q_NULLPTR) {
        Q_FOREACH (QAbstractSceneParser *parser, m_parsers) {
            if (parser->isFileTypeSupported(m_source)) {
                parser->setSource(m_source);
                sceneTree = parser->scene();
                m_managers->sceneManager()->addLoadedSceneTree(m_source, sceneTree);
            }
        }
    }
    // set clone of sceneTree in sceneComponent
    Scene *scene = m_managers->sceneManager()->lookupResource(m_sceneComponent);
    scene->setSceneSubtree(sceneTree);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
