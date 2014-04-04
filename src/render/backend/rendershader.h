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

#ifndef QT3D_RENDER_RENDERSHADER_H
#define QT3D_RENDER_RENDERSHADER_H

#include <QVector>

// for Parameter::StandardUniforms enum - maybe should move to
// somewhere common to avoid this include?
#include <Qt3DRenderer/technique.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3D {

class ShaderProgram;

namespace Render {

class RenderShader
{
public:
    RenderShader(ShaderProgram* peer);

    /**
     * @brief getOrCreateProgram
     * must be called with a valid, current QOpenGLContext
     * @return
     */
    QOpenGLShaderProgram* getOrCreateProgram();

    /**
     * @brief setStandardUniform - set the program name of a uniform
     * @param su - the standard uniform
     * @param loc - the GLSL name corresponding to the uniform
     */
    void setStandardUniform(Parameter::StandardUniform su, QString name);

    /**
     * @brief standardUniformLocations - vecotr of uniform locations
     * within the compiled program, corresponding to the standard uniforms
     * [index the vector by the StandardUniform enum]
     * @return
     */
    QVector<int> standardUniformLocations() const
    {
        return m_standardUniformLocations;
    }

    QString name() const;
private:
    QOpenGLShaderProgram* m_program;
    ShaderProgram* m_peer;

    QOpenGLShaderProgram *createProgram();
    QOpenGLShaderProgram *createDefaultProgram();

    QVector<QString> m_standardUniformNames;
    QVector<int> m_standardUniformLocations;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSHADER_H
