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


#include "metadata.h"
#include "generics.h"


namespace y
  {
    
    metadata::metadata(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le)
      : root(sc),
        sym_factory(sf),
        cache(ac),
        env(le)
      {
      }
    
    
    void metadata::set_model(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_model(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::add_email(author& a, lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_email(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::add_institute(author& a, lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return a.set_institute(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::set_name(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_name(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::add_author(lexeme_type& lex, author& a)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { a.set_name(name, lex); return this->root.add_author(name, lex, a); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::set_citeguide(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_citeguide(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::set_description(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_description(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::set_revision(lexeme_type& lex)
      {
        auto Setter = [&](auto& val, auto& lex) -> auto { return this->root.set_revision(val, lex); };
        SetIntegerValue(Setter, lex);
      }
    
    
    void metadata::set_license(lexeme_type& lex)
      {
        auto Setter = [&](auto& name, auto& lex) -> auto { return this->root.set_license(name, lex); };
        SetStringValue(Setter, lex);
      }
    
    
    void metadata::set_references(string_array& a)
      {
        this->root.set_references(a.get_array());
      }
    
    
    void metadata::set_urls(string_array& a)
      {
        this->root.set_urls(a.get_array());
      }
    
  }   // namespace y
