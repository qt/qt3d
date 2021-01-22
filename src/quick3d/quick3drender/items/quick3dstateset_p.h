/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DSTATESET_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DSTATESET_P_H

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

#include <Qt3DRender/qrenderstateset.h>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DStateSet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QRenderState> renderStates READ renderStateList CONSTANT)
public:
    explicit Quick3DStateSet(QObject *parent = 0);
    ~Quick3DStateSet();

    QQmlListProperty<QRenderState> renderStateList();
    inline QRenderStateSet *parentStateSet() const { return qobject_cast<QRenderStateSet *>(parent()); }

private:
    static void appendRenderState(QQmlListProperty<QRenderState> *list, QRenderState *state);
    static QRenderState *renderStateAt(QQmlListProperty<QRenderState> *list, int index);
    static int renderStateCount(QQmlListProperty<QRenderState> *list);
    static void clearRenderStates(QQmlListProperty<QRenderState> *list);
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DSTATESET_P_H
