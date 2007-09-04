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

#include "documentspecificationdocument.h"

namespace Cokoon {

    DocumentSpecificationDocument::DocumentSpecificationDocument(DocumentSpecification *spec)
        :m_spec(spec)
    {
    }
    DocumentSpecificationDocument::~DocumentSpecificationDocument()
    {
    }
    int DocumentSpecificationDocument::mapToId(DeclarationType type, const QString &str) const
    {
        int id = m_spec->mapToId(type,str);
        if (id==-1)
            return Document::mapToId(type,str);
        else
            return id;
    }
    int DocumentSpecificationDocument::objectStateLevels(int objId) const
    {
        return m_spec->itemStateLevels(objId);
    }
    int DocumentSpecificationDocument::objectStateLevelStates(int objId, int stateLevel) const
    {
        return m_spec->itemStateLevelStates(objId,stateLevel);
    }
    int DocumentSpecificationDocument::mapObjectStateToId(int objId, int stateLevel, const QString &stateName) const
    {
        return m_spec->mapItemStateToId(objId,stateLevel,stateName);
    }
    int DocumentSpecificationDocument::mapObjectStateToId(int objId,int stateLevel, int stateNumber) const
    {
        return m_spec->mapItemStateToId(objId,stateLevel,stateNumber);
    }
    int DocumentSpecificationDocument::customIdMappingBase(DeclarationType type) const
    {
        return m_spec->customIdMappingBase(type);
    }


}

