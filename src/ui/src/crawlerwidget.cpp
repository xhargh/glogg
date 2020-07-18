/*
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015 Nicolas Bonnefon and other contributors
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
 *truncateFile
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

// This file implements the CrawlerWidget class.
// It is responsible for creating and managing the two views and all
// the UI elements.  It implements the connection between the UI elements.
// It also interacts with the sets of data (full and filtered).

#include "log.h"

#include <cassert>
#include <chrono>

#include <QApplication>
#include <QCompleter>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QJsonDocument>
#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QFileDialog>
#include <QStandardPaths>

#include "crawlerwidget.h"
#include "cmdbutton.h"
#include "configuration.h"
#include "infoline.h"
#include "overview.h"
#include "quickfindpattern.h"
#include "quickfindwidget.h"
#include "savedsearches.h"
#include "savedcommands.h"

// Palette for error signaling (yellow background)
const QPalette CrawlerWidget::errorPalette( QColor( "yellow" ) );

// Implementation of the view context for the CrawlerWidget
class CrawlerWidgetContext : public ViewContextInterface {
  public:
    // Construct from the stored string representation
    explicit CrawlerWidgetContext( const QString& string );
    // Construct from the value passsed
    CrawlerWidgetContext( QList<int> sizes, bool ignore_case, bool auto_refresh, bool follow_file,
                          bool use_regexp, QList<LineNumber> markedLines,
                          std::vector<QString> btnCmds )
        : sizes_( sizes )
        , ignore_case_( ignore_case )
        , auto_refresh_( auto_refresh )
        , follow_file_( follow_file )
        , use_regexp_( use_regexp )
        , btnCmds_(btnCmds)
    {
        std::transform( markedLines.begin(), markedLines.end(), std::back_inserter( marks_ ),
                        []( const auto& m ) { return m.get(); } );
    }

    // Implementation of the ViewContextInterface function
    QString toString() const override;

    // Access the Qt sizes array for the QSplitter
    QList<int> sizes() const
    {
        return sizes_;
    }

    bool ignoreCase() const
    {
        return ignore_case_;
    }
    bool autoRefresh() const
    {
        return auto_refresh_;
    }
    bool followFile() const
    {
        return follow_file_;
    }
    bool useRegexp() const
    {
        return use_regexp_;
    }

    QList<LineNumber::UnderlyingType> marks() const
    {
        return marks_;
    }

    std::vector<QString> btnCommands() const
    {
        return btnCmds_;
    }

  private:
    void loadFromString( const QString& string );
    void loadFromJson( const QString& json );

  private:
    QList<int> sizes_;

    bool ignore_case_;
    bool auto_refresh_;
    bool follow_file_;
    bool use_regexp_;

    QList<LineNumber::UnderlyingType> marks_;
    std::vector<QString> btnCmds_;
};

// Constructor only does trivial construction. The real work is done once
// the data is attached.
CrawlerWidget::CrawlerWidget( QWidget* parent )
    : QSplitter( parent )
{
}

// The top line is first one on the main display
LineNumber CrawlerWidget::getTopLine() const
{
    return logMainView->getTopLine();
}

QString CrawlerWidget::getSelectedText() const
{
    if ( filteredView->hasFocus() )
        return filteredView->getSelection();
    else
        return logMainView->getSelection();
}

bool CrawlerWidget::isPartialSelection() const
{
    if ( filteredView->hasFocus() )
        return filteredView->isPartialSelection();
    else
        return logMainView->isPartialSelection();
}

void CrawlerWidget::selectAll()
{
    activeView()->selectAll();
}

absl::optional<int> CrawlerWidget::encodingMib() const
{
    return encodingMib_;
}

bool CrawlerWidget::isFollowEnabled() const
{
    return logMainView->isFollowEnabled();
}

QString CrawlerWidget::encodingText() const
{
    return encoding_text_;
}

// Return a pointer to the view in which we should do the QuickFind
SearchableWidgetInterface* CrawlerWidget::doGetActiveSearchable() const
{
    return activeView();
}

// Return all the searchable widgets (views)
std::vector<QObject*> CrawlerWidget::doGetAllSearchables() const
{
    std::vector<QObject*> searchables = { logMainView, filteredView };

    return searchables;
}

// Update the state of the parent
void CrawlerWidget::doSendAllStateSignals()
{
    emit updateLineNumber( currentLineNumber_ );
    if ( !loadingInProgress_ )
        emit loadingFinished( LoadingStatus::Successful );
}

void CrawlerWidget::keyPressEvent( QKeyEvent* keyEvent )
{
    keyEvent->accept();
    const auto noModifier = keyEvent->modifiers() == Qt::NoModifier;

#if defined(Q_OS_MACOS)
    const Qt::KeyboardModifier controlModifier = Qt::MetaModifier;
#else
    const Qt::KeyboardModifier controlModifier = Qt::ControlModifier; // qqq - not needed since Alt+number works on linux and windows
#endif

    if ( keyEvent->key() == Qt::Key_V && noModifier ) {
        visibilityBox->setCurrentIndex( ( visibilityBox->currentIndex() + 1 )
                                        % visibilityBox->count() );
    }
    else if (keyEvent->modifiers() == controlModifier) {
        if (logData_->isWritable())
        {
            switch (keyEvent->key()) {

            case Qt::Key_1:
                qInfo() << "Key 1";
                emit cmdBtns[0]->clicked();
                break;
            case Qt::Key_2:
                qInfo() << "Key 2";
                emit cmdBtns[1]->clicked();
                break;
            case Qt::Key_3:
                qInfo() << "Key 3";
                emit cmdBtns[2]->clicked();
                break;
            case Qt::Key_4:
                qInfo() << "Key 4";
                emit cmdBtns[3]->clicked();
                break;
            case Qt::Key_5:
                qInfo() << "Key 5";
                emit cmdBtns[4]->clicked();
                break;
            case Qt::Key_6:
                qInfo() << "Key 6";
                emit cmdBtns[5]->clicked();
                break;
            case Qt::Key_7:
                qInfo() << "Key 7";
                emit cmdBtns[6]->clicked();
                break;
            case Qt::Key_8:
                qInfo() << "Key 8";
                emit cmdBtns[7]->clicked();
                break;
            case Qt::Key_9:
                qInfo() << "Key 9";
                emit cmdBtns[8]->clicked();
                break;
            case Qt::Key_0:
                qInfo() << "Key 0";
                emit cmdBtns[9]->clicked();
                break;
            }
        }
    }
    else {
        switch ( keyEvent->key() ) {
        case Qt::Key_Plus:
            changeTopViewSize( 1 );
            break;
        case Qt::Key_Minus:
            changeTopViewSize( -1 );
            break;
        default:
            keyEvent->ignore();
            QSplitter::keyPressEvent( keyEvent );
            break;
        }
    }
}

//
// Public slots
//

void CrawlerWidget::stopLoading()
{
    logFilteredData_->interruptSearch();
    logData_->interruptLoading();
}

void CrawlerWidget::reload()
{
    searchState_.resetState();
    logFilteredData_->clearSearch();
    logFilteredData_->clearMarks();
    filteredView->updateData();
    printSearchInfoMessage();

    logData_->reload();

    // A reload is considered as a first load,
    // this is to prevent the "new data" icon to be triggered.
    firstLoadDone_ = false;
}

void CrawlerWidget::clearLog()
{
    logData_->clearLog();
}

void CrawlerWidget::setEncoding( absl::optional<int> mib )
{
    encodingMib_ = std::move( mib );
    updateEncoding();

    update();
}

void CrawlerWidget::focusSearchEdit()
{
    searchLineEdit->setFocus( Qt::ShortcutFocusReason );
}

//
// Protected functions
//
void CrawlerWidget::doSetData( std::shared_ptr<LogDataBase> log_data,
                               std::shared_ptr<LogFilteredData> filtered_data )
{
    logData_ = log_data.get();
    logFilteredData_ = filtered_data.get();
}

void CrawlerWidget::doSetQuickFindPattern( std::shared_ptr<QuickFindPattern> qfp )
{
    quickFindPattern_ = qfp;
}

void CrawlerWidget::doSetSavedSearches( SavedSearches* saved_searches )
{
    savedSearches_ = saved_searches;
}

void CrawlerWidget::doSetSavedCommands( SavedCommands* saved_commands )
{
    savedCommands_ = saved_commands;
}

void CrawlerWidget::doFinishSetup() {
    setup();
}

void CrawlerWidget::doSetViewContext( const QString& view_context )
{
    LOG( logDEBUG ) << "CrawlerWidget::doSetViewContext: " << view_context.toLocal8Bit().data();

    const auto context = CrawlerWidgetContext{ view_context };
    LOG( logDEBUG ) << "CrawlerWidget::doSetViewContext" << __LINE__;

    setSizes( context.sizes() );
    LOG( logDEBUG ) << "CrawlerWidget::doSetViewContext" << __LINE__ << " " << (uint32_t)(uintptr_t)matchCaseButton << " " << context.ignoreCase();

    matchCaseButton->setChecked( !context.ignoreCase() );
    LOG( logDEBUG ) << "CrawlerWidget::doSetViewContext" << __LINE__ << " " << (uint32_t)(uintptr_t)useRegexpButton << context.useRegexp();
    useRegexpButton->setChecked( context.useRegexp() );

    searchRefreshButton->setChecked( context.autoRefresh() );
    // Manually call the handler as it is not called when changing the state programmatically
    searchRefreshChangedHandler( context.autoRefresh() );

    logMainView->followSet( context.followFile() );

    if (logData_->isWritable()) {
        std::vector<QString> btnCommands = context.btnCommands();
        for(size_t i=0 ; i < btnCommands.size() ; i++) {
            cmdBtns[i]->setCmdLine(btnCommands.at(i));
        }
    }

    const auto savedMarks = context.marks();
    std::transform( savedMarks.begin(), savedMarks.end(), std::back_inserter( savedMarkedLines_ ),
                    []( const auto& l ) { return LineNumber( l ); } );
}

std::shared_ptr<const ViewContextInterface> CrawlerWidget::doGetViewContext() const
{
    std::vector<QString> btnCmds;
    for (const CmdButton *cb: cmdBtns) {
        btnCmds.emplace_back(cb->getCmdLine());
    }
    auto context = std::make_shared<const CrawlerWidgetContext>(
        sizes(), ( !matchCaseButton->isChecked() ), searchRefreshButton->isChecked(),
        logMainView->isFollowEnabled(), useRegexpButton->isChecked(),
        logFilteredData_->getMarks(),
        btnCmds );

    return static_cast<std::shared_ptr<const ViewContextInterface>>( context );
}

//
// Slots
//


void CrawlerWidget::executeBtnCommand(QString cmd) {
    qInfo() << __func__ << " " << cmd;
    logData_->write(cmd);
}

void CrawlerWidget::executeCommand()
{
    auto cmd = cmdEntryBox->currentText();

    auto& commands = SavedCommands::getSynced();
    savedCommands_->addRecent( cmd );
    commands.save();

    // Update the EntryBox (history)
    updateCommandCombo();
    // Call the private function to do the search
    qInfo() << "execute command: " << cmd;
    logData_->write(cmd);
}


void CrawlerWidget::exportLog()
{
    qInfo() << __func__;
    QString filename =
            QFileDialog::getSaveFileName(
                this,
                tr("Export File"),
                QStandardPaths::writableLocation(
                    QStandardPaths::DocumentsLocation)
                    + "/"
                    + logData_->getLastModifiedDate().toString(Qt::ISODate).replace(":", "").replace("-","")
                    + "_"
                    + "log"
                    + ".txt",
                    tr("Log Files (*.log *.txt)"
                )
            );

    if (!filename.isEmpty()) {
        QFile fOut(filename);
        if (fOut.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream s(&fOut);
            unsigned int numLines = logData_->getNbLine().get();
            for (unsigned int i = 0; i < numLines; i++) {
                s << logData_->getLineString(LineNumber(i)) << Qt::endl;
            }
        } else {
            qWarning() << __func__ << " unable to save to file " << filename;
        }
        fOut.close();
    }
}

void CrawlerWidget::startNewSearch()
{
    // Record the search line in the recent list
    // (reload the list first in case another glogg changed it)
    auto& searches = SavedSearches::getSynced();
    savedSearches_->addRecent( searchLineEdit->currentText() );
    searches.save();

    // Update the SearchLine (history)
    updateSearchCombo();
    // Call the private function to do the search
    replaceCurrentSearch( searchLineEdit->currentText() );
}

void CrawlerWidget::stopSearch()
{
    logFilteredData_->interruptSearch();
    searchState_.stopSearch();
    printSearchInfoMessage();
}

// When receiving the 'newDataAvailable' signal from LogFilteredData
void CrawlerWidget::updateFilteredView( LinesCount nbMatches, int progress,
                                        LineNumber initialPosition )
{
    LOG( logDEBUG ) << "updateFilteredView received.";

    searchInfoLine->show();

    if ( progress == 100 ) {
        // Searching done
        printSearchInfoMessage( nbMatches );
        searchInfoLine->hideGauge();
        // De-activate the stop button
        stopButton->setEnabled( false );
        stopButton->hide();
        searchButton->show();
    }
    else {
        // Search in progress
        // We ignore 0% and 100% to avoid a flash when the search is very short
        if ( progress > 0 ) {
            searchInfoLine->setText( tr( "Search in progress (%1 %)... %2 match%3 found so far." )
                                         .arg( QString::number( progress ),
                                               QString::number( nbMatches.get() ),
                                               QLatin1String( nbMatches.get() > 1 ? "es" : "" ) ) );

            searchInfoLine->displayGauge( progress );
        }
    }

    static auto lastUpdateTime = std::chrono::high_resolution_clock::now();

    const auto currentUpdateTime = std::chrono::high_resolution_clock::now();
    const auto timeSinceLastUpdate = currentUpdateTime - lastUpdateTime;
    if ( progress > 0 && progress < 100
         && timeSinceLastUpdate < std::chrono::milliseconds( 250 ) ) {
        LOG( logDEBUG ) << "updateFilteredView skipped";
        return;
    }
    lastUpdateTime = currentUpdateTime;

    // If more (or less, e.g. come back to 0) matches have been found
    if ( nbMatches != nbMatches_ ) {
        nbMatches_ = nbMatches;

        // Recompute the content of the filtered window.
        filteredView->updateData();

        // Update the match overview
        overview_.updateData( logData_->getNbLine() );

        // New data found icon
        if ( initialPosition > 0_lnum ) {
            changeDataStatus( DataStatus::NEW_FILTERED_DATA );
        }

        // Also update the top window for the coloured bullets.
        update();
    }

    // Try to restore the filtered window selection close to where it was
    // only for full searches to avoid disconnecting follow mode!
    if ( ( progress == 100 ) && ( initialPosition == 0_lnum ) && ( !isFollowEnabled() ) ) {
        const auto currenLineIndex = logFilteredData_->getLineIndexNumber( currentLineNumber_ );
        LOG( logDEBUG ) << "updateFilteredView: restoring selection: "
                        << " absolute line number (0based) " << currentLineNumber_ << " index "
                        << currenLineIndex;
        filteredView->selectAndDisplayLine( currenLineIndex );
        filteredView->setSearchLimits( searchStartLine_, searchEndLine_ );
    }
}

void CrawlerWidget::jumpToMatchingLine( LineNumber filteredLineNb )
{
    const auto mainViewLine = logFilteredData_->getMatchingLineNumber( filteredLineNb );
    logMainView->selectAndDisplayLine( mainViewLine ); // FIXME: should be done with a signal.
}

void CrawlerWidget::updateLineNumberHandler( LineNumber line )
{
    currentLineNumber_ = line;
    emit updateLineNumber( line );
}

void CrawlerWidget::markLinesFromMain( const std::vector<LineNumber>& lines )
{
    for ( const auto& line : lines ) {
        if ( line < logData_->getNbLine() ) {
            logFilteredData_->toggleMark( line );
        }
    }

    // Recompute the content of both window.
    filteredView->updateData();
    logMainView->updateData();

    // Update the match overview
    overview_.updateData( logData_->getNbLine() );

    // Also update the top window for the coloured bullets.
    update();
}

void CrawlerWidget::markLinesFromFiltered( const std::vector<LineNumber>& lines )
{
    for ( const auto& line : lines ) {
        if ( line < logData_->getNbLine() ) {
            const auto line_in_file = logFilteredData_->getMatchingLineNumber( line );
            logFilteredData_->toggleMark( line_in_file );
        }
    }

    // Recompute the content of both window.
    filteredView->updateData();
    logMainView->updateData();

    // Update the match overview
    overview_.updateData( logData_->getNbLine() );

    // Also update the top window for the coloured bullets.
    update();
}

void CrawlerWidget::applyConfiguration()
{
    const auto& config = Configuration::get();
    QFont font = config.mainFont();

    LOG( logDEBUG ) << "CrawlerWidget::applyConfiguration";

    // Whatever font we use, we should NOT use kerning
    font.setKerning( false );
    font.setFixedPitch( true );

    // Necessary on systems doing subpixel positionning (e.g. Ubuntu 12.04)
    if ( config.forceFontAntialiasing() ) {
        font.setStyleStrategy( static_cast<QFont::StyleStrategy>( QFont::ForceIntegerMetrics
                                                                  | QFont::PreferAntialias ) );
    }
    else {
        font.setStyleStrategy( QFont::ForceIntegerMetrics );
    }

    logMainView->setFont( font );
    filteredView->setFont( font );
    cmdEntryBox->setFont( font );
    promptLbl->setFont( font );

    logMainView->setLineNumbersVisible( config.mainLineNumbersVisible() );
    filteredView->setLineNumbersVisible( config.filteredLineNumbersVisible() );

    overview_.setVisible( config.isOverviewVisible() );
    logMainView->refreshOverview();

    logMainView->updateDisplaySize();
    logMainView->update();
    filteredView->updateDisplaySize();
    filteredView->update();

    // Update the SearchLine (history)
    updateSearchCombo();
    updateCommandCombo();

    FileWatcher::getFileWatcher().updateConfiguration();

    if ( isFollowEnabled() ) {
        changeDataStatus( DataStatus::OLD_DATA );
    }
}

void CrawlerWidget::enteringQuickFind()
{
    LOG( logDEBUG ) << "CrawlerWidget::enteringQuickFind";

    // Remember who had the focus (only if it is one of our views)
    QWidget* focus_widget = QApplication::focusWidget();

    if ( ( focus_widget == logMainView ) || ( focus_widget == filteredView ) )
        qfSavedFocus_ = focus_widget;
    else
        qfSavedFocus_ = nullptr;
}

void CrawlerWidget::exitingQuickFind()
{
    // Restore the focus once the QFBar has been hidden
    if ( qfSavedFocus_ )
        qfSavedFocus_->setFocus();
}

void CrawlerWidget::loadingFinishedHandler( LoadingStatus status )
{
    loadingInProgress_ = false;

    // We need to refresh the main window because the view lines on the
    // overview have probably changed.
    overview_.updateData( logData_->getNbLine() );

    // FIXME, handle topLine
    // logMainView->updateData( logData_, topLine );
    logMainView->updateData();

    // Shall we Forbid starting a search when loading in progress?
    // searchButton->setEnabled( false );

    // searchButton->setEnabled( true );

    // See if we need to auto-refresh the search
    if ( searchState_.isAutorefreshAllowed() ) {
        searchEndLine_ = LineNumber( logData_->getNbLine().get() );
        if ( searchState_.isFileTruncated() )
            // We need to restart the search
            replaceCurrentSearch( searchLineEdit->currentText() );
        else
            logFilteredData_->updateSearch( searchStartLine_, searchEndLine_ );
    }

    // Set the encoding for the views
    updateEncoding();

    clearSearchLimits();

    emit loadingFinished( status );

    // Also change the data available icon
    if ( firstLoadDone_ ) {
        changeDataStatus( DataStatus::NEW_DATA );
    }
    else {
        firstLoadDone_ = true;
        for ( const auto& m : savedMarkedLines_ ) {
            logFilteredData_->addMark( m );
        }
    }
}

void CrawlerWidget::fileChangedHandler( MonitoredFileStatus status )
{
    // Handle the case where the file has been truncated
    if ( status == MonitoredFileStatus::Truncated ) {
        // Clear all marks (TODO offer the option to keep them)
        logFilteredData_->clearMarks();
        if ( !searchInfoLine->text().isEmpty() ) {
            // Invalidate the search
            logFilteredData_->clearSearch();
            filteredView->updateData();
            searchState_.truncateFile();
            printSearchInfoMessage();
            nbMatches_ = 0_lcount;
        }
    }
}

// Returns a pointer to the window in which the search should be done
AbstractLogView* CrawlerWidget::activeView() const
{
    QWidget* activeView;

    // Search in the window that has focus, or the window where 'Find' was
    // called from, or the main window.
    if ( filteredView->hasFocus() || logMainView->hasFocus() )
        activeView = QApplication::focusWidget();
    else
        activeView = qfSavedFocus_;

    if ( activeView ) {
        auto* view = qobject_cast<AbstractLogView*>( activeView );
        return view;
    }
    else {
        LOG( logWARNING ) << "No active view, defaulting to logMainView";
        return logMainView;
    }
}

void CrawlerWidget::searchForward()
{
    LOG( logDEBUG ) << "CrawlerWidget::searchForward";

    activeView()->searchForward();
}

void CrawlerWidget::searchBackward()
{
    LOG( logDEBUG ) << "CrawlerWidget::searchBackward";

    activeView()->searchBackward();
}

void CrawlerWidget::searchRefreshChangedHandler( bool isRefreshing )
{
    searchState_.setAutorefresh( isRefreshing );
    printSearchInfoMessage( logFilteredData_->getNbMatches() );
}

void CrawlerWidget::searchTextChangeHandler( QString )
{
    // We suspend auto-refresh
    searchState_.changeExpression();
    printSearchInfoMessage( logFilteredData_->getNbMatches() );
}

void CrawlerWidget::changeFilteredViewVisibility( int index )
{
    QStandardItem* item = visibilityModel_->item( index );
    auto visibility = item->data().value<FilteredView::Visibility>();

    filteredView->setVisibility( visibility );

    if ( logFilteredData_->getNbLine() > 0_lcount ) {
        const auto lineIndex = logFilteredData_->getLineIndexNumber( currentLineNumber_ );
        filteredView->selectAndDisplayLine( lineIndex );
    }
}

void CrawlerWidget::addToSearch( const QString& string )
{
    QString text = searchLineEdit->currentText();

    if ( text.isEmpty() )
        text = string;
    else {
        // Escape the regexp chars from the string before adding it.
        text += ( '|' + QRegularExpression::escape( string ) );
    }

    searchLineEdit->setEditText( text );

    // Set the focus to lineEdit so that the user can press 'Return' immediately
    searchLineEdit->lineEdit()->setFocus();
}

void CrawlerWidget::mouseHoveredOverMatch( LineNumber line )
{
    const auto line_in_mainview = logFilteredData_->getMatchingLineNumber( line );

    overviewWidget_->highlightLine( line_in_mainview );
}

void CrawlerWidget::activityDetected()
{
    changeDataStatus( DataStatus::OLD_DATA );
}

void CrawlerWidget::setSearchLimits( LineNumber startLine, LineNumber endLine )
{
    searchStartLine_ = startLine;
    searchEndLine_ = endLine;

    logMainView->setSearchLimits( startLine, endLine );
    filteredView->setSearchLimits( startLine, endLine );
}

void CrawlerWidget::clearSearchLimits()
{
    setSearchLimits( 0_lnum, LineNumber( logData_->getNbLine().get() ) );
}

//
// Private functions
//

// Build the widget and connect all the signals, this must be done once
// the data are attached.
void CrawlerWidget::setup()
{
    setOrientation( Qt::Vertical );

    assert( logData_ );
    assert( logFilteredData_ );

    // The views
    mainWindow = new QWidget;
    bottomWindow = new QWidget;
    bottomWindow->setContentsMargins( 2, 0, 2, 0 );

    overviewWidget_ = new OverviewWidget();
    logMainView = new LogMainView( logData_, quickFindPattern_.get(), &overview_, overviewWidget_ );
    logMainView->setContentsMargins( 2, 0, 2, 0 );

    filteredView = new FilteredView( logFilteredData_, quickFindPattern_.get() );
    filteredView->setContentsMargins( 2, 0, 2, 0 );

    overviewWidget_->setOverview( &overview_ );
    overviewWidget_->setParent( logMainView );

    // Connect the search to the top view
    logMainView->useNewFiltering( logFilteredData_ );

    // Construct the visibility button
    using VisibilityFlags = LogFilteredData::VisibilityFlags;
    visibilityModel_ = new QStandardItemModel( this );

    QStandardItem* marksAndMatchesItem = new QStandardItem( tr( "Marks and matches" ) );
    marksAndMatchesItem->setData(
        QVariant::fromValue( VisibilityFlags::Marks | VisibilityFlags::Matches ) );
    visibilityModel_->appendRow( marksAndMatchesItem );

    QStandardItem* marksItem = new QStandardItem( tr( "Marks" ) );
    marksItem->setData( QVariant::fromValue<FilteredView::Visibility>( VisibilityFlags::Marks ) );
    visibilityModel_->appendRow( marksItem );

    QStandardItem* matchesItem = new QStandardItem( tr( "Matches" ) );
    matchesItem->setData(
        QVariant::fromValue<FilteredView::Visibility>( VisibilityFlags::Matches ) );
    visibilityModel_->appendRow( matchesItem );

    auto* visibilityView = new QListView( this );
    visibilityView->setMovement( QListView::Static );
    // visibilityView->setMinimumWidth( 170 ); // Only needed with custom style-sheet

    visibilityBox = new QComboBox();
    visibilityBox->setModel( visibilityModel_ );
    visibilityBox->setView( visibilityView );

    // Select "Marks and matches" by default (same default as the filtered view)
    visibilityBox->setCurrentIndex( 0 );
    visibilityBox->setContentsMargins( 2, 2, 2, 2 );

    // TODO: Maybe there is some way to set the popup width to be
    // sized-to-content (as it is when the stylesheet is not overriden) in the
    // stylesheet as opposed to setting a hard min-width on the view above.
    /*visibilityBox->setStyleSheet( " \
        QComboBox:on {\
            padding: 1px 2px 1px 6px;\
            width: 19px;\
        } \
        QComboBox:!on {\
            padding: 1px 2px 1px 7px;\
            width: 19px;\
            height: 16px;\
            border: 1px solid gray;\
        } \
        QComboBox::drop-down::down-arrow {\
            width: 0px;\
            border-width: 0px;\
        } \
" );*/

    // Construct the Search Info line
    searchInfoLine = new InfoLine();
    searchInfoLine->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    searchInfoLine->setLineWidth( 1 );
    searchInfoLine->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    auto searchInfoLineSizePolicy = searchInfoLine->sizePolicy();
    searchInfoLineSizePolicy.setRetainSizeWhenHidden( false );
    searchInfoLine->setSizePolicy( searchInfoLineSizePolicy );
    searchInfoLineDefaultPalette = searchInfoLine->palette();
    searchInfoLine->setContentsMargins( 2, 2, 2, 2 );

    matchCaseButton = new QToolButton();
    matchCaseButton->setToolTip( "Match case" );
    matchCaseButton->setIcon( iconLoader_.load( "icons8-font-size" ) );
    matchCaseButton->setCheckable( true );
    matchCaseButton->setFocusPolicy( Qt::NoFocus );
    matchCaseButton->setContentsMargins( 2, 2, 2, 2 );

    useRegexpButton = new QToolButton();
    useRegexpButton->setToolTip( "Use regex" );
    useRegexpButton->setIcon( iconLoader_.load( "regex" ) );
    useRegexpButton->setCheckable( true );
    useRegexpButton->setFocusPolicy( Qt::NoFocus );
    useRegexpButton->setContentsMargins( 2, 2, 2, 2 );

    searchRefreshButton = new QToolButton();
    searchRefreshButton->setToolTip( "Auto-refresh" );
    searchRefreshButton->setIcon( iconLoader_.load( "icons8-search-refresh" ) );
    searchRefreshButton->setCheckable( true );
    searchRefreshButton->setFocusPolicy( Qt::NoFocus );
    searchRefreshButton->setContentsMargins( 2, 2, 2, 2 );

    // Construct the Prompt line
    promptView = new QWidget;
    auto* promptLayout = new QHBoxLayout(promptView);

    promptLbl = new QLabel("");
    promptLayout->addWidget(promptLbl);
    promptView->setLayout(promptLayout);

    // Construct the Command line
    cmdView = new QWidget;
    auto* cmdLayout = new QHBoxLayout(cmdView);

    auto* cmdLbl = new QLabel(tr("Cmd:"));
    cmdEntryBox = new QComboBox();

    cmdEntryBox->setEditable(true);
    cmdEntryBox->setCompleter(0);
    cmdEntryBox->addItems( savedCommands_->recentCommands() );
    cmdEntryBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    cmdEntryBox->setSizeAdjustPolicy( QComboBox::AdjustToMinimumContentsLengthWithIcon );
    cmdLayout->addWidget(cmdLbl);
    cmdLayout->addWidget(cmdEntryBox);
    cmdView->setLayout(cmdLayout);

    QWidget* btnRow = nullptr;
    if (logData_->isWritable()) {
        btnRow = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnRow);
        btnLayout->setContentsMargins(6, 0, 3, 3);
        btnLayout->setAlignment(Qt::AlignLeft);

        btnLayout->addWidget(new QLabel("Commands:"));

        for (auto i : { 1, 2, 3, 4 ,5 ,6 ,7 ,8, 9, 0}) {
            auto* btn = new CmdButton(i, "");
            connect(btn, &CmdButton::execute, this, &CrawlerWidget::executeBtnCommand);
            btnLayout->addWidget(btn);
            cmdBtns.push_back(btn);
        }

        btnRow->setLayout(btnLayout);
    }

    // Construct the Search line
    searchLineCompleter = new QCompleter( savedSearches_->recentSearches(), this );
    searchLineCompleter->setCaseSensitivity( Qt::CaseInsensitive );
    searchLineEdit = new QComboBox;
    searchLineEdit->setEditable( true );
    searchLineEdit->setCompleter( searchLineCompleter );
    searchLineEdit->addItems( savedSearches_->recentSearches() );
    searchLineEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    searchLineEdit->setSizeAdjustPolicy( QComboBox::AdjustToMinimumContentsLengthWithIcon );
    searchLineEdit->lineEdit()->setMaxLength( std::numeric_limits<int>::max() / 1024 );
    searchLineEdit->setContentsMargins( 2, 2, 2, 2 );

    setFocusProxy( searchLineEdit );

    searchButton = new QToolButton();
    searchButton->setIcon( iconLoader_.load( "icons8-search" ) );
    searchButton->setText( tr( "Search" ) );
    searchButton->setAutoRaise( true );
    searchButton->setContentsMargins( 2, 2, 2, 2 );

    stopButton = new QToolButton();
    stopButton->setIcon( iconLoader_.load( "icons8-delete" ) );
    stopButton->setAutoRaise( true );
    stopButton->setEnabled( false );
    stopButton->setVisible( false );
    stopButton->setContentsMargins( 2, 2, 2, 2 );

    auto* searchLineLayout = new QHBoxLayout;
    searchLineLayout->setContentsMargins( 2, 2, 2, 2 );

    searchLineLayout->addWidget( visibilityBox );
    searchLineLayout->addWidget( matchCaseButton );
    searchLineLayout->addWidget( useRegexpButton );
    searchLineLayout->addWidget( searchLineEdit );
    searchLineLayout->addWidget( searchButton );
    searchLineLayout->addWidget( stopButton );
    searchLineLayout->addWidget( searchRefreshButton );
    searchLineLayout->addWidget( searchInfoLine );


    // Construct the main window
    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(logMainView);
    if (logData_->isWritable())
    {
        mainLayout->addWidget(promptView);
        mainLayout->addWidget(cmdView);
        mainLayout->addWidget(btnRow);
    }
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(0);
    mainWindow->setLayout(mainLayout);

    // Construct the bottom window
    auto* bottomMainLayout = new QVBoxLayout;
    bottomMainLayout->addLayout( searchLineLayout );
    bottomMainLayout->addWidget( filteredView );
    bottomMainLayout->setContentsMargins( 2, 2, 2, 2 );
    bottomWindow->setLayout( bottomMainLayout );

    addWidget( mainWindow );
    addWidget( bottomWindow );

    // Default search checkboxes
    auto& config = Configuration::get();
    searchRefreshButton->setChecked( config.isSearchAutoRefreshDefault() );
    // Manually call the handler as it is not called when changing the state programmatically
    searchRefreshChangedHandler( searchRefreshButton->isChecked() );
    matchCaseButton->setChecked( config.isSearchIgnoreCaseDefault() ? false : true );

    useRegexpButton->setChecked( config.mainRegexpType() == ExtendedRegexp );

    // Default splitter position (usually overridden by the config file)
    setSizes( config.splitterSizes() );

    // Connect the signals
    connect( searchLineEdit->lineEdit(), &QLineEdit::returnPressed, searchButton,
             &QToolButton::click );
    connect( searchLineEdit->lineEdit(), &QLineEdit::textEdited, this,
             &CrawlerWidget::searchTextChangeHandler );
    connect( searchButton, &QToolButton::clicked, this, &CrawlerWidget::startNewSearch );
    connect( stopButton, &QToolButton::clicked, this, &CrawlerWidget::stopSearch );

    connect(cmdEntryBox->lineEdit(), SIGNAL( returnPressed() ),
            this, SLOT( executeCommand() ) );

    connect( visibilityBox, QOverload<int>::of( &QComboBox::currentIndexChanged ), this,
             &CrawlerWidget::changeFilteredViewVisibility );

    connect( logMainView, &LogMainView::newSelection, [this]( auto ) { logMainView->update(); } );
    connect( filteredView, &FilteredView::newSelection,
             [this]( auto ) { filteredView->update(); } );

    connect( filteredView, &FilteredView::newSelection, this, &CrawlerWidget::jumpToMatchingLine );

    connect( logMainView, &LogMainView::updateLineNumber, this,
             &CrawlerWidget::updateLineNumberHandler );

    connect( logMainView, &LogMainView::markLines, this, &CrawlerWidget::markLinesFromMain );
    connect( filteredView, &FilteredView::markLines, this, &CrawlerWidget::markLinesFromFiltered );

    connect( logMainView, QOverload<const QString&>::of( &LogMainView::addToSearch ), this,
             &CrawlerWidget::addToSearch );
    connect( filteredView, QOverload<const QString&>::of( &FilteredView::addToSearch ), this,
             &CrawlerWidget::addToSearch );

    connect( filteredView, &FilteredView::mouseHoveredOverLine, this,
             &CrawlerWidget::mouseHoveredOverMatch );
    connect( filteredView, &FilteredView::mouseLeftHoveringZone, overviewWidget_,
             &OverviewWidget::removeHighlight );

    // Follow option (up and down)
    connect( this, &CrawlerWidget::followSet, logMainView, &LogMainView::followSet );
    connect( this, &CrawlerWidget::followSet, filteredView, &FilteredView::followSet );
    connect( logMainView, &LogMainView::followModeChanged, this,
             &CrawlerWidget::followModeChanged );
    connect( filteredView, &FilteredView::followModeChanged, this,
             &CrawlerWidget::followModeChanged );

    // Detect activity in the views
    connect( logMainView, &LogMainView::activity, this, &CrawlerWidget::activityDetected );
    connect( filteredView, &FilteredView::activity, this, &CrawlerWidget::activityDetected );

    connect( logMainView, &LogMainView::changeSearchLimits, this, &CrawlerWidget::setSearchLimits );
    connect( filteredView, &FilteredView::changeSearchLimits, this,
             &CrawlerWidget::setSearchLimits );

    connect( logMainView, &LogMainView::clearSearchLimits, this,
             &CrawlerWidget::clearSearchLimits );
    connect( filteredView, &FilteredView::clearSearchLimits, this,
             &CrawlerWidget::clearSearchLimits );

    auto saveSplitterSizes = [this, &config]() { config.setSplitterSizes( this->sizes() ); };

    connect( logMainView, &LogMainView::saveDefaultSplitterSizes, saveSplitterSizes );
    connect( filteredView, &FilteredView::saveDefaultSplitterSizes, saveSplitterSizes );

    connect( logFilteredData_, &LogFilteredData::searchProgressed, this,
             &CrawlerWidget::updateFilteredView, Qt::QueuedConnection );

    // Sent load file update to MainWindow (for status update)
    connect( logData_, &LogDataBase::loadingProgressed, this, &CrawlerWidget::loadingProgressed );
    connect( logData_, &LogDataBase::loadingFinished, this, &CrawlerWidget::loadingFinishedHandler );
    connect( logData_, &LogDataBase::fileChanged, this, &CrawlerWidget::fileChangedHandler );
    connect (logData_, &LogDataBase::promptUpdated, this, &CrawlerWidget::updatePrompt );

    // Search auto-refresh
    connect( searchRefreshButton, &QPushButton::toggled, this,
             &CrawlerWidget::searchRefreshChangedHandler );

    // Advise the parent the checkboxes have been changed
    // (for maintaining default config)
    connect( searchRefreshButton, &QPushButton::toggled, this,
             &CrawlerWidget::searchRefreshChanged );
    connect( matchCaseButton, &QPushButton::toggled, this, &CrawlerWidget::matchCaseChanged );

    // Switch between views
    connect( logMainView, &LogMainView::exitView, filteredView,
             QOverload<>::of( &FilteredView::setFocus ) );
    connect( filteredView, &FilteredView::exitView, logMainView,
             QOverload<>::of( &LogMainView::setFocus ) );
}

// Create a new search using the text passed, replace the currently
// used one and destroy the old one.
void CrawlerWidget::replaceCurrentSearch( const QString& searchText )
{
    // Interrupt the search if it's ongoing
    logFilteredData_->interruptSearch();

    // We have to wait for the last search update (100%)
    // before clearing/restarting to avoid having remaining results.

    // FIXME: this is a bit of a hack, we call processEvents
    // for Qt to empty its event queue, including (hopefully)
    // the search update event sent by logFilteredData_. It saves
    // us the overhead of having proper sync.
    QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );

    nbMatches_ = 0_lcount;

    // Clear and recompute the content of the filtered window.
    logFilteredData_->clearSearch();
    filteredView->updateData();

    // Update the match overview
    overview_.updateData( logData_->getNbLine() );

    if ( !searchText.isEmpty() ) {

        QString pattern;

        if ( !useRegexpButton->isChecked() ) {
            pattern = QRegularExpression::escape( searchText );
        }
        else {
            pattern = searchText;
        }

        // Set the pattern case insensitive if needed
        QRegularExpression::PatternOptions patternOptions
            = QRegularExpression::UseUnicodePropertiesOption;

        if ( !matchCaseButton->isChecked() )
            patternOptions |= QRegularExpression::CaseInsensitiveOption;

        // Constructs the regexp
        QRegularExpression regexp( pattern, patternOptions );

        if ( regexp.isValid() ) {
            // Activate the stop button
            stopButton->setEnabled( true );
            stopButton->show();
            searchButton->hide();
            // Start a new asynchronous search
            logFilteredData_->runSearch( regexp, searchStartLine_, searchEndLine_ );
            // Accept auto-refresh of the search
            searchState_.startSearch();
            searchInfoLine->hide();
        }
        else {
            // The regexp is wrong
            logFilteredData_->clearSearch();
            filteredView->updateData();
            searchState_.resetState();

            // Inform the user
            QString errorMessage = tr( "Error in expression" );
            const int offset = regexp.patternErrorOffset();
            if ( offset != -1 ) {
                errorMessage += " at position ";
                errorMessage += QString::number( offset );
            }
            errorMessage += ": ";
            errorMessage += regexp.errorString();
            searchInfoLine->setPalette( errorPalette );
            searchInfoLine->setText( errorMessage );
            searchInfoLine->show();
        }
    }
    else {
        searchState_.resetState();
        printSearchInfoMessage();
    }
}

// Updates the content of the drop down list for the saved searches,
// called when the SavedSearch has been changed.
void CrawlerWidget::updateSearchCombo()
{
    const QString text = searchLineEdit->lineEdit()->text();
    searchLineEdit->clear();

    auto search_history = savedSearches_->recentSearches();

    searchLineEdit->addItems( search_history );
    // In case we had something that wasn't added to the list (blank...):
    searchLineEdit->lineEdit()->setText( text );

    searchLineCompleter->setModel( new QStringListModel( search_history, searchLineCompleter ) );
}

// Updates the content of the drop down list for the saved commands,
// called when the SavedCommand has been changed.
void CrawlerWidget::updateCommandCombo()
{
    const QString text = cmdEntryBox->lineEdit()->text();
    cmdEntryBox->clear();
    cmdEntryBox->addItems( savedCommands_->recentCommands() );

    cmdEntryBox->lineEdit()->setText( "" );

    // qqq something like the searchLineCompleter???
}


// Print the search info message.
void CrawlerWidget::printSearchInfoMessage( LinesCount nbMatches )
{
    QString text;

    switch ( searchState_.getState() ) {
    case SearchState::NoSearch:
        // Blank text is fine
        break;
    case SearchState::Static:
    case SearchState::Autorefreshing:
        text = tr( "%1 match%2 found." )
                   .arg( nbMatches.get() )
                   .arg( nbMatches.get() > 1 ? "es" : "" );
        break;
    case SearchState::FileTruncated:
    case SearchState::TruncatedAutorefreshing:
        text = tr( "File truncated on disk" );
        break;
    }

    searchInfoLine->setPalette( searchInfoLineDefaultPalette );
    searchInfoLine->setText( text );
    searchInfoLine->setVisible( !text.isEmpty() );
}

// Change the data status and, if needed, advise upstream.
void CrawlerWidget::changeDataStatus( DataStatus status )
{
    LOG( logINFO ) << "New data status " << static_cast<int>( status );
    if ( ( status != dataStatus_ )
         && ( !( dataStatus_ == DataStatus::NEW_FILTERED_DATA
                 && status == DataStatus::NEW_DATA ) ) ) {
        dataStatus_ = status;
        emit dataStatusChanged( dataStatus_ );
    }
}

// Determine the right encoding and set the views.
void CrawlerWidget::updateEncoding()
{
    const QTextCodec* textCodec = [this]() {
        QTextCodec* codec = nullptr;
        if ( !encodingMib_ ) {
            codec = logData_->getDetectedEncoding();
        }
        else {
            codec = QTextCodec::codecForMib( *encodingMib_ );
        }
        return codec ? codec : QTextCodec::codecForLocale();
    }();

    QString encodingPrefix = encodingMib_ ? "Displayed as %1" : "Detected as %1";
    encoding_text_ = tr( encodingPrefix.arg( textCodec->name().constData() ).toLatin1() );

    logData_->setDisplayEncoding( textCodec->name().constData() );
    logMainView->forceRefresh();
    logFilteredData_->setDisplayEncoding( textCodec->name().constData() );
    filteredView->forceRefresh();
}

// Change the respective size of the two views
void CrawlerWidget::changeTopViewSize( int32_t delta )
{
    int min, max;
    getRange( 1, &min, &max );
    LOG( logDEBUG ) << "CrawlerWidget::changeTopViewSize " << sizes().at( 0 ) << " " << min << " "
                    << max;
    moveSplitter( closestLegalPosition( sizes().at( 0 ) + ( delta * 10 ), 1 ), 1 );
    LOG( logDEBUG ) << "CrawlerWidget::changeTopViewSize " << sizes().at( 0 );
}

//
// SearchState implementation
//
void CrawlerWidget::SearchState::resetState()
{
    state_ = NoSearch;
}

void CrawlerWidget::SearchState::setAutorefresh( bool refresh )
{
    autoRefreshRequested_ = refresh;

    if ( refresh ) {
        if ( state_ == Static )
            state_ = Autorefreshing;
        /*
        else if ( state_ == FileTruncated )
            state_ = TruncatedAutorefreshing;
        */
    }
    else {
        if ( state_ == Autorefreshing )
            state_ = Static;
        else if ( state_ == TruncatedAutorefreshing )
            state_ = FileTruncated;
    }
}

void CrawlerWidget::SearchState::truncateFile()
{
    if ( state_ == Autorefreshing || state_ == TruncatedAutorefreshing ) {
        state_ = TruncatedAutorefreshing;
    }
    else {
        state_ = FileTruncated;
    }
}

void CrawlerWidget::SearchState::changeExpression()
{
    if ( state_ == Autorefreshing )
        state_ = Static;
}

void CrawlerWidget::SearchState::stopSearch()
{
    if ( state_ == Autorefreshing )
        state_ = Static;
}

void CrawlerWidget::SearchState::startSearch()
{
    if ( autoRefreshRequested_ )
        state_ = Autorefreshing;
    else
        state_ = Static;
}

/*
 * CrawlerWidgetContext
 */
CrawlerWidgetContext::CrawlerWidgetContext( const QString& string )
{
    if ( string.startsWith( '{' ) ) {
        loadFromJson( string );
    }
    else {
        loadFromString( string );
    }
}

void CrawlerWidgetContext::loadFromString( const QString& string )
{
    QRegularExpression regex( "S(\\d+):(\\d+)" );
    QRegularExpressionMatch match = regex.match( string );
    if ( match.hasMatch() ) {
        sizes_ = { match.captured( 1 ).toInt(), match.captured( 2 ).toInt() };
        LOG( logDEBUG ) << "sizes_: " << sizes_[ 0 ] << " " << sizes_[ 1 ];
    }
    else {
        LOG( logWARNING ) << "Unrecognised view size: " << string.toLocal8Bit().data();

        // Default values;
        sizes_ = { 400, 100 };
    }

    QRegularExpression case_refresh_regex( "IC(\\d+):AR(\\d+)" );
    match = case_refresh_regex.match( string );
    if ( match.hasMatch() ) {
        ignore_case_ = ( match.captured( 1 ).toInt() == 1 );
        auto_refresh_ = ( match.captured( 2 ).toInt() == 1 );

        LOG( logDEBUG ) << "ignore_case_: " << ignore_case_ << " auto_refresh_: " << auto_refresh_;
    }
    else {
        LOG( logWARNING ) << "Unrecognised case/refresh: " << string.toLocal8Bit().data();
        ignore_case_ = false;
        auto_refresh_ = false;
    }

    QRegularExpression follow_regex( "AR(\\d+):FF(\\d+)" );
    match = follow_regex.match( string );
    if ( match.hasMatch() ) {
        follow_file_ = ( match.captured( 2 ).toInt() == 1 );

        LOG( logDEBUG ) << "follow_file_: " << follow_file_;
    }
    else {
        LOG( logWARNING ) << "Unrecognised follow file " << string.toLocal8Bit().data();
        follow_file_ = false;
    }

    use_regexp_ = Configuration::get().mainRegexpType() == ExtendedRegexp;
}

void CrawlerWidgetContext::loadFromJson( const QString& json )
{
    const auto properties = QJsonDocument::fromJson( json.toLatin1() ).toVariant().toMap();

    if ( properties.contains( "S" ) ) {
        const auto sizes = properties.value( "S" ).toList();
        for ( const auto& s : sizes ) {
            sizes_.append( s.toInt() );
        }
    }

    ignore_case_ = properties.value( "IC" ).toBool();
    auto_refresh_ = properties.value( "AR" ).toBool();
    follow_file_ = properties.value( "FF" ).toBool();
    if ( properties.contains( "RE" ) ) {
        use_regexp_ = properties.value( "RE" ).toBool();
    }
    else {
        use_regexp_ = Configuration::get().mainRegexpType() == ExtendedRegexp;
    }

    if ( properties.contains( "M" ) ) {
        const auto marks = properties.value( "M" ).toList();
        for ( const auto& m : marks ) {
            marks_.append( m.toUInt() );
        }
    }

    if ( properties.contains( "CB" ) ) {
        const auto cmdButtons = properties.value( "CB" ).toList();
        btnCmds_.clear();
        foreach (const auto & cmdButton, cmdButtons) {
            btnCmds_.push_back(cmdButton.toString());
        }
    }
}

QString CrawlerWidgetContext::toString() const
{
    const auto toVariantList = []( const auto& list ) -> QVariantList {
        QVariantList variantList;
        for ( const auto& item : list ) {
            variantList.append( item );
        }
        return variantList;
    };

    QVariantMap properies;

    properies[ "S" ] = toVariantList( sizes_ );
    properies[ "IC" ] = ignore_case_;
    properies[ "AR" ] = auto_refresh_;
    properies[ "FF" ] = follow_file_;
    properies[ "RE" ] = use_regexp_;
    properies[ "M" ] = toVariantList( marks_ );
    properies[ "CB" ] = toVariantList(btnCmds_);
    return QJsonDocument::fromVariant( properies ).toJson( QJsonDocument::Compact );
}
