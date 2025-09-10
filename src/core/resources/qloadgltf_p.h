// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QLOADGLTF_P_H
#define QT3DCORE_QLOADGLTF_P_H

#include <QtCore/qcborarray.h>
#include <QtCore/qcbormap.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <private/qglobal_p.h>

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

inline QJsonDocument qLoadGLTF(const QByteArray &gltfData)
{
    {
        const QCborValue cbor = QCborValue::fromCbor(gltfData);
        if (cbor.isMap())
            return QJsonDocument(cbor.toMap().toJsonObject());
        if (cbor.isArray())
            return QJsonDocument(cbor.toArray().toJsonArray());
    }
    return QJsonDocument::fromJson(gltfData);
}

#endif // QT3DCORE_QLOADGLTF_P_H
