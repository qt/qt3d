/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
