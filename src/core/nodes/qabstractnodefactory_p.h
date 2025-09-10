// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QABSTRACTNODEFACTORY_P_H
#define QABSTRACTNODEFACTORY_P_H

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

#include <Qt3DCore/qnode.h>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORE_PRIVATE_EXPORT QAbstractNodeFactory
{
public:
    virtual ~QAbstractNodeFactory();

    virtual QNode *createNode(const char *type) = 0;

    static void registerNodeFactory(QAbstractNodeFactory *factory);
    static QList<QAbstractNodeFactory *> nodeFactories();

    template<class T> static T *createNode(const char *type)
    {
        const auto factories = QAbstractNodeFactory::nodeFactories();
        for (QAbstractNodeFactory *f : factories) {
            QNode *n = f->createNode(type);
            if (n)
                return qobject_cast<T *>(n);
        }
        return new T;
    }
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QABSTRACTNODEFACTORY_P_H
