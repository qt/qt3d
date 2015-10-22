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

#ifndef QT3DRENDER_QSHADERDATA_P_H
#define QT3DRENDER_QSHADERDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qcomponent_p.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace {

const int qListShaderDataTypeId = qMetaTypeId<QList<QShaderData*> >();
const int qShaderDataTypeId = qMetaTypeId<QShaderData*>();

}

class QShaderDataPropertyReader: public PropertyReaderInterface
{
    QVariant readProperty(const QVariant &v) Q_DECL_OVERRIDE
    {
        QShaderData *shaderData = Q_NULLPTR;

        if (v.userType() == qShaderDataTypeId && (shaderData = v.value<QShaderData *>()) != Q_NULLPTR) {
            return QVariant::fromValue(shaderData->id());
        } else if (v.userType() == qListShaderDataTypeId) {
            QVariantList vlist;
            Q_FOREACH (QShaderData *data, v.value<QList<QShaderData *> >()) {
                if (data)
                    vlist.append(QVariant::fromValue(data->id()));
            }
            return vlist;
        }
        return v;
    }
};

class QT3DRENDERSHARED_PRIVATE_EXPORT QShaderDataPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QShaderDataPrivate();
    QShaderDataPrivate(PropertyReaderInterfacePtr reader);
    PropertyReaderInterfacePtr m_propertyReader;

    Q_DECLARE_PUBLIC(QShaderData)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERDATA_P_H
