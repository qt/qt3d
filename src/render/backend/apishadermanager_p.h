/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

    std::vector<APIShader *> takeActiveResources() const
    {
        QReadLocker lock(&m_readWriteLock);

        std::vector<APIShader *> keysV;
        const QList<APIShader *> tmp = m_apiShaders.keys();

        std::copy(tmp.cbegin(), tmp.cend(), std::back_insert_iterator(keysV));
        keysV.insert(keysV.end(),
                     m_abandonedShaders.begin(),
                     m_abandonedShaders.end());

        return keysV;
    }

    APIShader *lookupResource(Qt3DCore::QNodeId shaderId)
    {
        QReadLocker lock(&m_readWriteLock);
        return m_nodeIdToAPIShader.value(shaderId);
    }

    // Note: automatically adopts the Shader if it needs to be created
    APIShader *createOrAdoptExisting(const Shader *shader)
    {
        // Try to find if an APIShader that matches shader
        // already exists

        {
            QReadLocker readLock(&m_readWriteLock);
            {
                const auto end = m_apiShaders.cend();
                for (auto it = m_apiShaders.cbegin(); it != end; ++it)
                    if (isSameShader(it.key(), shader)) {
                        APIShader *apiShader = it.key();
                        // Adopt if needed
                        readLock.unlock();
                        adopt(apiShader, shader);
                        return apiShader;
                    }
            }

            // Try to find if one of the scheduled for deletion APIShader
            // could be reused
            {
                const auto end = m_abandonedShaders.end();
                for (auto it = m_abandonedShaders.begin(); it != end; ++it)
                    if (isSameShader(*it, shader)) {
                        APIShader *apiShader = *it;
                        // Adopt if needed
                        readLock.unlock();
                        // Remove from list of shaders scheduled for relase
                        m_abandonedShaders.erase(it);
                        adopt(apiShader, shader);
                        return apiShader;
                    }
            }
        }

        // If not create one
        APIShader *apiShader = create();
        adopt(apiShader, shader);
        return apiShader;
    }

    // Should never be called from outside code
    // but left public to maintain adopt/abandon symmetry
    void adopt(APIShader *apiShader, const Shader *shader)
    {
        QWriteLocker lock(&m_readWriteLock);
        auto &shaderIds = m_apiShaders[apiShader];

        if (!Qt3DCore::contains(shaderIds, shader->peerId())) {
            shaderIds.push_back(shader->peerId());
            m_nodeIdToAPIShader.insert(shader->peerId(), apiShader);
        }
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

    std::vector<APIShader *> takeUpdated()
    {
        QWriteLocker lock(&m_readWriteLock);
        return Qt3DCore::moveAndClear(m_updatedShaders);
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
        m_updatedShaders.push_back(apiShader);
        return apiShader;
    }


    QHash<Qt3DCore::QNodeId, APIShader *> m_nodeIdToAPIShader;
    QHash<APIShader *, std::vector<Qt3DCore::QNodeId>> m_apiShaders;

    std::vector<APIShader *> m_abandonedShaders;
    std::vector<APIShader *> m_updatedShaders;

    mutable QReadWriteLock m_readWriteLock;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_APISHADERMANAGER_H
