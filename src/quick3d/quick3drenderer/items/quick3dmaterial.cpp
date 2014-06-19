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

#include "quick3dmaterial.h"
#include <QDebug>
#include <Qt3DRenderer/texture.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DMaterial::Quick3DMaterial(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Parameter> Quick3DMaterial::qmlParameters()
{
    return QQmlListProperty<Parameter>(this, 0,
                                       &Quick3DMaterial::appendParameter,
                                       &Quick3DMaterial::parameterCount,
                                       &Quick3DMaterial::parameterAt,
                                       &Quick3DMaterial::clearParameters);
}

void Quick3DMaterial::appendParameter(QQmlListProperty<Parameter> *list, Parameter *param)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat) {
        param->setParent(mat->parentMaterial());
        mat->parentMaterial()->addParameter(param);
    }
}

Parameter *Quick3DMaterial::parameterAt(QQmlListProperty<Parameter> *list, int index)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat)
        return mat->parentMaterial()->parameters().at(index);
    return 0;
}

int Quick3DMaterial::parameterCount(QQmlListProperty<Parameter> *list)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat)
        return mat->parentMaterial()->parameters().count();
    return 0;
}

void Quick3DMaterial::clearParameters(QQmlListProperty<Parameter> *list)
{
    Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
    if (mat) {
        Q_FOREACH (Parameter *p, mat->parentMaterial()->parameters()) {
            mat->parentMaterial()->removeParameter(p);
        }
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
