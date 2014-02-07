/* -*- C++ -*-

   Base class for exceptions in ThreadWeaver.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005-2013 Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

#include <QString>

#include <threadweaver_export.h>

namespace ThreadWeaver
{

class THREADWEAVER_EXPORT Exception : public std::runtime_error
{
public:
    explicit Exception(const QString &message = QString());
    ~Exception() throw();
    QString message() const;

private:
    QString m_message;
};

class THREADWEAVER_EXPORT JobAborted : public Exception
{
public:
    explicit JobAborted(const QString &message = QString());
};

class THREADWEAVER_EXPORT JobFailed : public Exception
{
public:
    explicit JobFailed(const QString &message = QString());
};

}

#endif // EXCEPTION_H
