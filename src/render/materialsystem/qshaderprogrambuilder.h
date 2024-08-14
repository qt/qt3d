// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERPROGRAMBUILDER_H
#define QT3DRENDER_QSHADERPROGRAMBUILDER_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qshaderprogram.h>

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QShaderProgramBuilderPrivate;

class Q_3DRENDERSHARED_EXPORT QShaderProgramBuilder : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QShaderProgram* shaderProgram READ shaderProgram WRITE setShaderProgram NOTIFY shaderProgramChanged)
    Q_PROPERTY(QStringList enabledLayers READ enabledLayers WRITE setEnabledLayers NOTIFY enabledLayersChanged)
    Q_PROPERTY(QUrl vertexShaderGraph READ vertexShaderGraph WRITE setVertexShaderGraph NOTIFY vertexShaderGraphChanged)
    Q_PROPERTY(QUrl tessellationControlShaderGraph READ tessellationControlShaderGraph WRITE setTessellationControlShaderGraph NOTIFY tessellationControlShaderGraphChanged)
    Q_PROPERTY(QUrl tessellationEvaluationShaderGraph READ tessellationEvaluationShaderGraph WRITE setTessellationEvaluationShaderGraph NOTIFY tessellationEvaluationShaderGraphChanged)
    Q_PROPERTY(QUrl geometryShaderGraph READ geometryShaderGraph WRITE setGeometryShaderGraph NOTIFY geometryShaderGraphChanged)
    Q_PROPERTY(QUrl fragmentShaderGraph READ fragmentShaderGraph WRITE setFragmentShaderGraph NOTIFY fragmentShaderGraphChanged)
    Q_PROPERTY(QUrl computeShaderGraph READ computeShaderGraph WRITE setComputeShaderGraph NOTIFY computeShaderGraphChanged)
    Q_PROPERTY(QByteArray vertexShaderCode READ vertexShaderCode NOTIFY vertexShaderCodeChanged REVISION(2, 13))
    Q_PROPERTY(QByteArray tessellationControlShaderCode READ tessellationControlShaderCode NOTIFY tessellationControlShaderCodeChanged REVISION(2, 13))
    Q_PROPERTY(QByteArray tessellationEvaluationShaderCode READ tessellationEvaluationShaderCode  NOTIFY tessellationEvaluationShaderCodeChanged REVISION(2, 13))
    Q_PROPERTY(QByteArray geometryShaderCode READ geometryShaderCode NOTIFY geometryShaderCodeChanged REVISION(2, 13))
    Q_PROPERTY(QByteArray fragmentShaderCode READ fragmentShaderCode NOTIFY fragmentShaderCodeChanged REVISION(2, 13))
    Q_PROPERTY(QByteArray computeShaderCode READ computeShaderCode NOTIFY computeShaderCodeChanged REVISION(2, 13))

public:
    explicit QShaderProgramBuilder(Qt3DCore::QNode *parent = nullptr);
    ~QShaderProgramBuilder();

    QShaderProgram *shaderProgram() const;
    QStringList enabledLayers() const;
    QUrl vertexShaderGraph() const;
    QUrl tessellationControlShaderGraph() const;
    QUrl tessellationEvaluationShaderGraph() const;
    QUrl geometryShaderGraph() const;
    QUrl fragmentShaderGraph() const;
    QUrl computeShaderGraph() const;
    QByteArray vertexShaderCode() const;
    QByteArray tessellationControlShaderCode() const;
    QByteArray tessellationEvaluationShaderCode() const;
    QByteArray geometryShaderCode() const;
    QByteArray fragmentShaderCode() const;
    QByteArray computeShaderCode() const;

public Q_SLOTS:
    void setShaderProgram(Qt3DRender::QShaderProgram *program);
    void setEnabledLayers(const QStringList &layers);
    void setVertexShaderGraph(const QUrl &vertexShaderGraph);
    void setTessellationControlShaderGraph(const QUrl &tessellationControlShaderGraph);
    void setTessellationEvaluationShaderGraph(const QUrl &tessellationEvaluationShaderGraph);
    void setGeometryShaderGraph(const QUrl &geometryShaderGraph);
    void setFragmentShaderGraph(const QUrl &fragmentShaderGraph);
    void setComputeShaderGraph(const QUrl &computeShaderGraph);

Q_SIGNALS:
    void shaderProgramChanged(Qt3DRender::QShaderProgram *shaderProgram);
    void enabledLayersChanged(const QStringList &layers);
    void vertexShaderGraphChanged(const QUrl &vertexShaderGraph);
    void tessellationControlShaderGraphChanged(const QUrl &tessellationControlShaderGraph);
    void tessellationEvaluationShaderGraphChanged(const QUrl &tessellationEvaluationShaderGraph);
    void geometryShaderGraphChanged(const QUrl &geometryShaderGraph);
    void fragmentShaderGraphChanged(const QUrl &fragmentShaderGraph);
    void computeShaderGraphChanged(const QUrl &computeShaderGraph);
    Q_REVISION(13) void vertexShaderCodeChanged(const QByteArray &vertexShaderCode);
    Q_REVISION(13) void tessellationControlShaderCodeChanged(const QByteArray &tessellationControlShaderCode);
    Q_REVISION(13) void tessellationEvaluationShaderCodeChanged(const QByteArray &tessellationEvaluationShaderCode);
    Q_REVISION(13) void geometryShaderCodeChanged(const QByteArray &geometryShaderCode);
    Q_REVISION(13) void fragmentShaderCodeChanged(const QByteArray &fragmentShaderCode);
    Q_REVISION(13) void computeShaderCodeChanged(const QByteArray &computeShaderCode);

protected:
    explicit QShaderProgramBuilder(QShaderProgramBuilderPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QShaderProgramBuilder)
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERPROGRAMBUILDER_H
