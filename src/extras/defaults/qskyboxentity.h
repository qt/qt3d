// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSKYBOXENTITY_H
#define QT3DEXTRAS_QSKYBOXENTITY_H

#include <Qt3DCore/qentity.h>
#include <Qt3DExtras/qt3dextras_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSkyboxEntityPrivate;

class Q_3DEXTRASSHARED_EXPORT QSkyboxEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QString baseName READ baseName WRITE setBaseName NOTIFY baseNameChanged)
    Q_PROPERTY(QString extension READ extension WRITE setExtension NOTIFY extensionChanged)
    Q_PROPERTY(bool gammaCorrect READ isGammaCorrectEnabled WRITE setGammaCorrectEnabled NOTIFY gammaCorrectEnabledChanged REVISION 9)
public:
    explicit QSkyboxEntity(Qt3DCore::QNode *parent = nullptr);
    ~QSkyboxEntity();

    QString baseName() const;
    QString extension() const;
    bool isGammaCorrectEnabled() const;

public Q_SLOTS:
    void setBaseName(const QString &path);
    void setExtension(const QString &extension);
    void setGammaCorrectEnabled(bool enabled);

Q_SIGNALS:
    void baseNameChanged(const QString &path);
    void extensionChanged(const QString &extension);
    void gammaCorrectEnabledChanged(bool enabled);

private:
    Q_DECLARE_PRIVATE(QSkyboxEntity)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSKYBOXENTITY_H
