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

#ifndef QT3DCORE_QNODECOMMAND_H
#define QT3DCORE_QNODECOMMAND_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qscenechange.h>

#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNodeCommandPrivate;

class Q_3DCORESHARED_EXPORT QNodeCommand : public QSceneChange
{
public:
#if defined(Q_ATOMIC_INT64_IS_SUPPORTED)
    typedef quint64 CommandId;
#else
    typedef quint32 CommandId;
#endif

    Q3D_DECL_DEPRECATED explicit QNodeCommand(QNodeId id);
    ~QNodeCommand();

    CommandId commandId() const;

    QString name() const;
    void setName(const QString &name);
    QVariant data() const;
    void setData(const QVariant &data);
    CommandId inReplyTo() const;
    void setReplyToCommandId(CommandId id);

protected:
    explicit QNodeCommand(QNodeCommandPrivate &dd, QNodeId id);

private:
    Q_DECLARE_PRIVATE(QNodeCommand)
};

typedef QSharedPointer<QNodeCommand> QNodeCommandPtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QNODECOMMAND_H
