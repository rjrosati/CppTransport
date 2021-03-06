//
// Created by David Seery on 17/06/2013.
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



#ifndef CPPTRANSPORT_Y_LEXER_H
#define CPPTRANSPORT_Y_LEXER_H

#undef  YY_DECL
#define YY_DECL int y::y_lexer::yylex()


#include <iostream>

#include "lexical.h"
#include "lexeme.h"
#include "lexstream.h"

#include "y_common.h"
#include "y_parser.hpp"

namespace y
	{

    class y_lexer
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        y_lexer(lexstream_type& s);

        //! destructor
        ~y_lexer() = default;

        //! get next lexeme
        int yylex(y::y_parser::semantic_type* lval);

		    //! return current lexeme, if a valid one exists.
				//! Used to inform the parser error handler about our current
				//! location in the input stream
		    boost::optional< lexeme_type& > get_current_lexeme() { return current; }

      private:

				//! lexeme stream container
        lexstream_type& stream;

				//! cache current lexeme; returned via get_current_lexeme() method
				boost::optional< lexeme_type& > current;

	    };

	}

#endif //CPPTRANSPORT_Y_LEXER_H
