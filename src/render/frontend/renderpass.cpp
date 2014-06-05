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

#include "renderpass.h"
#include "parameter.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

RenderPass::RenderPass(Node *parent)
    : QAbstractRenderPass(parent)
    , m_stateSet(0)
{
}

void RenderPass::addUniformBinding(Parameter *param, QString glslUniformName)
{
    m_uniforms.append(param);
    m_parameterNameDict[param->name()] = glslUniformName;
}

void RenderPass::addAttributeBinding(Parameter *param, QString glslAttributeName)
{
    m_attributes.append(param);
    m_parameterNameDict[param->name()] = glslAttributeName;
}

QString RenderPass::glslNameForParameter(QString paramName) const
{
    return m_parameterNameDict.value(paramName);
}

ParameterList RenderPass::attributes() const
{
    return m_attributes;
}

ParameterList RenderPass::uniforms() const
{
    return m_uniforms;
}

void RenderPass::setStateSet(Render::DrawStateSet *ss)
{
    m_stateSet = ss;
}

Render::DrawStateSet *RenderPass::stateSet() const
{
    return m_stateSet;
}

} // namespace Qt3D

QT_END_NAMESPACE
