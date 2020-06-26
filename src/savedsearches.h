/*
 * Copyright (C) 2009, 2011 Nicolas Bonnefon and other contributors
 *
 * This file is part of glogg.
 *
 * glogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glogg.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SAVEDSEARCHES_H
#define SAVEDSEARCHES_H

#include <QString>
#include <QStringList>
#include <QMetaType>

#include "savedstrings.h"

// Keeps track of the previously used searches and allows the application
// to retrieve them.
class SavedSearches : public SavedStrings
{
  public:
    // Creates an empty set of saved searches
    SavedSearches() : SavedStrings()
    {
        qRegisterMetaTypeStreamOperators<SavedSearches>( "SavedSearches" );
    }

private:
    QString getClassName() const override {
        return "SavedSearches";
    }

};

Q_DECLARE_METATYPE(SavedSearches)

#endif
