// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_APISHADERMANAGER_H
#define QT3DRENDER_RENDER_APISHADERMANAGER_H


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

#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/shader_p.h>
#include <QtCore/QReadLocker>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Shader;

template<class APIShader>
class APIShaderManager
{
public:
    explicit APIShaderManager()
    {
    }

    ~APIShaderManager()
    {
    }

    std::vector<APIShader *> takeActiveResources()
    {
        QWriteLocker lock(&m_readWriteLock);

        std::vector<APIShader *> shaders;
        shaders.reserve(m_abandonedShaders.size() + m_apiShaders.size());

        const QList<APIShader *> tmp = m_apiShaders.keys();
        std::copy(tmp.cbegin(), tmp.cend(), std::back_insert_iterator(shaders));
        shaders.insert(shaders.end(),
                     m_abandonedShaders.begin(),
                     m_abandonedShaders.end());

        m_apiShaders.clear();
        m_abandonedShaders.clear();
        m_nodeIdToAPIShader.clear();

        return shaders;
    }

    APIShader *lookupResource(Qt3DCore::QNodeId shaderId)
    {
        QReadLocker lock(&m_readWriteLock);
        return m_nodeIdToAPIShader.value(shaderId);
    }

    // Note: automatically adopts the Shader if it needs to be created
    APIShader *createOrAdoptExisting(const Shader *shader)
    {
        // Hold the write lock for the whole lookup + adopt/create sequence so
        // that the containers can't change between finding a match and adopting it
        QWriteLocker lock(&m_readWriteLock);

        // Try to find if an APIShader that matches shader
        // already exists
        for (auto it = m_apiShaders.cbegin(), end = m_apiShaders.cend(); it != end; ++it) {
            if (isSameShader(it.key(), shader)) {
                APIShader *apiShader = it.key();
                adoptUnlocked(apiShader, shader);
                return apiShader;
            }
        }

        // Try to find if one of the scheduled for deletion APIShader
        // could be reused
        for (auto it = m_abandonedShaders.begin(), end = m_abandonedShaders.end(); it != end; ++it) {
            if (isSameShader(*it, shader)) {
                APIShader *apiShader = *it;
                // Remove from list of shaders scheduled for release
                m_abandonedShaders.erase(it);
                adoptUnlocked(apiShader, shader);
                return apiShader;
            }
        }

        // If not create one
        APIShader *apiShader = create();
        adoptUnlocked(apiShader, shader);
        return apiShader;
    }

    void abandon(APIShader *apiShader, const Shader *shader)
    {
        QWriteLocker lock(&m_readWriteLock);
        APIShader *storedApiShader = m_nodeIdToAPIShader.take(shader->peerId());
        Q_ASSERT(apiShader != nullptr && apiShader == storedApiShader);

        std::vector<Qt3DCore::QNodeId> &referencedShaderNodes = m_apiShaders[apiShader];
        referencedShaderNodes.erase(std::remove(referencedShaderNodes.begin(),
                                                referencedShaderNodes.end(),
                                                shader->peerId()),
                                    referencedShaderNodes.end());

        if (referencedShaderNodes.empty()) {
            m_abandonedShaders.push_back(apiShader);
            m_apiShaders.remove(apiShader);
        }
    }

    std::vector<APIShader *> takeAbandonned()
    {
        QWriteLocker lock(&m_readWriteLock);
        return Qt3DCore::moveAndClear(m_abandonedShaders);
    }

    std::vector<Qt3DCore::QNodeId> shaderIdsForProgram(APIShader *glShader) const
    {
        QReadLocker lock(&m_readWriteLock);
        return m_apiShaders.value(glShader);
    }

    void purge()
    {
        qDeleteAll(takeAbandonned());
    }

private:

    // Caller must hold m_readWriteLock for writing
    void adoptUnlocked(APIShader *apiShader, const Shader *shader)
    {
        auto &shaderIds = m_apiShaders[apiShader];

        if (!Qt3DCore::contains(shaderIds, shader->peerId())) {
            shaderIds.push_back(shader->peerId());
            m_nodeIdToAPIShader.insert(shader->peerId(), apiShader);
        }
    }

    bool isSameShader(const APIShader *apiShader, const Shader *shaderNode)
    {
        const std::vector<QByteArray> &nodeShaderCode = shaderNode->shaderCode();
        const std::vector<QByteArray> &apiShaderCode = apiShader->shaderCode();

        const size_t s = nodeShaderCode.size();

        Q_ASSERT(s == apiShaderCode.size());

        for (size_t i = 0; i < s; ++i)
            if (nodeShaderCode[i] != apiShaderCode[i])
                return false;

        return true;
    }

    APIShader *create()
    {
        APIShader *apiShader = new APIShader();
        return apiShader;
    }


    QHash<Qt3DCore::QNodeId, APIShader *> m_nodeIdToAPIShader;
    QHash<APIShader *, std::vector<Qt3DCore::QNodeId>> m_apiShaders;

    std::vector<APIShader *> m_abandonedShaders;

    mutable QReadWriteLock m_readWriteLock;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_APISHADERMANAGER_H
