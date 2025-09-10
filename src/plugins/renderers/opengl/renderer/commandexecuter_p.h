// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_DEBUG_COMMANDEXECUTER_H
#define QT3DRENDER_DEBUG_COMMANDEXECUTER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QList>
#include <QMutex>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace Debug {
class AsynchronousCommandReply;
} // Debug

} // Qt3DCore

namespace Qt3DRender {

namespace Render {
namespace OpenGL {
class Renderer;
class RenderView;
} // OpenGL
} // Render

namespace Debug {

class CommandExecuter
{
public:
    explicit CommandExecuter(Render::OpenGL::Renderer *renderer);

    void performAsynchronousCommandExecution(const std::vector<Render::OpenGL::RenderView *> &views);

    QVariant executeCommand(const QStringList &args);

private:
    Render::OpenGL::Renderer *m_renderer;
    QList<Qt3DCore::Debug::AsynchronousCommandReply *> m_pendingCommands;
    QMutex m_pendingCommandsMutex;
};

} // Debug

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_DEBUG_COMMANDEXECUTER_H
