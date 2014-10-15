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

#ifndef QT3D_QPARAMETERBINDER_H
#define QT3D_QPARAMETERBINDER_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QParameterMapperPrivate;

class QT3DRENDERERSHARED_EXPORT QParameterMapper : public QNode
{
    Q_OBJECT
    Q_ENUMS(Binding)
    Q_PROPERTY(QString parameterName READ parameterName WRITE setParameterName NOTIFY parameterNameChanged)
    Q_PROPERTY(QString shaderVariableName READ shaderVariableName WRITE setShaderVariableName NOTIFY shaderVariableNameChanged)
    Q_PROPERTY(Binding bindingType READ bindingType WRITE setBindingType NOTIFY bindingTypeChanged)
public:
    enum Binding
    {
        Uniform = 0,
        Attribute,
        StandardUniform,
        FragmentOutput
    };

    explicit QParameterMapper(QNode *parent = 0);
    QParameterMapper(const QString &parameterName, const QString &shaderParameterName, QParameterMapper::Binding bindingType, QNode *parent = 0);

    void setParameterName(const QString &name);
    void setShaderVariableName(const QString &name);
    void setBindingType(Binding type);

    QString parameterName() const;
    QString shaderVariableName() const;
    Binding bindingType() const;

Q_SIGNALS:
    void parameterNameChanged();
    void shaderVariableNameChanged();
    void bindingTypeChanged();

protected:
    QParameterMapper(QParameterMapperPrivate &dd, QNode *parent = 0);

private:
    Q_DECLARE_PRIVATE(QParameterMapper)
    QT3D_CLONEABLE(QParameterMapper)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QPARAMETERBINDER_H
