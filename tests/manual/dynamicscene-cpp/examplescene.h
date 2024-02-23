// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef EXAMPLESCENE_H
#define EXAMPLESCENE_H

#include <Qt3DCore/QEntity>

class BoxEntity;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class ExampleScene : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit ExampleScene(Qt3DCore::QNode *parent = 0);
    ~ExampleScene();

private Q_SLOTS:
    void updateScene();
    void buildScene();

private:
    QList<BoxEntity *> m_entities;
    QTimer *m_timer;
    bool m_even;
};

#endif // EXAMPLESCENE_H
