//
// Created by David Seery on 15/06/15.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_Y_COMMON_H
#define CPPTRANSPORT_Y_COMMON_H


#include "lexeme.h"
#include "lexical.h"
#include "lexstream.h"


namespace y
	{

    typedef lexeme::lexeme<keyword_type, character_type> lexeme_type;

    typedef lexstream<keyword_type, character_type> lexstream_type;

	}   // namespace y


#endif //CPPTRANSPORT_Y_COMMON_H
