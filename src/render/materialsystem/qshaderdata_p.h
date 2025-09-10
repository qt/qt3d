// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

const int qVectorShaderDataTypeId = qMetaTypeId<QList<QShaderData *>>();

}

class QShaderDataPropertyReader: public PropertyReaderInterface
{
    QVariant readProperty(const QVariant &v) override
    {
        const auto node = v.value<Qt3DCore::QNode *>();

        if (node) {
            return QVariant::fromValue(node->id());
        } else if (v.userType() == qVectorShaderDataTypeId) {
            QVariantList vlist;
            const auto data_ = v.value<QList<QShaderData *>>();
            for (QShaderData *data : data_) {
                if (data)
                    vlist.append(QVariant::fromValue(data->id()));
            }
            return vlist;
        }
        return v;
    }
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderDataPrivate : public Qt3DCore::QComponentPrivate
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
