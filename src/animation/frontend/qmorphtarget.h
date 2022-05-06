/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

#ifndef QT3DANIMATION_QMORPHTARGET_H
#define QT3DANIMATION_QMORPHTARGET_H

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qgeometry.h>

#include <QtCore/qstringlist.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QMorphTargetPrivate;

class Q_3DANIMATIONSHARED_EXPORT QMorphTarget : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList attributeNames READ attributeNames NOTIFY attributeNamesChanged)

public:
    explicit QMorphTarget(QObject *parent = nullptr);

    QList<Qt3DCore::QAttribute *> attributeList() const;
    QStringList attributeNames() const;

    void setAttributes(const QList<Qt3DCore::QAttribute *> &attributes);
    void addAttribute(Qt3DCore::QAttribute *attribute);
    void removeAttribute(Qt3DCore::QAttribute *attribute);

    Q_INVOKABLE static QMorphTarget *fromGeometry(Qt3DCore::QGeometry *geometry,
                                                  const QStringList &attributes);

Q_SIGNALS:
    void attributeNamesChanged(const QStringList &attributeNames);

private:

    Q_DECLARE_PRIVATE(QMorphTarget)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QMORPHTARGET_H
