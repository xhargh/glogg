/*
 * Copyright (C) 2011 Nicolas Bonnefon and other contributors
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

#include <QSettings>
#include <QFile>

#include "log.h"
#include "recentfiles.h"
#include "iodevicesettings.h"
#include "iodevicesettings_deserialize.h"

// operator== for comparing string with RecentFileT, don't care about settings
bool operator==(const RecentFileT& lhs, const RecentFileT& rhs)
{
    return lhs.name_ == rhs.name_;
}

void RecentFiles::addRecent( const QString& text, std::shared_ptr<IoDeviceSettings> settings)
{
    // First prune non existent files
    QMutableListIterator<RecentFileT> i(recentFiles_);
    while ( i.hasNext() ) {
        auto n = i.next();
        auto fn = n.name_;
        auto s = n.settings_;
        if ( !s && !QFile::exists(fn)) {
            i.remove();
        }
    }

    // Remove any copy of the about to be added filename
    recentFiles_.removeAll( RecentFileT{text, settings} );

    // Add at the front
    recentFiles_.push_front( RecentFileT{text, settings} );

    // Trim the list if it's too long
    while ( recentFiles_.size() > MAX_NUMBER_OF_FILES )
        recentFiles_.pop_back();
}

RecentFilesT RecentFiles::recentFiles() const
{
    return recentFiles_;
}

//
// Persistable virtual functions implementation
//

void RecentFiles::saveToStorage( QSettings& settings ) const
{
    LOG(logDEBUG) << "RecentFiles::saveToStorage";

    settings.beginGroup( "RecentFiles" );
    settings.setValue( "version", RECENTFILES_VERSION );
    settings.remove( "filesHistory" );
    settings.beginWriteArray( "filesHistory" );
    for (int i = 0; i < recentFiles_.size(); ++i) {
        settings.setArrayIndex( i );
        settings.setValue( "name", recentFiles_.at( i ).name_ );
        if (recentFiles_.at( i ).settings_) {
            settings.setValue( "ioDeviceSettingsType", recentFiles_.at( i ).settings_->getType());
            settings.setValue( "ioDeviceSettings", recentFiles_.at( i ).settings_->Serialize());
        }
    }
    settings.endArray();
    settings.endGroup();
}

void RecentFiles::retrieveFromStorage( QSettings& settings )
{
    LOG(logDEBUG) << "RecentFiles::retrieveFromStorage";

    recentFiles_.clear();

    if ( settings.contains( "RecentFiles/version" ) ) {
        settings.beginGroup( "RecentFiles" );
        if ( settings.value( "version" ).toInt() == RECENTFILES_VERSION ) {
            int size = settings.beginReadArray( "filesHistory" );
            for (int i = 0; i < size; ++i) {
                settings.setArrayIndex(i);
                QString file = settings.value( "name" ).toString();
                auto s = IoDeviceSettingsHelper::Deserialize(settings.value("ioDeviceSettings").toString());
                recentFiles_.append( RecentFileT{file, s} );
            }
            settings.endArray();
        }
        else {
            LOG(logERROR) << "Unknown version of recent files, ignoring it...";
        }
        settings.endGroup();
    }
}

std::shared_ptr<IoDeviceSettings> RecentFiles::lookupIoDeviceSettings(const QString name) const {
    for (auto& r : recentFiles_) {
        if (name == r.name_) {
            return r.settings_;
        }
    }
    return nullptr;
}
