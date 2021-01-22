/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#ifndef QT3DCORE_QLOADGLTF_P_H
#define QT3DCORE_QLOADGLTF_P_H

#include <QtCore/qcborarray.h>
#include <QtCore/qcbormap.h>
#include <QtCore/qcborvalue.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

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
#if QT_CONFIG(binaryjson)
    {
        QJsonDocument sceneDocument = QJsonDocument::fromBinaryData(gltfData);
        if (!sceneDocument.isNull())
            return sceneDocument;
    }
#endif
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
