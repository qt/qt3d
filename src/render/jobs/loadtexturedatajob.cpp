/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "loadtexturedatajob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/qtextureimagedata_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

LoadTextureDataJob::LoadTextureDataJob()
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadTextureData, 0);
}

LoadTextureDataJob::~LoadTextureDataJob()
{
}

void LoadTextureDataJob::run()
{
    // NOTE: This must run after Renderer::updateGLResources(),
    // because that is where pendingGenerators is populated.
    // We are therefore not able to create one job for each texture
    // before we add the ability for running jobs (like this) to
    // spawn new jobs.
    const QVector<QTextureImageDataGeneratorPtr> pendingImgGen = m_manager->textureImageDataManager()->pendingGenerators();
    for (const QTextureImageDataGeneratorPtr &imgGen : pendingImgGen) {
        QTextureImageDataPtr imgData = (*imgGen)();
        m_manager->textureImageDataManager()->assignData(imgGen, imgData);
    }
    const QVector<QTextureGeneratorPtr> pendingTexGen = m_manager->textureDataManager()->pendingGenerators();
    for (const QTextureGeneratorPtr &texGen : pendingTexGen) {
        QTextureDataPtr texData = (*texGen)();
        m_manager->textureDataManager()->assignData(texGen, texData);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
