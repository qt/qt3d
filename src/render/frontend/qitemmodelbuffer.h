/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QITEMMODELBUFFER_H
#define QT3D_QITEMMODELBUFFER_H

#include <QAbstractItemModel>
#include <QMap>
#include <QObject>

#include <meshdata.h>

namespace Qt3D {

class QItemModelBuffer : public QObject
{
    Q_OBJECT
public:
    QItemModelBuffer();

    void setModel(QAbstractItemModel* model);
    void setRoot(const QModelIndex& rootIndex);

    void mapRoleName(QByteArray roleName, int type);
    void mapRoleName(QByteArray roleName, QString attributeName, int type);

    BufferPtr buffer();

    QStringList attributeNames() const;
    AttributePtr attributeByName(QString nm) const;

private slots:

    void onModelReset();

    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
    QAbstractItemModel* m_model;
    QModelIndex m_rootIndex;

    struct RoleMapping {
        RoleMapping(QByteArray role, QString attr, int ty);

        QByteArray roleName;
        int cachedRole;
        QString attribute;
        int type;
        int byteSize;
    };

    QList<RoleMapping> m_mappings;

    BufferPtr m_buffer;
    QMap<QString, AttributePtr> m_attributes;
    int m_itemStride;

    QByteArray computeBufferData();

    void writeDataForIndex(const QModelIndex &index, int mappingCount, char *bufferPtr);
    bool validateRoles();
};

} // namespace Qt3D

#endif // QT3D_QITEMMODELBUFFER_H
