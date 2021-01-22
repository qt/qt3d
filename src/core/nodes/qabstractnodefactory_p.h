/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
    static QVector<QAbstractNodeFactory *> nodeFactories();

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
