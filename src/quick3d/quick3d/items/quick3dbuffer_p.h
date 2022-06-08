// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DBUFFER_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DBUFFER_P_H

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

#include <Qt3DCore/QBuffer>

#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

class QQmlEngine;
class QJSValue;

namespace QV4 {
struct ExecutionEngine;
}

namespace Qt3DCore {

namespace Quick {

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DBuffer : public Qt3DCore::QBuffer
{
    Q_OBJECT
    Q_PROPERTY(QVariant data READ bufferData WRITE setBufferData NOTIFY bufferDataChanged)
    QML_NAMED_ELEMENT(Buffer)
    QML_ADDED_IN_VERSION(2, 0)
public:
    explicit Quick3DBuffer(Qt3DCore::QNode *parent = nullptr);

    QVariant bufferData() const;
    void setBufferData(const QVariant &bufferData);

    Q_INVOKABLE QVariant readBinaryFile(const QUrl &fileUrl);

public Q_SLOTS:
    void updateData(int offset, const QVariant &bytes);

Q_SIGNALS:
    void bufferDataChanged();

private:
    QQmlEngine *m_engine;
    QV4::ExecutionEngine *m_v4engine;
    void initEngines();
    QByteArray convertToRawData(const QJSValue &jsValue);
};

} // Quick

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DBUFFER_P_H
