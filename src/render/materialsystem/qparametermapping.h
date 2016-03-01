/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QPARAMETERMAPPING_H
#define QT3DRENDER_QPARAMETERMAPPING_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameterMappingPrivate;

class QT3DRENDERSHARED_EXPORT QParameterMapping : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString parameterName READ parameterName WRITE setParameterName NOTIFY parameterNameChanged)
    Q_PROPERTY(QString shaderVariableName READ shaderVariableName WRITE setShaderVariableName NOTIFY shaderVariableNameChanged)
    Q_PROPERTY(Binding bindingType READ bindingType WRITE setBindingType NOTIFY bindingTypeChanged)
public:
    enum Binding
    {
        Uniform = 0,
        Attribute,
        StandardUniform,
        FragmentOutput,
        UniformBufferObject,
        ShaderStorageBufferObject
    };
    Q_ENUM(Binding)

    explicit QParameterMapping(Qt3DCore::QNode *parent = Q_NULLPTR);
    QParameterMapping(const QString &parameterName, const QString &shaderParameterName, QParameterMapping::Binding bindingType, Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QParameterMapping();

    QString parameterName() const;
    QString shaderVariableName() const;
    Binding bindingType() const;

public Q_SLOTS:
    void setParameterName(const QString &name);
    void setShaderVariableName(const QString &name);
    void setBindingType(Binding type);

Q_SIGNALS:
    void parameterNameChanged(const QString &parameterName);
    void shaderVariableNameChanged(const QString &shaderVariableName);
    void bindingTypeChanged(Binding bindingType);

protected:
    QParameterMapping(QParameterMappingPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QParameterMapping)
    QT3D_CLONEABLE(QParameterMapping)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPARAMETERBINDER_H
