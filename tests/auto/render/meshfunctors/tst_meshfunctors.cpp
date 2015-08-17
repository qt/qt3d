/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DRenderer/qgeometryfunctor.h>
#include <Qt3DRenderer/qgeometry.h>

class MeshFunctorA : public Qt3D::QGeometryFunctor
{
public:
    MeshFunctorA()
    {}

    ~MeshFunctorA()
    {}

    Qt3D::QGeometry *operator ()() Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

    bool operator ==(const Qt3D::QGeometryFunctor &other) const Q_DECL_OVERRIDE
    {
        return functor_cast<MeshFunctorA>(&other);
    }

    QT3D_FUNCTOR(MeshFunctorA)
};

class MeshFunctorB : public Qt3D::QGeometryFunctor
{
public:
    MeshFunctorB()
    {}

    ~MeshFunctorB()
    {}

    Qt3D::QGeometry *operator ()() Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

    bool operator ==(const Qt3D::QGeometryFunctor &other) const Q_DECL_OVERRIDE
    {
        return functor_cast<MeshFunctorB>(&other);
    }

    QT3D_FUNCTOR(MeshFunctorB)
};

class MeshFunctorASub : public MeshFunctorA
{
public:
    MeshFunctorASub()
    {}

    ~MeshFunctorASub()
    {}

    bool operator ==(const Qt3D::QGeometryFunctor &other) const Q_DECL_OVERRIDE
    {
        return functor_cast<MeshFunctorASub>(&other);
    }

    QT3D_FUNCTOR(MeshFunctorASub)
};


class tst_MeshFunctors : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void functorComparison()
    {
        // GIVEN
        QScopedPointer<MeshFunctorA> functorA(new MeshFunctorA);
        QScopedPointer<MeshFunctorB> functorB(new MeshFunctorB);
        QScopedPointer<MeshFunctorASub> functorASub(new MeshFunctorASub);

        // THEN
        QVERIFY(!(*functorA == *functorB));
        QVERIFY(!(*functorA == *functorASub));

        QVERIFY(!(*functorB == *functorA));
        QVERIFY(!(*functorB == *functorASub));

        QVERIFY(!(*functorASub == *functorA));
        QVERIFY(!(*functorASub == *functorB));

        QVERIFY(*functorA == *functorA);
        QVERIFY(*functorB == *functorB);
        QVERIFY(*functorASub == *functorASub);
    }
};

QTEST_APPLESS_MAIN(tst_MeshFunctors)

#include "tst_meshfunctors.moc"
