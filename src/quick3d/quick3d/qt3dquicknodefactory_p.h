// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QUICKNODEFACTORY_H
#define QUICKNODEFACTORY_H

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

#include <QtCore/qhash.h>
#include <QtQml/private/qqmlmetatype_p.h>
#include <Qt3DCore/private/qabstractnodefactory_p.h>

QT_BEGIN_NAMESPACE

class QQmlType;

namespace Qt3DCore {

class QuickNodeFactory : public QAbstractNodeFactory
{
public:
    QNode *createNode(const char *type) override;

    void registerType(const char *className, const char *quickName, int major, int minor);

    static QuickNodeFactory *instance();

private:
    struct Type {
        Type() : t(nullptr), resolved(false) { }
        Type(const char *quickName, int major, int minor)
            : quickName(quickName), version(major, minor), t(nullptr), resolved(false) { }
        QByteArray quickName;
        QPair<int, int> version;
        QQmlType t;
        bool resolved;
    };
    QHash<QByteArray, Type> m_types;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QUICKNODEFACTORY_H
