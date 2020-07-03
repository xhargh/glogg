/*
 * Copyright (C) 2009, 2010, 2011 Nicolas Bonnefon and other contributors
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

// This file implements class SavedCommands

#include <QDataStream>
#include <QSettings>
#include <QDebug>

#include "log.h"
#include "savedcommands.h"

SavedCommands::SavedCommands()
    : savedCommands_()
{
    qRegisterMetaTypeStreamOperators<SavedCommands>( "SavedCommands" );
}

void SavedCommands::addRecent( const QString& text )
{
    // We're not interested in blank lines
    if ( text.isEmpty() )
        return;

    // Remove any copy of the about to be added text
    savedCommands_.removeAll( text );

    // Add at the front
    savedCommands_.push_front( text );

    // Trim the list if it's too long
    while ( savedCommands_.size() > maxNumberOfRecentCommands )
        savedCommands_.pop_back();
}

QStringList SavedCommands::recentCommands() const
{
    return savedCommands_;
}

void SavedCommands::clear()
{
    savedCommands_.clear();
}

//
// Operators for serialization
//

QDataStream& operator<<( QDataStream& out, const SavedCommands& object )
{
    LOG( logDEBUG ) << "<<operator from SavedCommands";

    out << object.savedCommands_;

    return out;
}

QDataStream& operator>>( QDataStream& in, SavedCommands& object )
{
    LOG( logDEBUG ) << ">>operator from SavedCommands";

    in >> object.savedCommands_;

    return in;
}

//
// Persistable virtual functions implementation
//

void SavedCommands::saveToStorage( QSettings& settings ) const
{
    LOG( logDEBUG ) << "SavedCommands::saveToStorage";

    settings.beginGroup( "SavedCommands" );
    settings.setValue( "version", SAVEDCOMMANDS_VERSION );
    settings.remove( "commandHistory" );
    settings.beginWriteArray( "commandHistory" );
    for ( int i = 0; i < savedCommands_.size(); ++i ) {
        settings.setArrayIndex( i );
        settings.setValue( "string", savedCommands_.at( i ) );
    }
    settings.endArray();
    settings.endGroup();
}

void SavedCommands::retrieveFromStorage( QSettings& settings )
{
    LOG( logDEBUG ) << "SavedCommands::retrieveFromStorage";

    savedCommands_.clear();

    if ( settings.contains( "SavedCommands/version" ) ) {
        settings.beginGroup( "SavedCommands" );
        if ( settings.value( "version" ) == SAVEDCOMMANDS_VERSION ) {
            int size = settings.beginReadArray( "commandHistory" );
            for ( int i = 0; i < size; ++i ) {
                settings.setArrayIndex( i );
                QString str = settings.value( "string" ).toString();
                savedCommands_.append( str );
            }
            settings.endArray();
        }
        else {
            LOG( logERROR ) << "Unknown version of saved commands, ignoring it...";
        }
        settings.endGroup();
    }
}
