// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QMLSCENEREADER_H
#define QMLSCENEREADER_H

#include <QObject>

QT_BEGIN_NAMESPACE

class QQmlEngine;
class QQmlComponent;

class QmlSceneReader : public QObject
{
    Q_OBJECT
public:
    explicit QmlSceneReader(const QUrl &source);
    ~QmlSceneReader();
    QObject *root() const;

private:
    QScopedPointer<QQmlEngine> m_qmlEngine;
    QScopedPointer<QQmlComponent> m_component;
    QObject *m_root;

    void loadComplete();
};

QT_END_NAMESPACE

#endif
