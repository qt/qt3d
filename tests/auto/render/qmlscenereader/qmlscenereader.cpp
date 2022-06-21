// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmlscenereader.h"
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDebug>

QT_BEGIN_NAMESPACE

QmlSceneReader::QmlSceneReader(const QUrl &source)
    : m_qmlEngine(new QQmlEngine)
    , m_component(new QQmlComponent(m_qmlEngine.data(), source, QQmlComponent::PreferSynchronous))
    , m_root(nullptr)
{
    loadComplete();
}

QmlSceneReader::~QmlSceneReader()
{}

// Caller needs to delete m_root
QObject *QmlSceneReader::root() const
{
    return m_root;
}

void QmlSceneReader::loadComplete()
{
    if (m_component->isError()) {
        const auto errors = m_component->errors();
        for (const QQmlError &error : errors) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        return;
    }
    m_root = m_component->create();
    if (m_component->isError()) {
        const auto errors = m_component->errors();
        for (const QQmlError &error : errors) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
    }
}

QT_END_NAMESPACE

#include "moc_qmlscenereader.cpp"
