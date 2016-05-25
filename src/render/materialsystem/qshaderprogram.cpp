/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qshaderprogram.h"
#include "qshaderprogram_p.h"
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/private/qurlhelper_p.h>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

/*!
 * \qmltype ShaderProgram
 * \instantiates Qt3DRender::QShaderProgram
 * \inqmlmodule Qt3D.Render
 * \brief Encapsulates a Shader Program.
 */

/*!
 * \class Qt3DRender::QShaderProgram
 * \inmodule Qt3DRender
 *
 * \brief Encapsulates a Shader Program.
 */

/*!
    \enum QShaderProgram::ShaderType

    This enum identifies the type of shader used
    \value Vertex
    \value Fragment
    \value TessellationControl
    \value TessellationEvaluation
    \value Geometry
    \value Compute
*/

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QShaderProgramPrivate::QShaderProgramPrivate()
    : QNodePrivate()
{
}

/*!
  \fn Qt3DRender::QShaderProgram::QShaderProgram(Qt3DCore::QNode *parent)
  Constructs a new QShaderProgram with the specified \a parent.
 */
QShaderProgram::QShaderProgram(QNode *parent)
    : QNode(*new QShaderProgramPrivate, parent)
{
}

/*! \internal */
QShaderProgram::~QShaderProgram()
{
}

/*! \internal */
QShaderProgram::QShaderProgram(QShaderProgramPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::vertexShaderCode
    Specifies the vertex shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::vertexShaderCode
    Specifies the vertex shader code to be used
 */
void QShaderProgram::setVertexShaderCode(const QByteArray &vertexShaderCode)
{
    Q_D(QShaderProgram);
    if (vertexShaderCode != d->m_vertexShaderCode) {
        d->m_vertexShaderCode = vertexShaderCode;
        emit vertexShaderCodeChanged(vertexShaderCode);
    }
}

QByteArray QShaderProgram::vertexShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_vertexShaderCode;
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::tessellationControlShaderCode
    Specifies the tessellation control shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::tessellationControlShaderCode
    Specifies the tessellation control shader code to be used
 */
void QShaderProgram::setTessellationControlShaderCode(const QByteArray &tessellationControlShaderCode)
{
    Q_D(QShaderProgram);
    if (tessellationControlShaderCode != d->m_tessControlShaderCode) {
        d->m_tessControlShaderCode = tessellationControlShaderCode;
        emit tessellationControlShaderCodeChanged(tessellationControlShaderCode);
    }
}

QByteArray QShaderProgram::tessellationControlShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_tessControlShaderCode;
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::tessellationEvaluationShaderCode
    Specifies the tessellation evaluation shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::tessellationEvaluationShaderCode
    Specifies the tessellation evaluation shader code to be used
 */
void QShaderProgram::setTessellationEvaluationShaderCode(const QByteArray &tessellationEvaluationShaderCode)
{
    Q_D(QShaderProgram);
    if (tessellationEvaluationShaderCode != d->m_tessEvalShaderCode) {
        d->m_tessEvalShaderCode = tessellationEvaluationShaderCode;
        emit tessellationEvaluationShaderCodeChanged(tessellationEvaluationShaderCode);
    }
}

QByteArray QShaderProgram::tessellationEvaluationShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_tessEvalShaderCode;
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::geometryShaderCode
    Specifies the geometry shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::geometryShaderCode
    Specifies the geometry shader code to be used
 */
void QShaderProgram::setGeometryShaderCode(const QByteArray &geometryShaderCode)
{
    Q_D(QShaderProgram);
    if (geometryShaderCode != d->m_geometryShaderCode) {
        d->m_geometryShaderCode = geometryShaderCode;
        emit geometryShaderCodeChanged(geometryShaderCode);
    }
}

QByteArray QShaderProgram::geometryShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_geometryShaderCode;
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::fragmentShaderCode
    Specifies the fragment shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::fragmentShaderCode
    Specifies the fragment shader code to be used
 */
void QShaderProgram::setFragmentShaderCode(const QByteArray &fragmentShaderCode)
{
    Q_D(QShaderProgram);
    if (fragmentShaderCode != d->m_fragmentShaderCode) {
        d->m_fragmentShaderCode = fragmentShaderCode;
        emit fragmentShaderCodeChanged(fragmentShaderCode);
    }
}

QByteArray QShaderProgram::fragmentShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_fragmentShaderCode;
}

/*!
  \qmlproperty QByteArray Qt3D.Render::ShaderProgram::computeShaderCode
    Specifies the compute shader code to be used
*/

/*!
  \property Qt3DRender::QShaderProgram::computeShaderCode
    Specifies the compute shader code to be used
 */
void QShaderProgram::setComputeShaderCode(const QByteArray &computeShaderCode)
{
    Q_D(QShaderProgram);
    if (computeShaderCode != d->m_computeShaderCode) {
        d->m_computeShaderCode = computeShaderCode;
        emit computeShaderCodeChanged(computeShaderCode);
    }
}

QByteArray QShaderProgram::computeShaderCode() const
{
    Q_D(const QShaderProgram);
    return d->m_computeShaderCode;
}


/*!
 * Sets the \a type shader from raw data in \a shaderCode.
 */
void QShaderProgram::setShaderCode(ShaderType type, const QByteArray &shaderCode)
{
    switch (type) {
    case Vertex:
        setVertexShaderCode(shaderCode);
        break;
    case TessellationControl:
        setTessellationControlShaderCode(shaderCode);
        break;
    case TessellationEvaluation:
        setTessellationEvaluationShaderCode(shaderCode);
        break;
    case Geometry:
        setGeometryShaderCode(shaderCode);
        break;
    case Fragment:
        setFragmentShaderCode(shaderCode);
        break;
    case Compute:
        setComputeShaderCode(shaderCode);
        break;
    default:
        Q_UNREACHABLE();
    }
}

/*!
 * Returns the \a type shader code.
 */
QByteArray QShaderProgram::shaderCode(ShaderType type) const
{
    Q_D(const QShaderProgram);
    switch (type) {
    case Vertex:
        return d->m_vertexShaderCode;
    case TessellationControl:
        return d->m_tessControlShaderCode;
    case TessellationEvaluation:
        return d->m_tessEvalShaderCode;
    case Geometry:
        return d->m_geometryShaderCode;
    case Fragment:
        return d->m_fragmentShaderCode;
    case Compute:
        return d->m_computeShaderCode;
    default:
        Q_UNREACHABLE();
    }
}

static QByteArray deincludify(const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not read shader source file:" << f.fileName();
        return QByteArray();
    }

    QByteArray contents = f.readAll();
    QByteArrayList lines = contents.split('\n');
    const QByteArray includeDirective = QByteArrayLiteral("#pragma include");
    for (int i = 0; i < lines.count(); ++i) {
        if (lines[i].startsWith(includeDirective)) {
            QString includeFileName = QFileInfo(filePath).absolutePath()
                + QLatin1Char('/')
                + QString::fromUtf8(lines[i].mid(includeDirective.count() + 1));
            if (qEnvironmentVariableIsSet("QT3D_GLSL100_WORKAROUND")) {
                QString candidate = includeFileName + QLatin1String("100");
                if (QFile::exists(candidate))
                    includeFileName = candidate;
            }
            lines.removeAt(i);
            QByteArray includedContents = deincludify(includeFileName);
            lines.insert(i, includedContents);
            QString lineDirective = QString(QStringLiteral("#line %1")).arg(i + 2);
            lines.insert(i + 1, lineDirective.toUtf8());
        }
    }

    return lines.join('\n');
}

/*!
    \qmlmethod QByteArray ShaderProgram::loadSource(const QUrl &sourceUrl)

    Returns the shader code loaded from \a sourceUrl.
*/
/*!
 * Returns the shader code loaded from \a sourceUrl.
 */
QByteArray QShaderProgram::loadSource(const QUrl &sourceUrl)
{
    // TO DO: Handle remote path
    return deincludify(Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(sourceUrl));
}

Qt3DCore::QNodeCreatedChangeBasePtr QShaderProgram::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QShaderProgramData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QShaderProgram);
    data.vertexShaderCode = d->m_vertexShaderCode;
    data.tessellationControlShaderCode = d->m_tessControlShaderCode;
    data.tessellationEvaluationShaderCode = d->m_tessEvalShaderCode;
    data.geometryShaderCode = d->m_geometryShaderCode;
    data.fragmentShaderCode = d->m_fragmentShaderCode;
    data.computeShaderCode = d->m_computeShaderCode;
    return creationChange;
}

} // of namespace Qt3DRender

QT_END_NAMESPACE
