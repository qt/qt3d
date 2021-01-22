/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#ifndef QT3DRENDER_RENDER_SHADERBUILDER_H
#define QT3DRENDER_RENDER_SHADERBUILDER_H

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

#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

struct ShaderBuilderUpdate
{
    Qt3DCore::QNodeId builderId;
    Qt3DRender::QShaderProgram::ShaderType shaderType;
    QByteArray shaderCode;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT ShaderBuilder : public BackendNode
{
public:
    static QString getPrototypesFile();
    static void setPrototypesFile(const QString &file);
    static QStringList getPrototypeNames();

    ShaderBuilder();
    ~ShaderBuilder();
    void cleanup();

    Qt3DCore::QNodeId shaderProgramId() const;
    QStringList enabledLayers() const;

    GraphicsApiFilterData graphicsApi() const;
    void setGraphicsApi(const GraphicsApiFilterData &graphicsApi);

    QUrl shaderGraph(QShaderProgram::ShaderType type) const;
    void setShaderGraph(QShaderProgram::ShaderType type, const QUrl &url);

    QByteArray shaderCode(QShaderProgram::ShaderType type) const;
    bool isShaderCodeDirty(QShaderProgram::ShaderType type) const;

    void generateCode(QShaderProgram::ShaderType type);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    QVector<ShaderBuilderUpdate> takePendingUpdates() { return std::move(m_pendingUpdates); }

    QByteArray hashKeyForShaderGraph(QShaderProgram::ShaderType type) const;

private:
    void setEnabledLayers(const QStringList &layers);

    GraphicsApiFilterData m_graphicsApi;
    Qt3DCore::QNodeId m_shaderProgramId;
    QStringList m_enabledLayers;
    QHash<QShaderProgram::ShaderType, QUrl> m_graphs;
    QHash<QShaderProgram::ShaderType, QByteArray> m_codes;
    QSet<QShaderProgram::ShaderType> m_dirtyTypes;
    QVector<ShaderBuilderUpdate> m_pendingUpdates;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SHADERBUILDER_H
