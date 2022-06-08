// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCHANNEL_H
#define QT3DANIMATION_QCHANNEL_H

#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qchannelcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelPrivate;

class Q_3DANIMATIONSHARED_EXPORT QChannel
{
public:
    QChannel();
    explicit QChannel(const QString &name);
    QChannel(const QChannel &);
    QChannel &operator=(const QChannel &);
    ~QChannel();

    void setName(const QString &name);
    QString name() const;

    void setJointIndex(int jointIndex);
    int jointIndex() const;

    int channelComponentCount() const;
    void appendChannelComponent(const QChannelComponent &component);
    void insertChannelComponent(int index, const QChannelComponent &component);
    void removeChannelComponent(int index);
    void clearChannelComponents();

    // Iterator API
    typedef const QChannelComponent *const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    const_iterator begin()  const noexcept;
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator end()    const noexcept;
    const_iterator cend()   const noexcept { return end(); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return rend(); }

    friend Q_3DANIMATIONSHARED_EXPORT bool operator==(const QChannel &,
                                                      const QChannel &) noexcept;
    friend Q_3DANIMATIONSHARED_EXPORT bool operator!=(const QChannel &,
                                                      const QChannel &) noexcept;

private:
    QScopedPointer<QChannelPrivate> d;
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCHANNEL_H
