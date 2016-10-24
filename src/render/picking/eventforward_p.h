/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#ifndef QT3DRENDER_RENDER_EVENTFORWARD_P_H
#define QT3DRENDER_RENDER_EVENTFORWARD_P_H


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

#include <private/backendnode_p.h>

#include <QtGui/qmatrix4x4.h>
#include <QtGui/qevent.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_AUTOTEST_EXPORT EventForward : public BackendNode
{
public:
    EventForward();
    ~EventForward();

    void cleanup();

    QObject *target() const;
    QString coordinateAttribute() const;
    QMatrix4x4 coordinateTransform() const;
    bool forwardMouseEvents() const;
    bool forwardKeyboardEvents() const;

    void setTarget(QObject *target);
    void setCoordinateTransform(const QMatrix4x4 &transform);
    void setCoordinateAttribute(const QString &attribute);
    void setForwardMouseEvents(bool enabled);
    void setForwardKeyboardEvents(bool enabled);

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_FINAL;

    void forward(const QMouseEvent &event, const QVector4D &coordinate);
    void forward(const QList<QKeyEvent> &events);

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) Q_DECL_FINAL;

    QObject *m_target;
    QString m_coordinateAttribute;
    QMatrix4x4 m_coordinateTransform;
    bool m_forwardMouseEvents;
    bool m_forwardKeyboardEvents;
};

} // Render
} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_EVENTFORWARD_P_H
