// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
