/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
