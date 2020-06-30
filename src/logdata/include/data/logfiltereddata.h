/*
 * Copyright (C) 2009, 2010, 2011, 2012, 2017 Nicolas Bonnefon and other contributors
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

#ifndef LOGFILTEREDDATA_H
#define LOGFILTEREDDATA_H

#include <memory>

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QRegularExpression>

#include "abstractlogdata.h"
#include "logfiltereddataworker.h"
#include "marks.h"

class LogDataBase;
class Marks;
class QTimer;

// A list of matches found in a LogData, it stores all the matching lines,
// which can be accessed using the AbstractLogData interface, together with
// the original line number where they were found.
// Constructing such objet does not start the search.
// This object should be constructed by a LogData.
class LogFilteredData : public AbstractLogData {
  Q_OBJECT

  public:
    // Constructor used by LogData
    explicit LogFilteredData( const LogDataBase* logData );

    // Starts the async search, sending newDataAvailable() when new data found.
    // If a search is already in progress this function will block until
    // it is done, so the application should call interruptSearch() first.
    void runSearch(const QRegularExpression &regExp, LineNumber startLine, LineNumber endLine);
    // Shortcut for runSearch on all file
    void runSearch(const QRegularExpression &regExp);

    // Add to the existing search, starting at the line when the search was
    // last stopped. Used when the file on disk has been added too.
    void updateSearch(LineNumber startLine, LineNumber endLine);
    // Interrupt the running search if one is in progress.
    // Nothing is done if no search is in progress.
    void interruptSearch();
    // Clear the search and the list of results.
    void clearSearch();

    // Returns the line number in the original LogData where the element
    // 'index' was found.
    LineNumber getMatchingLineNumber( LineNumber index ) const;
    // Returns the line 'index' in filterd log data that matches
    // given original line number
    LineNumber getLineIndexNumber( LineNumber lineNumber ) const;

    // Returns the number of lines in the source log data
    LinesCount getNbTotalLines() const;
    // Returns the number of matches (independently of the visibility)
    LinesCount getNbMatches() const;
    // Returns the number of marks (independently of the visibility)
    LinesCount getNbMarks() const;

    LineType lineTypeByIndex( LineNumber index ) const;
    LineType lineTypeByLine( LineNumber lineNumber ) const;

    // Marks interface (delegated to a Marks object)

    // Add a mark at the given line, optionally identified by the given char
    // If a mark for this char already exist, the previous one is replaced.
    void addMark( LineNumber line, QChar mark = QChar() );
    // Get the (unique) mark identified by the passed char.
    LineNumber getMark( QChar mark ) const;
    // Get the first mark after the line passed
    OptionalLineNumber getMarkAfter( LineNumber line ) const;
    // Get the first mark before the line passed
    OptionalLineNumber getMarkBefore( LineNumber line ) const;
    // Delete the mark identified by the passed char.
    void deleteMark( QChar mark );
    // Delete the mark present on the passed line or do nothing if there is
    // none.
    void deleteMark( LineNumber line );
    // Toggle presence of the mark on the passed line.
    void toggleMark( LineNumber line, QChar mark = {} );
    // Completely clear the marks list.
    void clearMarks();
    // Get all marked lines
    QList<LineNumber> getMarks() const;

    // Changes what the AbstractLogData returns via its getXLines/getNbLines
    // API.
    enum class VisibilityFlags
    {
        None    = static_cast<LineType::Int>( LineTypeFlags::Plain ), //this is for internal use
        Matches = static_cast<LineType::Int>( LineTypeFlags::Match ),
        Marks   = static_cast<LineType::Int>( LineTypeFlags::Mark  ),
    };
    Q_ENUM( VisibilityFlags );
    Q_DECLARE_FLAGS( Visibility, VisibilityFlags )
    void setVisibility( Visibility visibility );

  signals:
    // Sent when the search has progressed, give the number of matches (so far)
    // and the percentage of completion
    void searchProgressed( LinesCount nbMatches, int progress, LineNumber initialLine );

  private slots:
    void handleSearchProgressed( LinesCount nbMatches, int progress, LineNumber initialLine );

  private:
    class FilteredItem;

    // Implementation of virtual functions
    QString doGetLineString( LineNumber line ) const override;
    QString doGetExpandedLineString( LineNumber line ) const override;
    std::vector<QString> doGetLines( LineNumber first, LinesCount number ) const override;
    std::vector<QString> doGetExpandedLines( LineNumber first, LinesCount number ) const override;
    std::vector<QString> doGetLines(LineNumber first, LinesCount number, const std::function<QString(LineNumber)>& lineGetter) const;
    LinesCount doGetNbLine() const override;
    LineLength doGetMaxLength() const override;
    LineLength doGetLineLength( LineNumber line ) const override;

    void doSetDisplayEncoding( const char* encoding ) override;
    QTextCodec* doGetDisplayEncoding() const override;

    void doAttachReader() const override;
    void doDetachReader() const override;

    // Insert new mark into filteredItemsCache_.
    void updateCacheWithMark( uint32_t index, LineNumber line );

    // Returns whether the line number passed is in our list of matching ones.
    bool isLineMatched( LineNumber lineNumber ) const;
    // Returns wheither the passed line has a mark on it.
    bool isLineMarked( LineNumber line ) const;

    // List of the matching line numbers
    SearchResultArray matching_lines_;

    const LogDataBase* sourceLogData_;

    QRegularExpression currentRegExp_;
    LineLength maxLength_;
    LineLength maxLengthMarks_;
    // Number of lines of the LogData that has been searched for:
    LinesCount nbLinesProcessed_;

    Visibility visibility_;

    // Cache used to combine Marks and Matches
    // when visibility_ == MarksAndMatches
    // (QVector store actual objects instead of pointers)
    mutable std::vector<FilteredItem> filteredItemsCache_;

    LogFilteredDataWorker workerThread_;
    Marks marks_;

    struct CachedSearchResult
    {
        SearchResultArray matching_lines;
        LineLength maxLength;
    };

    using SearchCacheKey = QPair<QRegularExpression, QPair<uint32_t, uint32_t>>;

    inline SearchCacheKey makeCacheKey( const QRegularExpression& regExp,
                                        LineNumber startLine, LineNumber endLine ) {
      return qMakePair( regExp, qMakePair( startLine.get(), endLine.get() ) );
    }

    QHash<SearchCacheKey, CachedSearchResult> searchResultsCache_;
    SearchCacheKey currentSearchKey_;

    inline LineNumber getExpectedSearchEnd( const SearchCacheKey& cacheKey ) const {
        return LineNumber(cacheKey.second.second);
    }

    // Utility functions
    LineNumber findLogDataLine( LineNumber lineNum ) const;
    LineNumber findFilteredLine( LineNumber lineNum ) const;

    void regenerateFilteredItemsCache() const;
    // start_index can be passed in as an optimization when finding the item.
    // It refers to the index of the singular arrays (Marks or SearchResultArray) where the item was inserted.
    void insertIntoFilteredItemsCache( size_t start_index, FilteredItem&& item );
    void insertIntoFilteredItemsCache( FilteredItem&& item );
    // Insert new matches into matching_lines_ and filteredItemsCache_
    void insertNewMatches( const SearchResultArray& new_matches );
    // remove_index can be passed in as an optimization when finding the item.
    // It refers to the index of the singular arrays (Marks or SearchResultArray) where the item was removed.
    void removeFromFilteredItemsCache( size_t remove_index, FilteredItem&& item );
    void removeFromFilteredItemsCache( FilteredItem&& item );
    void removeAllFromFilteredItemsCache( LineType type );

    // update maxLengthMarks_ when a Mark was removed.
    void updateMaxLengthMarks( LineNumber removed_line );
};

// A class representing a Mark or Match.
// Conceptually it should be a base class for Mark and MatchingLine,
// but we implement it this way for performance reason as we create plenty of
// those everytime we refresh the cache.
// Specifically it allows to store this in the cache by value instead
// of pointer (less small allocations and no RTTI).
class LogFilteredData::FilteredItem {
  public:
    // A default ctor seems to be necessary for QVector
    FilteredItem();
    FilteredItem( LineNumber lineNumber, LineType type )
        : lineNumber_{ lineNumber }
        , type_ { type }
    {}

    LineNumber lineNumber() const
    { return lineNumber_; }

    LineType type() const
    { return type_; }

    void add( LineType type )
    { type_ |= type; }

    // Returns the new type flags.
    LineType remove( LineType type )
    {
        type_ &= ~type;
        return type_;
    }

    bool operator <( const LogFilteredData::FilteredItem& other ) const
    { return lineNumber_ < other.lineNumber_; }

    bool operator <( const LineNumber& lineNumber ) const
    { return lineNumber_ < lineNumber; }

  private:
    LineNumber lineNumber_;
    LineType type_;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( LogFilteredData::Visibility )
Q_DECLARE_METATYPE( LogFilteredData::Visibility )

#endif
