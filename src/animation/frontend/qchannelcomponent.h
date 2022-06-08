// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCHANNELCOMPONENT_H
#define QT3DANIMATION_QCHANNELCOMPONENT_H

#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qkeyframe.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelComponentPrivate;

class Q_3DANIMATIONSHARED_EXPORT QChannelComponent
{
public:
    QChannelComponent();
    explicit QChannelComponent(const QString &name);
    QChannelComponent(const QChannelComponent &);
    QChannelComponent &operator=(const QChannelComponent &);
    ~QChannelComponent();

    void setName(const QString &name);
    QString name() const;

    int keyFrameCount() const;
    void appendKeyFrame(const QKeyFrame &kf);
    void insertKeyFrame(int index, const QKeyFrame &kf);
    void removeKeyFrame(int index);
    void clearKeyFrames();

    // Iterator API
    typedef const QKeyFrame *const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    const_iterator begin()  const noexcept;
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator end()    const noexcept;
    const_iterator cend()   const noexcept { return end(); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return rend(); }

    friend Q_3DANIMATIONSHARED_EXPORT bool operator==(const QChannelComponent &,
                                                      const QChannelComponent &) noexcept;
    friend Q_3DANIMATIONSHARED_EXPORT bool operator!=(const QChannelComponent &,
                                                      const QChannelComponent &) noexcept;

private:
    QScopedPointer<QChannelComponentPrivate> d;
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCHANNELCOMPONENT_H
