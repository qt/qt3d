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

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <node.h>
#include <qt3drenderer_global.h>

#include "shaderprogram.h"
#include <drawstate.h>

#include <QHash>

namespace Qt3D {

class Parameter;
typedef QList<Parameter*> ParameterList;

class QT3DRENDERERSHARED_EXPORT RenderPass : public Node
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Qt3D::ShaderProgram * shaderProgram READ shaderProgram WRITE setShaderProgram NOTIFY shaderProgramChanged)

public:
    explicit RenderPass(Node *parent = 0);

    void setName(const QString &name);
    QString name() const;

    void setShaderProgram(Qt3D::ShaderProgram* shaderProgram);
    Qt3D::ShaderProgram* shaderProgram() const;

    void addUniformBinding(Parameter* param, QString glslUniformName);

    void addAttributeBinding(Parameter* param, QString glslAttributeName);

    QString glslNameForParameter(QString paramName) const;

    ParameterList attributes() const;
    ParameterList uniforms() const;

    void setStateSet(Render::DrawStateSet* ss);
    Render::DrawStateSet* stateSet() const;

signals:
    void nameChanged();
    void shaderProgramChanged();

protected:
    QString m_name;
    ShaderProgram* m_shaderProgram;
    ParameterList m_attributes;
    ParameterList m_uniforms;

    // map Parameter names to GLSL names
    QHash<QString, QString> m_parameterNameDict;

    Render::DrawStateSet* m_stateSet;
};

}

#endif // RENDERPASS_H
