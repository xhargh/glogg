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

#ifndef SAVEDCOMMANDS_H
#define SAVEDCOMMANDS_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "persistable.h"

// Keeps track of the previously used strings and allows the application
// to retrieve them.
class SavedCommands final : public Persistable<SavedCommands, session_settings> {
  public:
    static const char* persistableName()
    {
        return "SavedCommands";
    }

    // Creates an empty set of saved strings
    SavedCommands();

    // Adds the passed string to the list of recently used strings
    void addRecent( const QString& text );

    // Returns a list of recent strings (newer first)
    QStringList recentCommands() const;

    void clear();

    // Operators for serialization
    // (only for migrating pre 0.8.2 settings, will be removed)
    friend QDataStream& operator<<( QDataStream& out, const SavedCommands& object );
    friend QDataStream& operator>>( QDataStream& in, SavedCommands& object );

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    static constexpr int SAVEDCOMMANDS_VERSION = 1;
    static constexpr int maxNumberOfRecentCommands = 50;

    QStringList savedCommands_;
};

Q_DECLARE_METATYPE( SavedCommands )

#endif // SAVEDCOMMANDS_H
