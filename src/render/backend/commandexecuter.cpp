/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
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

#ifdef QT3D_JOBS_RUN_STATS

#include "commandexecuter_p.h"
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DRender/private/graphicscontext_p.h>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Debug {

CommandExecuter::CommandExecuter(Render::Renderer *renderer)
    : m_renderer(renderer)
{
}

// Render thread
void CommandExecuter::performAsynchronousCommandExecution()
{
    QMutexLocker lock(m_renderer->mutex());
    const QVector<Qt3DCore::Debug::AsynchronousCommandReply *> shellCommands = std::move(m_pendingCommands);
    lock.unlock();

    for (auto *reply : shellCommands) {
        if (reply->commandName() == QLatin1String("glinfo")) {
            QJsonObject replyObj;
            const GraphicsApiFilterData *contextInfo = m_renderer->m_graphicsContext->contextInfo();
            if (contextInfo != nullptr) {
                replyObj.insert(QLatin1String("api"),
                                contextInfo->m_api == QGraphicsApiFilter::OpenGL
                                ? QLatin1String("OpenGL")
                                : QLatin1String("OpenGLES"));
                const QString versionString =
                        QString::number(contextInfo->m_major)
                        + QStringLiteral(".")
                        + QString::number(contextInfo->m_minor);
                replyObj.insert(QLatin1String("version"), versionString);
                replyObj.insert(QLatin1String("profile"),
                                contextInfo->m_profile == QGraphicsApiFilter::CoreProfile
                                ? QLatin1String("Core")
                                : contextInfo->m_profile == QGraphicsApiFilter::CompatibilityProfile
                                  ? QLatin1String("Compatibility")
                                  : QLatin1String("None"));
            }
            reply->setData(QJsonDocument(replyObj).toJson());
        }
        reply->setFinished(true);
    }
}

// Main thread
QVariant CommandExecuter::executeCommand(const QStringList &args)
{
    // Note: The replies will be deleted by the AspectCommandDebugger
    if (args.length() > 0 && args.first() == QLatin1String("glinfo")) {
        auto reply = new Qt3DCore::Debug::AsynchronousCommandReply(args.first());
        QMutexLocker lock(m_renderer->mutex());
        m_pendingCommands.push_back(reply);
        return QVariant::fromValue(reply);
    }
}

} // Debug

} // Qt3DRenderer

QT_END_NAMESPACE

#endif
