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

#include "quick3dmaterial.h"
#include <Qt3DRenderer/qtexture.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DMaterial::Quick3DMaterial(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QParameter> Quick3DMaterial::qmlParameters()
{
    return QQmlListProperty<QParameter>(this, 0,
                                       &Quick3DMaterial::appendParameter,
                                       &Quick3DMaterial::parameterCount,
                                       &Quick3DMaterial::parameterAt,
                                       &Quick3DMaterial::clearParameters);
}

void Quick3DMaterial::appendParameter(QQmlListProperty<QParameter> *list, QParameter *param)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat) {
        param->setParent(mat->parentMaterial());
        mat->parentMaterial()->addParameter(param);
    }
}

QParameter *Quick3DMaterial::parameterAt(QQmlListProperty<QParameter> *list, int index)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat)
        return mat->parentMaterial()->parameters().at(index);
    return 0;
}

int Quick3DMaterial::parameterCount(QQmlListProperty<QParameter> *list)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat)
        return mat->parentMaterial()->parameters().count();
    return 0;
}

void Quick3DMaterial::clearParameters(QQmlListProperty<QParameter> *list)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat) {
        Q_FOREACH (QParameter *p, mat->parentMaterial()->parameters()) {
            mat->parentMaterial()->removeParameter(p);
        }
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
