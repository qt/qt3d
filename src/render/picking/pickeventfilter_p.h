// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PICKEVENTFILTER_H
#define QT3DRENDER_RENDER_PICKEVENTFILTER_H

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

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtCore/qmutex.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderAspectPrivate;

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT PickEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit PickEventFilter(QRenderAspectPrivate *aspect, QObject *parent = nullptr);
    ~PickEventFilter();

protected:
    bool eventFilter(QObject *obj, QEvent *e) final;

private:
    QRenderAspectPrivate *m_aspect;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKEVENTFILTER_H
