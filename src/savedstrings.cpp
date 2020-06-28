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

#include <QSettings>
#include <QDataStream>

#include "log.h"
#include "savedstrings.h"


const int SavedStrings::SAVEDSTRINGS_VERSION = 1;
const int SavedStrings::maxNumberOfRecentStrings = 50;


SavedStrings::SavedStrings() : savedStrings_()
{

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
    while (savedStrings_.size() > maxNumberOfRecentStrings)
        savedStrings_.pop_back();
}

QStringList SavedStrings::recentStrings() const
{
    return savedStrings_;
}


//
// Operators for serialization
//

QDataStream& operator<<( QDataStream& out, const SavedStrings& object )
{
    LOG(logDEBUG) << QString("<<operator from " + object.getClassName()).toStdString().c_str();

    out << object.savedStrings_;

    return out;
}

QDataStream& operator>>( QDataStream& in, SavedStrings& object )
{
    LOG(logDEBUG) << QString(">>operator from " + object.getClassName()).toStdString().c_str();

    in >> object.savedStrings_;

    return in;
}

//
// Persistable virtual functions implementation
//

void SavedStrings::saveToStorage(QSettings &settings) const
{
    LOG(logDEBUG) << QString(getClassName() + "::saveToStorage").toStdString().c_str();

    settings.beginGroup( getClassName() );
    // Remove everything in case the array is shorter than the previous one
    settings.remove("");
    settings.setValue( "version", getVersion() );
    settings.beginWriteArray( getClassName() + "History" );
    for (int i = 0; i < savedStrings_.size(); ++i) {
        settings.setArrayIndex( i );
        settings.setValue( "string", savedStrings_.at( i ) );
    }
    settings.endArray();
    settings.endGroup();
}

void SavedStrings::retrieveFromStorage(QSettings &settings)
{
    LOG(logDEBUG) << QString(getClassName() + "::retrieveFromStorage").toStdString().c_str();

    savedStrings_.clear();

    if ( settings.contains( getClassName() + "/version" ) ) {
        // Unserialise the "new style" stored history
        settings.beginGroup( getClassName() );
        if ( settings.value( "version" ) == getVersion() ) {
            int size = settings.beginReadArray( getClassName() + "History" );
            for (int i = 0; i < size; ++i) {
                settings.setArrayIndex(i);
                QString search = settings.value( "string" ).toString();
                savedStrings_.append( search );
            }
            settings.endArray();
        }
        else {
            LOG(logERROR) << "Unknown version of FilterSet, ignoring it...";
        }
        settings.endGroup();
    }
}

int SavedStrings::getVersion() const
{
    return SavedStrings::SAVEDSTRINGS_VERSION;
}
