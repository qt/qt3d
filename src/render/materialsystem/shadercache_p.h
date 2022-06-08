// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_SHADERCACHE_H
#define QT3DRENDER_RENDER_SHADERCACHE_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/shader_p.h>

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3DRender {
namespace Render {

#if defined(QT_BUILD_INTERNAL)
class tst_ShaderCache;
#endif

class Q_3DRENDERSHARED_PRIVATE_EXPORT ShaderCache
{
public:
    ~ShaderCache();

    QOpenGLShaderProgram *getShaderProgramAndAddRef(ProgramDNA dna, Qt3DCore::QNodeId shaderPeerId, bool *wasPresent = nullptr);
    void insert(ProgramDNA dna, Qt3DCore::QNodeId shaderPeerId, QOpenGLShaderProgram *program);
    void removeRef(ProgramDNA dna, Qt3DCore::QNodeId shaderPeerId);
    void purge();
    void clear();

    // Only ever used from the OpenGL submission thread
    QOpenGLShaderProgram *getShaderProgramForDNA(ProgramDNA dna) const;
    QList<Qt3DCore::QNodeId> shaderIdsForProgram(ProgramDNA dna) const;

private:
    // Only ever used from the OpenGL submission thread
    QHash<ProgramDNA, QOpenGLShaderProgram *> m_programHash;

    // Accessed from both the OpenGL submission thread and the aspect thread
    QHash<ProgramDNA, QList<Qt3DCore::QNodeId>> m_programRefs;
    QList<ProgramDNA> m_pendingRemoval;
    QMutex m_refsMutex;

#if defined(QT_BUILD_INTERNAL)
    friend class tst_ShaderCache;
#endif
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SHADERCACHE_H
