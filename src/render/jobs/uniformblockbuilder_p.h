// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_UNIFORMBLOCKBUILDER_P_H
#define QT3DRENDER_RENDER_UNIFORMBLOCKBUILDER_P_H

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

#include <QtCore/qhash.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class ShaderDataManager;
class TextureManager;

typedef QHash<int, QVariant> UniformBlockValueBuilderHash;

struct Q_3DRENDERSHARED_PRIVATE_EXPORT UniformBlockValueBuilder
{
    explicit UniformBlockValueBuilder(const std::vector<int> &uniformNamesIds,
                                      ShaderDataManager *shaderDataManager,
                                      TextureManager *textureManager,
                                      const Matrix4x4 &matrix);

    void buildActiveUniformNameValueMapHelper(const ShaderData *currentShaderData,
                                              const QString &blockName,
                                              const int propertyInBlockNameId,
                                              const int propertyNameId,
                                              const ShaderData::PropertyValue *value);
    void buildActiveUniformNameValueMapStructHelper(ShaderData *rShaderData,
                                                    const QString &blockName,
                                                    const QString &qmlPropertyName = QString());

    UniformBlockValueBuilderHash activeUniformNamesToValue;

private:
    const std::vector<int> &m_uniformNamesIds;
    ShaderDataManager *m_shaderDataManager = nullptr;
    TextureManager *m_textureManager = nullptr;
    const Matrix4x4 &m_viewMatrix;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_UNIFORMBLOCKBUILDER_P_H
