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

#include "shaderpropertyparser_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

ShaderPropertyParser::ShaderPropertyParser()
    : QQmlCustomParser(AcceptsAttachedProperties)
{
}

void ShaderPropertyParser::verifyBindings(const QV4::CompiledData::Unit *qmlUnit,
                                          const QList<const QV4::CompiledData::Binding *> &bindings)
{
    // TODO: Complete below when a proper Uniform API is defined
    Q_FOREACH (const QV4::CompiledData::Binding *binding, bindings) {
        QString propertyName = qmlUnit->stringAt(binding->propertyNameIndex);
        qDebug() << Q_FUNC_INFO << "propertyName " << propertyName;

        if (binding->type == QV4::CompiledData::Binding::Type_Object) {
            qDebug() << Q_FUNC_INFO << "Object property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_AttachedProperty) {
            qDebug() << Q_FUNC_INFO << "Attached property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_GroupProperty) {
            qDebug() << Q_FUNC_INFO << "Group property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_String) {
            qDebug() << Q_FUNC_INFO << "String property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_Boolean) {
            qDebug() << Q_FUNC_INFO << "Boolean property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_Number) {
            qDebug() << Q_FUNC_INFO << "Number property";
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_Script) {
            qDebug() << Q_FUNC_INFO << "Script property";
        }
    }
}

void ShaderPropertyParser::applyBindings(QObject *, QQmlCompiledData *, const QList<const QV4::CompiledData::Binding *> &)
{
    // TODO: Implement me
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
