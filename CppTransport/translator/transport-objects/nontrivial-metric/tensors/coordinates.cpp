//
// Created by David Seery on 16/06/2017.
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

#include "coordinates.h"


namespace nontrivial_metric
  {
    
    std::unique_ptr<flattened_tensor> coordinates::compute(const index_literal_list& indices)
      {
        if(indices.size() != COORDINATE_TENSOR_INDICES) throw tensor_exception("coordinate indices");
        if(indices[0]->get_class() != index_class::full) throw tensor_exception("coordinates");
        if(indices[0]->get_variance() != variance::contravariant) throw tensor_exception(ERROR_COORD_INDICES_ARE_CONTRAVARIANT);
        
        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(COORDINATE_TENSOR_INDICES));
        
        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());

        unsigned deriv_offset = result->size()/2;

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            GiNaC::ex f;
            GiNaC::ex d;
            
            std::tie(f, d) = this->compute_component(i);
            
            (*result)[this->fl.flatten(i)] = f;
            (*result)[this->fl.flatten(i) + deriv_offset] = d;
          }
        
        return result;
      }
    
    
    std::pair<GiNaC::ex, GiNaC::ex> coordinates::compute_component(field_index i)
      {
        if(!this->cached) throw tensor_exception("coordinates cache not ready");
        if(i.get_variance() != variance::contravariant) throw tensor_exception(ERROR_COORD_INDICES_ARE_CONTRAVARIANT);

        unsigned int index = this->fl.flatten(i);

        GiNaC::ex f = (*this->fields)[index];
        GiNaC::ex d = (*this->derivs)[index];
        
        return std::make_pair(f, d);
      }
    
    
    void coordinates::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("coordinates already cached");

        fields = this->res.generate_field_vector(this->printer);
        derivs = this->res.generate_deriv_vector(variance::contravariant, this->printer);

        this->cached = true;
      }
    
    
    void coordinates::post()
      {
        if(!this->cached) throw tensor_exception("coordinates not cached");

        // invalidate cache
        this->cached = false;
      }
    
    
    std::unique_ptr<atomic_lambda> coordinates::compute_lambda(const index_literal& i)
      {
        if(i.get_class() != index_class::full) throw tensor_exception("coordinates");
        if(i.get_variance() != variance::contravariant) throw tensor_exception(ERROR_COORD_INDICES_ARE_CONTRAVARIANT);
        
        auto idx_i = this->shared.generate_index<GiNaC::varidx>(i);
        
        auto args = std::make_unique<cache_tags>();
        
        auto result = this->res.generate_field_vector(i, this->printer);
    
        return std::make_unique<atomic_lambda>(i, result, expression_item_types::coordinates_lambda, *args, this->shared.generate_working_type());
      }
    
    
    unroll_state coordinates::get_unroll(const index_literal_list& idx_list)
      {
        if(this->shared.can_roll_coordinates()) return unroll_state::allow;
        return unroll_state::force;
      }
    
    
    coordinates::coordinates(language_printer& p, cse& cw, resources& r, shared_resources& s,
                             index_flatten& f, index_traits& t)
      : ::coordinates(),
        printer(p),
        cse_worker(cw),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        cached(false)
      {
      }

  }   // namespace nontrivial_metric
