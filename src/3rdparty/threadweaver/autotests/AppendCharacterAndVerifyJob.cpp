/* -*- C++ -*-

   Helper class for unit tests.

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

#include "AppendCharacterAndVerifyJob.h"

AppendCharacterAndVerifyJob::AppendCharacterAndVerifyJob()
    : AppendCharacterJob()
{
}

void AppendCharacterAndVerifyJob::setValues(QChar c, QString *stringref, const QString &expected)
{
    AppendCharacterJob::setValues(c, stringref);
    m_expected = expected;
}

void AppendCharacterAndVerifyJob::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *)
{
    using namespace ThreadWeaver;
    QMutexLocker locker(&s_GlobalMutex);
    stringRef()->append(character());
    if (m_expected.mid(0, stringRef()->length()) != *stringRef()) {
        debug(3, "It broke!");
    }
    debug(3, "AppendCharacterJob::run: %c appended, result is %s.\n",
          character().toLatin1(), qPrintable(*stringRef()));
}
