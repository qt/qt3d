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

#ifndef QT3DRENDER_QEVENTFORWARD_H
#define QT3DRENDER_QEVENTFORWARD_H

#include <Qt3DRender/qt3drender_global.h>

#include <Qt3DCore/qnode.h>

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QEventForwardPrivate;

class QT3DRENDERSHARED_EXPORT QEventForward : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(bool forwardMouseEvents READ forwardMouseEvents WRITE setForwardMouseEvents NOTIFY forwardMouseEventsChanged)
    Q_PROPERTY(bool forwardKeyboardEvents READ forwardKeyboardEvents WRITE setForwardKeyboardEvents NOTIFY forwardKeyboardEventsChanged)
    Q_PROPERTY(QMatrix4x4 coordinateTransform READ coordinateTransform WRITE setCoordinateTransform NOTIFY coordinateTransformChanged)
    Q_PROPERTY(QString coordinateAttribute READ coordinateAttribute WRITE setCoordinateAttribute NOTIFY coordinateAttributeChanged)
    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged)

public:
    explicit QEventForward(Qt3DCore::QNode *parent = nullptr);
    ~QEventForward();

    QObject *target() const;
    QMatrix4x4 coordinateTransform() const;
    QString coordinateAttribute() const;
    bool forwardMouseEvents() const;
    bool forwardKeyboardEvents() const;
    bool focus() const;

public Q_SLOTS:
    void setTarget(QObject *target);
    void setCoordinateTransform(const QMatrix4x4 &coordinateTransform);
    void setCoordinateAttribute(const QString &coordinateAttribute);
    void setForwardMouseEvents(bool forward);
    void setForwardKeyboardEvents(bool forward);
    void setFocus(bool focus);

Q_SIGNALS:
    void targetChanged(QObject *target);
    void coordinateTransformChanged(const QMatrix4x4 &coordinateTransform);
    void coordinateAttributeChanged(const QString &coordinateAttribute);
    void forwardMouseEventsChanged(bool forward);
    void forwardKeyboardEventsChanged(bool forward);
    void focusChanged(bool focus);

private:
    Q_DECLARE_PRIVATE(QEventForward)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const Q_DECL_OVERRIDE;
};

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QEventForward*)

#endif // QT3DRENDER_QEVENTFORWARD_H
