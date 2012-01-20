/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgl3dsscenehandler.h"
#include "qgl3dsscene.h"

#include <lib3ds/file.h>
#include <lib3ds/io.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>

#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

struct IODevice3ds
{
    QIODevice *dev;
    bool errorState;

};

extern "C" {
    static Lib3dsBool qgl3ds_fileio_error_func(void *self)
    {
        IODevice3ds *io3d = (IODevice3ds*)self;
        return io3d->errorState;
    }

    static long qgl3ds_fileio_seek_func(void *self, long offset, Lib3dsIoSeek origin)
    {
        IODevice3ds *io3d = (IODevice3ds*)self;
        // cannot deal with sockets right now
        Q_ASSERT(!io3d->dev->isSequential());
        switch (origin)
        {
        case LIB3DS_SEEK_SET:
            io3d->errorState = io3d->dev->seek(offset);
            break;
        case LIB3DS_SEEK_CUR:
            io3d->errorState = io3d->dev->seek(io3d->dev->pos() + offset);
            break;
        case LIB3DS_SEEK_END:
            io3d->errorState = io3d->dev->seek(io3d->dev->size() + offset);
            break;
        default:
            Q_ASSERT(0);
            return(0);
        }
        if (io3d->errorState)
            return -1;
        return io3d->dev->pos();
    }

    static long qgl3ds_fileio_tell_func(void *self)
    {
        IODevice3ds *io3d = (IODevice3ds*)self;
        return io3d->dev->pos();
    }

    static size_t qgl3ds_fileio_read_func(void *self, void *buffer, size_t size)
    {
        IODevice3ds *io3d = (IODevice3ds*)self;
        qint64 result = io3d->dev->read((char*)buffer, size);
        io3d->errorState = (result == -1);
        if (io3d->errorState)
            qWarning("3ds io error: %s\n", io3d->dev->errorString().toLocal8Bit().constData());
        return result;
    }

    static size_t qgl3ds_fileio_write_func(void *self, const void *buffer, size_t size)
    {
        IODevice3ds *io3d = (IODevice3ds*)self;
        qint64 result = io3d->dev->write((char*)buffer, size);
        io3d->errorState = (result == -1);
        if (io3d->errorState)
            qWarning("3ds io error: %s\n", io3d->dev->errorString().toLocal8Bit().constData());
        return result;
    }
}

static Lib3dsFile *qgl3ds_lib3ds_file_load(QIODevice *iod)
{
    Lib3dsFile *file;
    Lib3dsIo *io;
    Q_ASSERT(iod->isOpen() && iod->isReadable());
    file = lib3ds_file_new();
    if (!file) {
        iod->close();
        return(0);
    }
    IODevice3ds io3d;
    io3d.dev = iod;
    io3d.errorState = false;
    io = lib3ds_io_new(
            &io3d,
            qgl3ds_fileio_error_func,
            qgl3ds_fileio_seek_func,
            qgl3ds_fileio_tell_func,
            qgl3ds_fileio_read_func,
            qgl3ds_fileio_write_func
            );
    if (!io) {
        lib3ds_file_free(file);
        iod->close();
        return(0);
    }
    if (!lib3ds_file_read(file, io)) {
        lib3ds_file_free(file);
        iod->close();
        return(0);
    }
    lib3ds_io_free(io);
    iod->close();
    return(file);
}

QGL3dsSceneHandler::QGL3dsSceneHandler()
    : m_options(0)
{
}

QGL3dsSceneHandler::~QGL3dsSceneHandler()
{
    // nothing to do
}

void QGL3dsSceneHandler::decodeOptions(const QString &options)
{
    static const char *validOptions[] = {
        "NativeIndices",
        "CorrectNormals",
        "CorrectAcute",
        "ForceSmooth",
        "ForceFaceted",
        "ShowWarnings"
    };
    static int optionKeys[] = {
        QGL::NativeIndices,
        QGL::CorrectNormals,
        QGL::CorrectAcute,
        QGL::ForceSmooth,
        QGL::ForceFaceted,
        QGL::ShowWarnings,
        -1
    };

    // format: "mesh=option mesh=option option option"
    // mesh: string name of a mesh in the file, with no spaces
    // standalone option applies to the whole file - all meshes
    // option: NativeIndices | CorrectNormals | CorrectAcute etc
    QStringList opList = options.split(QLatin1Char(' '), QString::SkipEmptyParts);
    for (int i = 0; i < opList.count(); ++i)
    {
        QString op = opList.at(i);
        QString mdl;
        if (op.contains(QLatin1Char('=')))
        {
            QStringList sl = op.split(QLatin1Char('='), QString::SkipEmptyParts);
            mdl = sl[0];
            op = sl[1];
        }
        int k = 0;
        for ( ; optionKeys[k] != -1; ++k)
            if (op == QLatin1String(validOptions[k]))
                break;
        if (optionKeys[k] != -1) // found
        {
            QGL::ModelOptions o = static_cast<QGL::ModelOptions>(optionKeys[k]);
            if (!mdl.isEmpty())
                setMeshOptions(o, mdl);
            else
                setOptions(o);
        }
        else
        {
            qWarning("Bad option: \"%s\" in %s", qPrintable(op),
                     qPrintable(options));
        }
    }
}

QGLAbstractScene *QGL3dsSceneHandler::read()
{
    Lib3dsFile *file = qgl3ds_lib3ds_file_load(device());
    Q_CHECK_PTR(file);  // wtf?

    if (!file->nodes)
    {
        Lib3dsMesh *mesh;
        Lib3dsNode *node;
        for (mesh = file->meshes; mesh; mesh = mesh->next)
        {
            if (mesh->faces && mesh->points)
            {
                node = lib3ds_node_new_object();
                qstrcpy(node->name, mesh->name);
                node->parent_id = LIB3DS_NO_PARENT;
                lib3ds_file_insert_node(file, node);
            }
        }
    }

    lib3ds_file_eval(file, 0.0f);

    QGL3dsScene *scene = new QGL3dsScene(file, this);
    return scene;
}

QT_END_NAMESPACE
