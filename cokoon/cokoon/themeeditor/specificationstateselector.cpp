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

#include <QDebug>

#include <QtAlgorithms>

#include <math.h>

#include "specificationstateselector.h"
#include <QFile>

#include "ui_specificationstateselector.h"
#include "ui_specificationstateselectorstate.h"

namespace Cokoon {

    SpecificationStateSelectorState::SpecificationStateSelectorState(int stateLevel, QStringList states, QWidget *parent)
        : QWidget(parent), m_lvl(stateLevel),m_states(states),m_ui(new Ui_SpecificationStateSelectorState())
    {
        m_ui->setupUi(this);
        m_ui->label->setText(QString("State &%1:").arg(stateLevel));
        m_ui->state->addItems(states);
        connect(m_ui->state, SIGNAL(currentIndexChanged(int)), this, SIGNAL(stateIndexChanged(int)));
    }

    SpecificationStateSelectorState::~SpecificationStateSelectorState()
    {
    }

    int SpecificationStateSelectorState::stateIndex() const
    {
        return m_ui->state->currentIndex();
    }

    SpecificationStateSelector::SpecificationStateSelector(QWidget * parent)
        : QWidget(parent), m_spec(0), m_ui(new Ui_SpecificationStateSelector())
    {
        m_ui->setupUi(this);

        m_stateLayout = new QHBoxLayout(m_ui->stateContainer);
        m_stateLayout->setContentsMargins(0,0,0,0);

        connect(m_ui->specItem, SIGNAL(activated(int)),this,SLOT(setCurrentItem(int)));
        connect(this, SIGNAL(currentItemChanged(int)),SLOT(slotObjIdChanged()));
    }

    SpecificationStateSelector::~SpecificationStateSelector()
    {
        clear();
    }

    void SpecificationStateSelector::clear()
    {
        qDeleteAll(m_stateLevels);
        m_stateLevels.clear();
    }

    void SpecificationStateSelector::setSpecification(const DocumentSpecification *spec)
    {
        m_spec = spec;

        m_ui->specItem->clear();
        foreach(const SpecificationItem *i, m_spec->items()) {
            m_ui->specItem->addItem(i->id);
        }
        m_ui->specItem->setEnabled(spec->items().size()>0);
        setCurrentItem(0);
    }

    int SpecificationStateSelector::currentObjectId() const
    {
        Q_ASSERT(m_spec);

        int itemId = m_ui->specItem->currentIndex();
        int id = itemId;
        if (id >= 0) {
            int lvlIndex = 0;
            foreach(SpecificationStateSelectorState *s, m_stateLevels) {
                id += m_spec->mapItemStateToId(itemId,lvlIndex,s->stateIndex());
                ++lvlIndex;
            }
            return id;
        }
        return -1;
    }

    void SpecificationStateSelector::setCurrentItem(int itemIndex)
    {
        Q_ASSERT(m_spec);

        clear();
        const SpecificationItem *item = m_spec->items().value(itemIndex);
        if(item && itemIndex>=0 && itemIndex<m_ui->specItem->count()) {
            m_ui->specItem->setCurrentIndex(itemIndex);
            int lvlIndex = 0;
            foreach(SpecificationItemStateLevel *lvl, item->stateLevels) {
                SpecificationStateSelectorState *s =
                    new SpecificationStateSelectorState(lvlIndex,lvl->states,
                                                        m_ui->stateContainer);
                m_stateLevels.append(s);
                m_stateLayout->addWidget(s);
                connect(s,SIGNAL(stateIndexChanged(int)),this,SLOT(slotObjIdChanged()));
                ++lvlIndex;
            }
            emit currentItemChanged(itemIndex);
        }
    }

    void SpecificationStateSelector::slotObjIdChanged()
    {
        emit objectIdChanged(currentObjectId());
    }

}

#include "specificationstateselector.moc"
