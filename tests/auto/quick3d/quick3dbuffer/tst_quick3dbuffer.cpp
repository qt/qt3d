// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DQuick/private/quick3dbuffer_p.h>
#include <QObject>
#include <QVector3D>

namespace {

bool writeBinaryFile(const QString filePath, const void *data, int byteSize)
{
    QFile f(filePath);
    if (f.open(QIODevice::WriteOnly))
        return f.write(reinterpret_cast<const char *>(data), byteSize) == byteSize;
    return false;
}

} // anonymous

class tst_Quick3DBuffer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInvalidBinaryFile()
    {
        // GIVEN
        Qt3DCore::Quick::Quick3DBuffer buf;

        // WHEN
        QVariant data = buf.readBinaryFile(QUrl::fromLocalFile(QLatin1String("this_should_not_exist.bin")));

        // THEN
        QCOMPARE(data.userType(), static_cast<int>(QMetaType::QByteArray));
        QVERIFY(data.value<QByteArray>().isEmpty());
    }

    void checkValidBinaryFile()
    {
        // GIVEN
        Qt3DCore::Quick::Quick3DBuffer buf;
        const QVector<QVector3D> dataArray = { QVector3D(327.0f, 350.0f, 355.0f),
                                              QVector3D(383.0f, 427.0f, 454.0f) };

        const int bufferByteSize = int(dataArray.size()) * sizeof(QVector3D);
        const QLatin1String filePath("binary_data.bin");
        const bool writingSucceeded = writeBinaryFile(filePath, dataArray.constData(), bufferByteSize);
        Q_ASSERT(writingSucceeded);

        // WHEN
        const QUrl path = QUrl::fromLocalFile(filePath);
        QVariant data = buf.readBinaryFile(path);

        // THEN
        QCOMPARE(data.userType(), static_cast<int>(QMetaType::QByteArray));
        const QByteArray byteArray = data.value<QByteArray>();
        QCOMPARE(byteArray.size(), bufferByteSize);
        QVERIFY(memcmp(byteArray, dataArray.constData(), bufferByteSize) == 0);
    }

};

QTEST_MAIN(tst_Quick3DBuffer)

#include "tst_quick3dbuffer.moc"
