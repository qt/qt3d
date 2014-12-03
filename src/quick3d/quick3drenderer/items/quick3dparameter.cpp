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

#include "quick3dparameter.h"
#include "quick3dparameter_p.h"

#include <QJSValue>
#include <QJSValueIterator>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

namespace {

const int qjsValueTypeId = qMetaTypeId<QJSValue>();

}

Quick3DParameterPrivate::Quick3DParameterPrivate(Quick3DParameter *qq)
    : QParameterPrivate(qq)
{
}

void Quick3DParameterPrivate::setValue(const QVariant &value)
{
    if (value.userType() == qjsValueTypeId) {
        QJSValue v = value.value<QJSValue>();
        if (v.isArray()) {
            QJSValueIterator it(v);
            QVariantList values;
            while (it.hasNext()) {
                it.next();
                if (it.hasNext()) // The last element in an Array is the count, which we don't want
                    values << it.value().toVariant();
            }
            QParameterPrivate::setValue(values);
        }
    } else {
        QParameterPrivate::setValue(value);
    }
}

Quick3DParameter::Quick3DParameter(QNode *parent)
    : QParameter(*new Quick3DParameterPrivate(this), parent)
{
}

Quick3DParameter::Quick3DParameter(Quick3DParameterPrivate &dd, QNode *parent)
    : QParameter(dd, parent)
{
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE


