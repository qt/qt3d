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

#ifndef QT3DRENDER_QSHADERPROGRAMBUILDER_P_H
#define QT3DRENDER_QSHADERPROGRAMBUILDER_P_H

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

#include <private/qnode_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderProgramBuilderPrivate : public Qt3DCore::QNodePrivate
{
public:
    QShaderProgramBuilderPrivate();

    Q_DECLARE_PUBLIC(QShaderProgramBuilder)
    QShaderProgram *m_shaderProgram;
    QStringList m_enabledLayers;
    QUrl m_vertexShaderGraph;
    QUrl m_tessControlShaderGraph;
    QUrl m_tessEvalShaderGraph;
    QUrl m_geometryShaderGraph;
    QUrl m_fragmentShaderGraph;
    QUrl m_computeShaderGraph;
    QByteArray m_vertexShaderCode;
    QByteArray m_tessControlShaderCode;
    QByteArray m_tessEvalShaderCode;
    QByteArray m_geometryShaderCode;
    QByteArray m_fragmentShaderCode;
    QByteArray m_computeShaderCode;

    void setShaderCode(const QByteArray &code, QShaderProgram::ShaderType type);
};

struct QShaderProgramBuilderData
{
    Qt3DCore::QNodeId shaderProgramId;
    QStringList enabledLayers;
    QUrl vertexShaderGraph;
    QUrl tessellationControlShaderGraph;
    QUrl tessellationEvaluationShaderGraph;
    QUrl geometryShaderGraph;
    QUrl fragmentShaderGraph;
    QUrl computeShaderGraph;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERPROGRAMBUILDER_P_H
