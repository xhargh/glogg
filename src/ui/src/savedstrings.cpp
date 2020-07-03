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

// This file implements class SavedStrings

#include <QDataStream>
#include <QSettings>

#include "log.h"
#include "savedstrings.h"

SavedStrings::SavedStrings()
    : savedStrings_()
{
    qRegisterMetaTypeStreamOperators<SavedStrings>( "SavedStrings" );
}

void SavedStrings::addRecent( const QString& text )
{
    // We're not interested in blank lines
    if ( text.isEmpty() )
        return;

    // Remove any copy of the about to be added text
    savedStrings_.removeAll( text );

    // Add at the front
    savedStrings_.push_front( text );

    // Trim the list if it's too long
    while ( savedStrings_.size() > maxNumberOfRecentStrings )
        savedStrings_.pop_back();
}

QStringList SavedStrings::recentStrings() const
{
    return savedStrings_;
}

void SavedStrings::clear()
{
    savedStrings_.clear();
}

//
// Operators for serialization
//

QDataStream& operator<<( QDataStream& out, const SavedStrings& object )
{
    LOG( logDEBUG ) << "<<operator from SavedStrings";

    out << object.savedStrings_;

    return out;
}

QDataStream& operator>>( QDataStream& in, SavedStrings& object )
{
    LOG( logDEBUG ) << ">>operator from SavedStrings";

    in >> object.savedStrings_;

    return in;
}

//
// Persistable virtual functions implementation
//

void SavedStrings::saveToStorage( QSettings& settings ) const
{
    LOG( logDEBUG ) << "SavedStrings::saveToStorage";

    settings.beginGroup( "SavedStrings" );
    settings.setValue( "version", SAVEDSTRINGS_VERSION );
    settings.remove( "stringHistory" );
    settings.beginWriteArray( "stringHistory" );
    for ( int i = 0; i < savedStrings_.size(); ++i ) {
        settings.setArrayIndex( i );
        settings.setValue( "string", savedStrings_.at( i ) );
    }
    settings.endArray();
    settings.endGroup();
}

void SavedStrings::retrieveFromStorage( QSettings& settings )
{
    LOG( logDEBUG ) << "SavedStrings::retrieveFromStorage";

    savedStrings_.clear();

    if ( settings.contains( "SavedStrings/version" ) ) {
        settings.beginGroup( "SavedStrings" );
        if ( settings.value( "version" ) == SAVEDSTRINGS_VERSION ) {
            int size = settings.beginReadArray( "stringHistory" );
            for ( int i = 0; i < size; ++i ) {
                settings.setArrayIndex( i );
                QString str = settings.value( "string" ).toString();
                savedStrings_.append( str );
            }
            settings.endArray();
        }
        else {
            LOG( logERROR ) << "Unknown version of saved strings, ignoring it...";
        }
        settings.endGroup();
    }
    else {
        LOG( logWARNING ) << "Trying to import legacy (<=0.8.2) saved strings...";
        SavedStrings tmp_saved_strings = settings.value( "savedStrings" ).value<SavedStrings>();
        *this = tmp_saved_strings;
        LOG( logWARNING ) << "...imported strings: " << savedStrings_.count() << " elements";
        // Remove the old key once migration is done
        settings.remove( "savedStrings" );
        // And replace it with the new one
        saveToStorage( settings );
        settings.sync();
    }
}
