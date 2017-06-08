//
// Created by David Seery on 15/12/2015.
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


#include "replacement_rule_definitions.h"


namespace macro_packages
  {

    std::string replacement_rule_simple::operator()(const macro_argument_list& args)
      {
        this->validate(args);
        return this->evaluate(args);
      }
    
    
    void replacement_rule_simple::validate(const macro_argument_list& args)
      {
        // check that correct number of arguments have been supplied
        if(args.size() == this->num_args) return;
    
        std::ostringstream msg;
    
        msg << ERROR_MACRO_WRONG_ARGUMENT_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_ARGUMENT_COUNT
            << " " << this->num_args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
        throw argument_mismatch(msg.str());
      }


    std::string
    replacement_rule_index::evaluate_unroll(const macro_argument_list& args, const assignment_list& indices)
      {
        this->validate(args);
        this->validate(indices);
        return this->unroll(args, indices);
      }


    void replacement_rule_index::pre(const macro_argument_list& args)
      {
        this->validate(args);
        this->pre_hook(args);
      }


    void replacement_rule_index::post(const macro_argument_list& args)
      {
        this->validate(args);
        this->post_hook(args);
      }


    std::string
    replacement_rule_index::evaluate_roll(const macro_argument_list& args, const index_literal_list& indices)
      {
        this->validate(args);
        this->validate(indices);
        return this->roll(args, indices);
      }
    
    
    void replacement_rule_index::validate(const macro_argument_list& args)
      {
        if(args.size() == this->num_args) return;

        std::ostringstream msg;
    
        msg << ERROR_MACRO_WRONG_ARGUMENT_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_ARGUMENT_COUNT << " "
            << this->num_args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
        throw argument_mismatch(msg.str());
      }
    


    template <typename IndexDatabase>
    void replacement_rule_index::validate(const IndexDatabase& indices)
      {
        // check that correct number of indices have been supplied
        if(indices.size() != this->num_indices)
          {
            std::ostringstream msg;
    
            msg << ERROR_MACRO_WRONG_INDEX_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_INDEX_COUNT << " "
                << this->num_indices << ", " << ERROR_RECEIVED_INDEX_COUNT << " " << indices.size();
            throw index_mismatch(msg.str());
          }

        // check that index types are compatible
        if(!this->idx_class) return;
    
        for(const auto& T : indices)
          {
            index_class cl = index_traits_impl::get_index_class<decltype(T)>(T);
            char lb = index_traits_impl::get_index_label<decltype(T)>(T);
        
            switch(*this->idx_class)
              {
                case index_class::full:
                  {
                    // full is compatible with either full or field_only
                    if(cl != index_class::full && cl != index_class::field_only)
                      {
                        std::ostringstream msg;
                    
                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '"
                            << lb << "'";
                        throw index_mismatch(msg.str());
                      }
                
                    break;
                  }
            
                case index_class::field_only:
                  {
                    // field_only is compatible only with field_only
                
                    if(cl != index_class::field_only)
                      {
                        std::ostringstream msg;
                    
                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '"
                            << lb << "'";
                        throw index_mismatch(msg.str());
                      }
                
                    break;
                  }
            
                case index_class::parameter:
                  {
                    // parameter is compatible only with parameter
                
                    if(cl != index_class::parameter)
                      {
                        std::ostringstream msg;
                    
                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '"
                            << lb << "'";
                        throw index_mismatch(msg.str());
                      }
                
                    break;
                  }
              }
          }
      }

  }   // namespace macro_packages
