//
// Created by David Seery on 22/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_LAGRANGIAN_H
#define CPPTRANSPORT_LAGRANGIAN_H


#include "semantic_data.h"
#include "script.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"


namespace y
  {
    
    class lagrangian
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        lagrangian(script& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);
        
        //! destructor is default
        ~lagrangian() = default;


        // INTERFACE -- EMPLACE LAGRANGIAN DATA IN THE MODEL DESCRIPTOR
  
      public:
    
        void add_field(lexeme_type* lex, attributes* a);
    
        void add_parameter(lexeme_type* lex, attributes* a);
    
        void add_subexpr(lexeme_type* lex, subexpr* e);
    
        void set_potential(GiNaC::ex* V, lexeme_type* lex);
    
        void add_latex_attribute(subexpr* e, lexeme_type* lex);
    
        void add_value_attribute(subexpr* e, GiNaC::ex* v, lexeme_type* lex);
    
    
        // INTERNAL DATA
      
      protected:
        
        //! model description container
        script& root;
        
        //! delegated symbol factory
        symbol_factory& sym_factory;
        
        //! delegated argument cache
        argument_cache& cache;
        
        //! delegated local environment
        local_environment& env;
        
      };
    
  }


#endif //CPPTRANSPORT_LAGRANGIAN_H
