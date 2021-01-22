/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#ifndef QT3DCORE_QSKELETONCREATEDCHANGE_P_H
#define QT3DCORE_QSKELETONCREATEDCHANGE_P_H

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

#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSkeletonCreatedChangeBasePrivate;

class Q_3DCORESHARED_EXPORT QSkeletonCreatedChangeBase : public QNodeCreatedChangeBase
{
public:
    Q3D_DECL_DEPRECATED explicit QSkeletonCreatedChangeBase(const QAbstractSkeleton *skeleton);
    ~QSkeletonCreatedChangeBase();

    enum SkeletonType {
        Skeleton = 0,
        SkeletonLoader
    };

    SkeletonType type() const;

private:
    Q_DECLARE_PRIVATE(QSkeletonCreatedChangeBase)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QSkeletonCreatedChangeBase> QSkeletonCreatedChangeBasePtr;

template<typename T>
class QSkeletonCreatedChange : public QSkeletonCreatedChangeBase
{
public:
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_DEPRECATED
    explicit QSkeletonCreatedChange(const QAbstractSkeleton *_skeleton)
        : QSkeletonCreatedChangeBase(_skeleton)
        , data()
    {
    }
    QT_WARNING_POP

    T data;
};

template<typename T>
using QSkeletonCreatedChangePtr = QSharedPointer<QSkeletonCreatedChange<T>>;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSKELETONCREATEDCHANGE_P_H
