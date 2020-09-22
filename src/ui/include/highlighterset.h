/*
 * Copyright (C) 2009, 2010 Nicolas Bonnefon and other contributors
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
 * Copyright (C) 2019 Anton Filimonov and other contributors
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

#ifndef highlighterSet_H
#define highlighterSet_H

#include <QColor>
#include <QMetaType>
#include <QRegularExpression>

#include "highlightedmatch.h"
#include "persistable.h"

// Represents a filter, i.e. a regexp and the colors matching text
// should be rendered in.
class Highlighter {
  public:
    // Construct an uninitialized Highlighter (when reading from a config file)
    Highlighter() = default;
    Highlighter( const QString& pattern, bool ignoreCase, bool onlyMatch, const QColor& foreColor,
                 const QColor& backColor );

    bool matchLine( const QString& line, std::vector<HighlightedMatch>& matches ) const;

    // Accessor functions
    QString pattern() const;
    void setPattern( const QString& pattern );
    bool ignoreCase() const;
    void setIgnoreCase( bool ignoreCase );

    bool highlightOnlyMatch() const;
    void setHighlightOnlyMatch( bool onlyMatch );

    const QColor& foreColor() const;
    void setForeColor( const QColor& foreColor );
    const QColor& backColor() const;
    void setBackColor( const QColor& backColor );

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    QRegularExpression regexp_;

    bool highlightOnlyMatch_ = false;
    QColor foreColor_;
    QColor backColor_;
};

enum class HighlighterMatchType {
  NoMatch,
  WordMatch,
  LineMatch
};

// Represents an ordered set of filters to be applied to each line displayed.
class HighlighterSet {
  public:
    static const char* persistableName()
    {
        return "HighlighterSet";
    }

    static HighlighterSet createNewSet( const QString& name );

    HighlighterSet() = default;

    QString name() const;
    QString id() const;

    // Returns weither the passed line match a filter of the set,
    // if so, it returns the fore/back colors the line should use.
    HighlighterMatchType matchLine( const QString& line, std::vector<HighlightedMatch>& matches ) const;

    bool isEmpty() const;

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    explicit HighlighterSet( const QString& name );

  private:
    static constexpr int HighlighterSet_VERSION = 3;
    static constexpr int FilterSet_VERSION = 2;

    QString name_;
    QString id_;
    QList<Highlighter> highlighterList_;

    // To simplify this class interface, HighlightersDialog can access our
    // internal structure directly.
    friend class HighlighterSetEdit;
};

class HighlighterSetCollection final : public Persistable<HighlighterSetCollection> {
  public:
    static const char* persistableName()
    {
        return "HighlighterSetCollection";
    }

    QList<HighlighterSet> highlighterSets() const;
    void setHighlighterSets( const QList<HighlighterSet>& highlighters );

    HighlighterSet currentSet() const;

    bool hasSet( const QString& setId ) const;
    QString currentSetId() const;
    void setCurrentSet( const QString& setId );

    // Reads/writes the current config in the QSettings object passed
    void saveToStorage( QSettings& settings ) const;
    void retrieveFromStorage( QSettings& settings );

  private:
    static constexpr int HighlighterSetCollection_VERSION = 1;

  private:
    QList<HighlighterSet> highlighters_;
    QString currentSet_;

    // To simplify this class interface, HighlightersDialog can access our
    // internal structure directly.
    friend class HighlightersDialog;
};

#endif
