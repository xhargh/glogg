/*
 * Copyright (C) 2009, 2010, 2011, 2013, 2015 Nicolas Bonnefon and other
 * contributors
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

/*
 * Copyright (C) 2016 -- 2019 Anton Filimonov and other contributors
 *
 * This file is part of klogg.
 *
 * klogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * klogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klogg.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFont>
#include <QSettings>

#include "persistable.h"

// Type of regexp to use for searches
enum SearchRegexpType {
    ExtendedRegexp,
    Wildcard,
    FixedString,
};

static constexpr QLatin1String DarkStyleKey = QLatin1String( "Dark", 4 );

// Configuration class containing everything in the "Settings" dialog
class Configuration final : public Persistable<Configuration> {
  public:
    static const char* persistableName()
    {
        return "Configuration";
    }
    Configuration();

    // Accesses the main font used for display
    QFont mainFont() const;
    void setMainFont( QFont newFont );

    // Accesses the regexp types
    SearchRegexpType mainRegexpType() const
    {
        return mainRegexpType_;
    }
    SearchRegexpType quickfindRegexpType() const
    {
        return quickfindRegexpType_;
    }
    bool isQuickfindIncremental() const
    {
        return quickfindIncremental_;
    }
    void setMainRegexpType( SearchRegexpType type )
    {
        mainRegexpType_ = type;
    }
    void setQuickfindRegexpType( SearchRegexpType type )
    {
        quickfindRegexpType_ = type;
    }
    void setQuickfindIncremental( bool is_incremental )
    {
        quickfindIncremental_ = is_incremental;
    }

    // "Advanced" settings
    bool anyFileWatchEnabled() const
    {
        return nativeFileWatchEnabled() || pollingEnabled();
    }

    bool nativeFileWatchEnabled() const
    {
        return nativeFileWatchEnabled_;
    }
    void setNativeFileWatchEnabled( bool enabled )
    {
        nativeFileWatchEnabled_ = enabled;
    }
    bool pollingEnabled() const
    {
        return pollingEnabled_;
    }
    void setPollingEnabled( bool enabled )
    {
        pollingEnabled_ = enabled;
    }
    int pollIntervalMs() const
    {
        return pollIntervalMs_;
    }
    void setPollIntervalMs( int interval )
    {
        pollIntervalMs_ = interval;
    }

    bool fastModificationDetection() const
    {
        return fastModificationDetection_;
    }

    void setFastModificationDetection( bool fastDetection )
    {
        fastModificationDetection_ = fastDetection;
    }

    bool loadLastSession() const
    {
        return loadLastSession_;
    }
    void setLoadLastSession( bool enabled )
    {
        loadLastSession_ = enabled;
    }
    bool followFileOnLoad() const
    {
        return followFileOnLoad_;
    }
    void setFollowFileOnLoad( bool enabled )
    {
        followFileOnLoad_ = enabled;
    }
    bool allowMultipleWindows() const
    {
        return allowMultipleWindows_;
    }
    void setAllowMultipleWindows( bool enabled )
    {
        allowMultipleWindows_ = enabled;
    }

    // perf settings
    bool useParallelSearch() const
    {
        return useParallelSearch_;
    }
    void setUseParallelSearch( bool enabled )
    {
        useParallelSearch_ = enabled;
    }
    bool useSearchResultsCache() const
    {
        return useSearchResultsCache_;
    }
    void setUseSearchResultsCache( bool enabled )
    {
        useSearchResultsCache_ = enabled;
    }
    int searchResultsCacheLines() const
    {
        return searchResultsCacheLines_;
    }
    void setSearchResultsCacheLines( int lines )
    {
        searchResultsCacheLines_ = lines;
    }
    int indexReadBufferSizeMb() const
    {
        return indexReadBufferSizeMb_;
    }
    void setIndexReadBufferSizeMb( int bufferSizeMb )
    {
        indexReadBufferSizeMb_ = bufferSizeMb;
    }
    int searchReadBufferSizeLines() const
    {
        return searchReadBufferSizeLines_;
    }
    void setSearchReadBufferSizeLines( int lines )
    {
        searchReadBufferSizeLines_ = lines;
    }
    int searchThreadPoolSize() const
    {
        return searchThreadPoolSize_;
    }
    void setSearchThreadPoolSize( int threads )
    {
        searchThreadPoolSize_ = threads;
    }
    bool keepFileClosed() const
    {
        return keepFileClosed_;
    }
    void setKeepFileClosed( bool shouldKeepClosed )
    {
        keepFileClosed_ = shouldKeepClosed;
    }

    bool useLineEndingCache() const
    {
        return useLineEndingCache_;
    }
    void setUseLineEndingCache( bool useLineEndingCache )
    {
        useLineEndingCache_ = useLineEndingCache;
    }

    // Accessors
    bool versionCheckingEnabled() const
    {
        return enableVersionChecking_;
    }
    void setVersionCheckingEnabled( bool enabled )
    {
        enableVersionChecking_ = enabled;
    }

    // View settings
    bool isOverviewVisible() const
    {
        return overviewVisible_;
    }
    void setOverviewVisible( bool isVisible )
    {
        overviewVisible_ = isVisible;
    }
    bool mainLineNumbersVisible() const
    {
        return lineNumbersVisibleInMain_;
    }
    bool filteredLineNumbersVisible() const
    {
        return lineNumbersVisibleInFiltered_;
    }
    bool minimizeToTray() const
    {
        return minimizeToTray_;
    }
    QString style() const
    {
        return style_;
    }
    void setMainLineNumbersVisible( bool lineNumbersVisible )
    {
        lineNumbersVisibleInMain_ = lineNumbersVisible;
    }
    void setFilteredLineNumbersVisible( bool lineNumbersVisible )
    {
        lineNumbersVisibleInFiltered_ = lineNumbersVisible;
    }
    void setMinimizeToTray( bool minimizeToTray )
    {
        minimizeToTray_ = minimizeToTray;
    }
    void setStyle( const QString& style )
    {
        style_ = style;
    }

    bool enableLogging() const
    {
        return enableLogging_;
    }
    uint8_t loggingLevel() const
    {
        return loggingLevel_;
    }

    void setEnableLogging( bool enableLogging )
    {
        enableLogging_ = enableLogging;
    }
    void setLoggingLevel( uint8_t level )
    {
        loggingLevel_ = level;
    }

    // Default settings for new views
    bool isSearchAutoRefreshDefault() const
    {
        return searchAutoRefresh_;
    }
    void setSearchAutoRefreshDefault( bool auto_refresh )
    {
        searchAutoRefresh_ = auto_refresh;
    }
    bool isSearchIgnoreCaseDefault() const
    {
        return searchIgnoreCase_;
    }
    void setSearchIgnoreCaseDefault( bool ignore_case )
    {
        searchIgnoreCase_ = ignore_case;
    }
    QList<int> splitterSizes() const
    {
        return splitterSizes_;
    }
    void setSplitterSizes( QList<int> sizes )
    {
        splitterSizes_ = std::move( sizes );
    }

    bool extractArchives() const
    {
        return extractArchives_;
    }
    void setExtractArchives( bool extract )
    {
        extractArchives_ = extract;
    }

    bool extractArchivesAlways() const
    {
        return extractArchivesAlways_;
    }
    void setExtractArchivesAlways( bool extract )
    {
        extractArchivesAlways_ = extract;
    }

    bool verifySslPeers() const
    {
        return verifySslPeers_;
    }
    void setVerifySslPeers( bool verify )
    {
        verifySslPeers_ = verify;
    }

    bool forceFontAntialiasing() const
    {
        return forceFontAntialiasing_;
    }
    void setForceFontAntialiasing( bool force )
    {
        forceFontAntialiasing_ = force;
    }

    bool enableQtHighDpi() const
    {
        return enableQtHighDpi_;
    }
    void setEnableQtHighDpi( bool enable )
    {
        enableQtHighDpi_ = enable;
    }

    int scaleFactorRounding() const
    {
        return scaleFactorRounding_;
    }
    void setScaleFactorRounding( int rounding )
    {
        scaleFactorRounding_ = rounding;
    }

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    // Configuration settings
    QFont mainFont_ = { "monaco", 10 };
    SearchRegexpType mainRegexpType_ = ExtendedRegexp;
    SearchRegexpType quickfindRegexpType_ = FixedString;
    bool quickfindIncremental_ = true;

    bool nativeFileWatchEnabled_ = true;
#ifdef Q_OS_WIN
    bool pollingEnabled_ = true;
#else
    bool pollingEnabled_ = false;
#endif

    int pollIntervalMs_ = 2000;

    bool fastModificationDetection_ = false;

    bool loadLastSession_ = true;
    bool followFileOnLoad_ = false;
    bool allowMultipleWindows_ = false;

    // View settings
    bool overviewVisible_ = true;
    bool lineNumbersVisibleInMain_ = false;
    bool lineNumbersVisibleInFiltered_ = true;
    bool minimizeToTray_ = false;
    QString style_;

    // Default settings for new views
    bool searchAutoRefresh_ = false;
    bool searchIgnoreCase_ = false;
    QList<int> splitterSizes_;

    // Performance settings
    bool useSearchResultsCache_ = true;
    int searchResultsCacheLines_ = 1000000;
    bool useParallelSearch_ = true;
    int indexReadBufferSizeMb_ = 16;
    int searchReadBufferSizeLines_ = 5000;
    int searchThreadPoolSize_ = 0;
    bool keepFileClosed_ = false;

    bool useLineEndingCache_ = true;

    bool enableLogging_ = false;
    uint8_t loggingLevel_ = 4;

    bool enableVersionChecking_ = true;

    bool extractArchives_ = true;
    bool extractArchivesAlways_ = false;

    bool verifySslPeers_ = true;

    bool forceFontAntialiasing_ = false;
    bool enableQtHighDpi_ = true;

    int scaleFactorRounding_ = 1;
};

#endif
