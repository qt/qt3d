/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QSHADERPROGRAM_H
#define QT3D_QSHADERPROGRAM_H

#include <QUuid>
#include <Qt3DCore/qabstractshader.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QShaderProgramPrivate;

class QT3DRENDERERSHARED_EXPORT QShaderProgram : public QAbstractShader
{
    Q_OBJECT

    Q_PROPERTY(QString vertexShaderSourceFile READ vertexShaderSourceFile WRITE setVertexShaderSourceFile NOTIFY vertexShaderSourceFileChanged)
    Q_PROPERTY(QString tessellationControlShaderSourceFile READ tessellationControlShaderSourceFile WRITE setTessellationControlShaderSourceFile NOTIFY tessellationControlShaderSourceFileChanged)
    Q_PROPERTY(QString tessellationEvaluationShaderSourceFile READ tessellationEvaluationShaderSourceFile WRITE setTessellationEvaluationShaderSourceFile NOTIFY tessellationEvaluationShaderSourceFileChanged)
    Q_PROPERTY(QString geometryShaderSourceFile READ geometryShaderSourceFile WRITE setGeometryShaderSourceFile NOTIFY geometryShaderSourceFileChanged)
    Q_PROPERTY(QString fragmentShaderSourceFile READ fragmentShaderSourceFile WRITE setFragmentShaderSourceFile NOTIFY fragmentShaderSourceFileChanged)
    Q_PROPERTY(QString computeShaderSourceFile READ computeShaderSourceFile WRITE setComputeShaderSourceFile NOTIFY computeShaderSourceFileChanged)
    Q_PROPERTY(QByteArray vertexShaderCode READ vertexShaderCode WRITE setVertexShaderCode NOTIFY vertexShaderCodeChanged)
    Q_PROPERTY(QByteArray tessellationControlShaderCode READ tessellationControlShaderCode WRITE setTessellationControlShaderCode NOTIFY tessellationControlShaderCodeChanged)
    Q_PROPERTY(QByteArray tessellationEvaluationShaderCode READ tessellationEvaluationShaderCode WRITE setTessellationEvaluationShaderCode NOTIFY tessellationEvaluationShaderCodeChanged)
    Q_PROPERTY(QByteArray geometryShaderCode READ geometryShaderCode WRITE setGeometryShaderCode NOTIFY geometryShaderCodeChanged)
    Q_PROPERTY(QByteArray fragmentShaderCode READ fragmentShaderCode WRITE setFragmentShaderCode NOTIFY fragmentShaderCodeChanged)
    Q_PROPERTY(QByteArray computeShaderCode READ computeShaderCode WRITE setComputeShaderCode NOTIFY computeShaderCodeChanged)
    Q_ENUMS(ShaderType)

public:
    explicit QShaderProgram(QNode *parent = 0);

    enum ShaderType {
        Vertex = 0,
        Fragment,
        TessellationControl,
        TessellationEvaluation,
        Geometry,
        Compute
    };

    void copy(const QNode *ref) Q_DECL_OVERRIDE;

    // Source from file
    void setVertexShaderSourceFile(const QString &vertexShaderSourceFile);
    QString vertexShaderSourceFile() const;

    void setTessellationControlShaderSourceFile(const QString &tessellationControlShaderSourceFile);
    QString tessellationControlShaderSourceFile() const;

    void setTessellationEvaluationShaderSourceFile(const QString &tessellationEvaluationShaderSourceFile);
    QString tessellationEvaluationShaderSourceFile() const;

    void setGeometryShaderSourceFile(const QString &geometryShaderSourceFile);
    QString geometryShaderSourceFile() const;

    void setFragmentShaderSourceFile(const QString &fragmentShaderSourceFile);
    QString fragmentShaderSourceFile() const;

    void setComputeShaderSourceFile(const QString &computeShaderSourceFile);
    QString computeShaderSourceFile() const;

    void setShaderSourceFile(ShaderType type, const QString &sourceFile);
    QString shaderSourceFile(ShaderType type) const;

    // Source code in-line
    void setVertexShaderCode(const QByteArray &vertexShaderCode);
    QByteArray vertexShaderCode() const;

    void setTessellationControlShaderCode(const QByteArray &tessellationControlShaderCode);
    QByteArray tessellationControlShaderCode() const;

    void setTessellationEvaluationShaderCode(const QByteArray &tessellationEvaluationShaderCode);
    QByteArray tessellationEvaluationShaderCode() const;

    void setGeometryShaderCode(const QByteArray &geometryShaderCode);
    QByteArray geometryShaderCode() const;

    void setFragmentShaderCode(const QByteArray &fragmentShaderCode);
    QByteArray fragmentShaderCode() const;

    void setComputeShaderCode(const QByteArray &computeShaderCode);
    QByteArray computeShaderCode() const;

    void setShaderCode(ShaderType type, const QByteArray &shaderCode);
    QByteArray shaderCode(ShaderType type) const;

Q_SIGNALS:
    void vertexShaderSourceFileChanged();
    void tessellationControlShaderSourceFileChanged();
    void tessellationEvaluationShaderSourceFileChanged();
    void geometryShaderSourceFileChanged();
    void fragmentShaderSourceFileChanged();
    void computeShaderSourceFileChanged();

    void vertexShaderCodeChanged();
    void tessellationControlShaderCodeChanged();
    void tessellationEvaluationShaderCodeChanged();
    void geometryShaderCodeChanged();
    void fragmentShaderCodeChanged();
    void computeShaderCodeChanged();

protected:
    Q_DECLARE_PRIVATE(QShaderProgram)
    QShaderProgram(QShaderProgramPrivate &dd, QNode *parent = 0);
    QShaderProgram *doClone(bool isClone) const Q_DECL_OVERRIDE;
};

}

QT_END_NAMESPACE

#endif // QT3D_QSHADERPROGRAM_H
