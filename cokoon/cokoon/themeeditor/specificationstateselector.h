/* This file is part of the Cokoon theme library
 * Copyright (C) 2007 Sandro Giessl <giessl@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef COKOON_SPECIFICATIONSTATESELECTOR_H
#define COKOON_SPECIFICATIONSTATESELECTOR_H

#include <QString>
#include <QWidget>
#include <QStringList>

#include "../specification.h"

class QPainter;
class Ui_SpecificationStateSelector;
class Ui_SpecificationStateSelectorState;

namespace Cokoon {

    class SpecificationStateSelectorState : public QWidget
    {
        Q_OBJECT
     public:
        SpecificationStateSelectorState(int stateLevel, QStringList states, QWidget *parent);
        ~SpecificationStateSelectorState();

        int stateIndex() const;
        int setStateIndex();
    signals:
        void stateIndexChanged(int stateIndex);
    private:
        int m_lvl;
        QStringList m_states;
        Ui_SpecificationStateSelectorState *m_ui;
    };

/**
 * Selects a single item/state combination from a given specification.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Q_DECL_EXPORT SpecificationStateSelector : public QWidget
{
    Q_OBJECT
 public:

    /**
     * Construct.
     */
    SpecificationStateSelector(QWidget * parent = 0);
    /**
     * Destruct.
     */
    virtual ~SpecificationStateSelector();

    void setSpecification(const DocumentSpecification *spec);

    /**
     * @return the currently selected object ID which is made up of 
     *   the selected item and its state
     * @see Specification::mapItemStateToId()
     */
    int currentObjectId() const;

 signals:
    void currentItemChanged(int itemIndex);
    void objectIdChanged(int objId);

 public slots:
    void setCurrentItem(int itemIndex);

 private slots:
    void slotObjIdChanged();

 private:
    void clear();

    const DocumentSpecification *m_spec;
    QLayout *m_stateLayout;
    QList<SpecificationStateSelectorState*> m_stateLevels;
    Ui_SpecificationStateSelector *m_ui;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
