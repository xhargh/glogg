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

#ifndef SAVEDSTRINGS_H
#define SAVEDSTRINGS_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "persistable.h"

// Keeps track of the previously used strings and allows the application
// to retrieve them.
class SavedStrings final : public Persistable<SavedStrings, session_settings> {
  public:
    static const char* persistableName()
    {
        return "SavedStrings";
    }

    // Creates an empty set of saved strings
    SavedStrings();

    // Adds the passed string to the list of recently used strings
    void addRecent( const QString& text );

    // Returns a list of recent strings (newer first)
    QStringList recentStrings() const;

    void clear();

    // Operators for serialization
    // (only for migrating pre 0.8.2 settings, will be removed)
    friend QDataStream& operator<<( QDataStream& out, const SavedStrings& object );
    friend QDataStream& operator>>( QDataStream& in, SavedStrings& object );

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    static constexpr int SAVEDSTRINGS_VERSION = 1;
    static constexpr int maxNumberOfRecentStrings = 50;

    QStringList savedStrings_;
};

Q_DECLARE_METATYPE( SavedStrings )

#endif // SAVEDSTRINGS_H
