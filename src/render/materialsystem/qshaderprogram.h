// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERPROGRAM_H
#define QT3DRENDER_QSHADERPROGRAM_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QShaderProgramPrivate;

class Q_3DRENDERSHARED_EXPORT QShaderProgram : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QByteArray vertexShaderCode READ vertexShaderCode WRITE setVertexShaderCode NOTIFY vertexShaderCodeChanged)
    Q_PROPERTY(QByteArray tessellationControlShaderCode READ tessellationControlShaderCode WRITE setTessellationControlShaderCode NOTIFY tessellationControlShaderCodeChanged)
    Q_PROPERTY(QByteArray tessellationEvaluationShaderCode READ tessellationEvaluationShaderCode WRITE setTessellationEvaluationShaderCode NOTIFY tessellationEvaluationShaderCodeChanged)
    Q_PROPERTY(QByteArray geometryShaderCode READ geometryShaderCode WRITE setGeometryShaderCode NOTIFY geometryShaderCodeChanged)
    Q_PROPERTY(QByteArray fragmentShaderCode READ fragmentShaderCode WRITE setFragmentShaderCode NOTIFY fragmentShaderCodeChanged)
    Q_PROPERTY(QByteArray computeShaderCode READ computeShaderCode WRITE setComputeShaderCode NOTIFY computeShaderCodeChanged)
    Q_PROPERTY(QString log READ log NOTIFY logChanged REVISION 9)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged REVISION 9)
    Q_PROPERTY(Format format READ format WRITE setFormat NOTIFY formatChanged REVISION 15)

public:
    explicit QShaderProgram(Qt3DCore::QNode *parent = nullptr);
    ~QShaderProgram();

    enum ShaderType {
        Vertex = 0,
        Fragment,
        TessellationControl,
        TessellationEvaluation,
        Geometry,
        Compute
    };
    Q_ENUM(ShaderType) // LCOV_EXCL_LINE

    enum Status {
        NotReady = 0,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    enum Format {
        GLSL = 0,
        SPIRV
    };
    Q_ENUM(Format) // LCOV_EXCL_LINE

    // Source code in-line
    QByteArray vertexShaderCode() const;
    QByteArray tessellationControlShaderCode() const;
    QByteArray tessellationEvaluationShaderCode() const;
    QByteArray geometryShaderCode() const;
    QByteArray fragmentShaderCode() const;
    QByteArray computeShaderCode() const;

    void setShaderCode(ShaderType type, const QByteArray &shaderCode);
    QByteArray shaderCode(ShaderType type) const;

    QString log() const;
    Status status() const;

    void setFormat(Format format);
    Format format() const;

    Q_INVOKABLE static QByteArray loadSource(const QUrl &sourceUrl);

public Q_SLOTS:
    void setVertexShaderCode(const QByteArray &vertexShaderCode);
    void setTessellationControlShaderCode(const QByteArray &tessellationControlShaderCode);
    void setTessellationEvaluationShaderCode(const QByteArray &tessellationEvaluationShaderCode);
    void setGeometryShaderCode(const QByteArray &geometryShaderCode);
    void setFragmentShaderCode(const QByteArray &fragmentShaderCode);
    void setComputeShaderCode(const QByteArray &computeShaderCode);

Q_SIGNALS:
    void vertexShaderCodeChanged(const QByteArray &vertexShaderCode);
    void tessellationControlShaderCodeChanged(const QByteArray &tessellationControlShaderCode);
    void tessellationEvaluationShaderCodeChanged(const QByteArray &tessellationEvaluationShaderCode);
    void geometryShaderCodeChanged(const QByteArray &geometryShaderCode);
    void fragmentShaderCodeChanged(const QByteArray &fragmentShaderCode);
    void computeShaderCodeChanged(const QByteArray &computeShaderCode);
    void logChanged(const QString &log);
    void statusChanged(Status status);
    void formatChanged(Format format);

protected:
    explicit QShaderProgram(QShaderProgramPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QShaderProgram)
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERPROGRAM_H
