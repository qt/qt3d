// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtCore/private/qfactoryloader_p.h>
#include <Qt3DRender/private/qgeometryloaderinterface_p.h>
#include <Qt3DRender/private/qgeometryloaderfactory_p.h>

using namespace Qt3DRender;

class tst_ObjGeometryLoader : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkOutOfBoundFaceAccess_data()
    {
        QTest::addColumn<QString>("filePath");

        QTest::newRow("invalid position index") << ":invalid_vertex_position.obj";
    }


    void checkOutOfBoundFaceAccess()
    {
        // GIVEN
        QFETCH(QString, filePath);

        QFactoryLoader geometryLoader(QGeometryLoaderFactory_iid,
                                      QLatin1String("/geometryloaders"),
                                      Qt::CaseInsensitive);

        QScopedPointer<QGeometryLoaderInterface> loader;
        loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(&geometryLoader, QLatin1String("obj")));

        if (!loader)
            QSKIP("ObjLoaderPlugin not deployed");

        QFile file(filePath);
        QVERIFY(file.open(QIODevice::ReadOnly));

        // WHEN
        loader->load(&file);

        // THEN
        // -> shouldn't crash
    }
};

QTEST_MAIN(tst_ObjGeometryLoader)

#include "tst_objgeometryloader.moc"
