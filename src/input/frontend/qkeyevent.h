// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QKEYEVENT_H
#define QT3DINPUT_QKEYEVENT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <QtCore/QObject>
#include <QtGui/QKeyEvent>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QKeyEventPrivate;
class QKeyEvent;

typedef QSharedPointer<QKeyEvent> QKeyEventPtr;

class Q_3DINPUTSHARED_EXPORT QKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(int modifiers READ modifiers CONSTANT)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat CONSTANT)
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(quint32 nativeScanCode READ nativeScanCode CONSTANT)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    explicit QKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text=QString(), bool autorep=false, ushort count=1);
    explicit QKeyEvent(const QT_PREPEND_NAMESPACE(QKeyEvent) &ke);
    ~QKeyEvent();

    inline int key() const { return m_event->key(); }
    inline QString text() const { return m_event->text(); }
    inline int modifiers() const { return int(m_event->modifiers().toInt()); }
    inline bool isAutoRepeat() const { return m_event->isAutoRepeat(); }
    inline int count() const { return m_event->count(); }
    inline quint32 nativeScanCode() const { return m_event->nativeScanCode(); }
    inline bool isAccepted() const { return m_event->isAccepted(); }
    inline void setAccepted(bool accepted) { m_event->setAccepted(accepted); }
    inline QEvent::Type type() const { return m_event->type(); }
#if QT_CONFIG(shortcut)
    Q_INVOKABLE bool matches(QKeySequence::StandardKey key_) const { return m_event->matches(key_); }
#endif

private:
    std::unique_ptr<QT_PREPEND_NAMESPACE(QKeyEvent)> m_event;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QKEYEVENT_H
