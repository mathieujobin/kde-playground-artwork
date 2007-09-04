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

#ifndef COKOON_DOCUMENTSPECIFICATIONDOCUMENT_H
#define COKOON_DOCUMENTSPECIFICATIONDOCUMENT_H

#import "../specification.h"
#import "../document.h"

class QPaintEvent;

namespace Cokoon {

    class DocumentSpecificationDocument : public Document
    {
    public:
        DocumentSpecificationDocument(DocumentSpecification *spec);
        virtual ~DocumentSpecificationDocument();
        virtual int mapToId(DeclarationType type, const QString &str) const;
        virtual int objectStateLevels(int objId) const;
        virtual int objectStateLevelStates(int objId, int stateLevel) const;
        virtual int mapObjectStateToId(int objId, int stateLevel, const QString &stateName) const;
        virtual int mapObjectStateToId(int objId,int stateLevel, int stateNumber) const;
        virtual int customIdMappingBase(DeclarationType type) const;
    private:
        DocumentSpecification *m_spec;
    };

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
