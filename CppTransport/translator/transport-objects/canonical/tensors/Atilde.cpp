//
// Created by David Seery on 07/08/2016.
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

#include "Atilde.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor>
    Atilde::compute(const index_literal_list& indices, symbol_wrapper& k1, symbol_wrapper& k2,
                    symbol_wrapper& k3, symbol_wrapper& a)
      {
        if(indices.size() != A_TILDE_TENSOR_INDICES) throw tensor_exception("Atilde indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(A_TILDE_TENSOR_INDICES));

        const field_index max_i = this->shared.get_max_field_index(indices[0]->get_variance());
        const field_index max_j = this->shared.get_max_field_index(indices[1]->get_variance());
        const field_index max_k = this->shared.get_max_field_index(indices[2]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(field_index i = field_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            for(field_index j = field_index(0, indices[1]->get_variance()); j < max_j; ++j)
              {
                for(field_index k = field_index(0, indices[2]->get_variance()); k < max_k; ++k)
                  {
                    (*result)[this->fl.flatten(i, j, k)] = this->compute_component(i, j, k, k1, k2, k3, a);
                  }
              }
          }

        return(result);
      }
    
    
    GiNaC::ex Atilde::compute_component(field_index i, field_index j, field_index k,
                                        symbol_wrapper& k1, symbol_wrapper& k2, symbol_wrapper& k3, symbol_wrapper& a)
      {
        if(!this->cached) throw tensor_exception("Atilde cache not ready");

        unsigned int index = this->fl.flatten(i, j, k);
        auto args = this->res.generate_cache_arguments(use_dV | use_ddV | use_dddV, this->printer);
        args += { k1, k2, k3, a };

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::Atilde_item, index, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto& deriv_i = (*derivs)[this->fl.flatten(i)];
            auto& deriv_j = (*derivs)[this->fl.flatten(j)];
            auto& deriv_k = (*derivs)[this->fl.flatten(k)];

            auto& Vijk = (*dddV)[this->fl.flatten(i,j,k)];

            auto& Vij  = (*ddV)[this->fl.flatten(i,j)];
            auto& Vjk  = (*ddV)[this->fl.flatten(j,k)];
            auto& Vik  = (*ddV)[this->fl.flatten(i,k)];

            auto& Vi   = (*dV)[this->fl.flatten(i)];
            auto& Vj   = (*dV)[this->fl.flatten(j)];
            auto& Vk   = (*dV)[this->fl.flatten(k)];

            auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
            auto idx_j = this->shared.generate_index<GiNaC::idx>(j);
            auto idx_k = this->shared.generate_index<GiNaC::idx>(k);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::Atilde_item, index, args, result);
          }

        return(result);
      }
    
    
    GiNaC::ex Atilde::expr(const GiNaC::idx& i, const GiNaC::idx& j, const GiNaC::idx& k,
                           const GiNaC::ex& Vijk, const GiNaC::ex& Vij, const GiNaC::ex& Vjk, const GiNaC::ex& Vik,
                           const GiNaC::ex& Vi, const GiNaC::ex& Vj, const GiNaC::ex& Vk,
                           const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const GiNaC::ex& deriv_k,
                           const symbol_wrapper& k1, const symbol_wrapper& k2, const symbol_wrapper& k3,
                           const symbol_wrapper& a)
      {
        GiNaC::ex xi_i = -2*(3-eps) * deriv_i - 2 * Vi/Hsq;
        GiNaC::ex xi_j = -2*(3-eps) * deriv_j - 2 * Vj/Hsq;
        GiNaC::ex xi_k = -2*(3-eps) * deriv_k - 2 * Vk/Hsq;

        GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2) / 2;
        GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3) / 2;
        GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3) / 2;

        GiNaC::ex result = - Vijk / (3*Hsq);

        result += - (deriv_i / (2*Mp*Mp)) * (Vjk / (3*Hsq))
                  - (deriv_j / (2*Mp*Mp)) * (Vik / (3*Hsq))
                  - (deriv_k / (2*Mp*Mp)) * (Vij / (3*Hsq));

        result += + (deriv_i * deriv_j * xi_k) / (8 * 3 * Mp*Mp*Mp*Mp)
                  + (deriv_j * deriv_k * xi_i) / (8 * 3 * Mp*Mp*Mp*Mp)
                  + (deriv_i * deriv_k * xi_j) / (8 * 3 * Mp*Mp*Mp*Mp);

        result += + (deriv_i * xi_j * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k2dotk3*k2dotk3 / (k2*k2 * k3*k3)) / 3
                  + (deriv_j * xi_i * xi_k) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk3*k1dotk3 / (k1*k1 * k3*k3)) / 3
                  + (deriv_k * xi_i * xi_j) / (32*Mp*Mp*Mp*Mp) * (1 - k1dotk2*k1dotk2 / (k1*k1 * k2*k2)) / 3;

        result +=( deriv_i * deriv_j * deriv_k ) / (8 * Mp*Mp*Mp*Mp) * (6 - 2*eps);

        return(result);
      }
    
    
    unroll_state Atilde::get_unroll(const index_literal_list& idx_list)
      {
        if(this->shared.can_roll_coordinates() && this->res.can_roll_dV() && this->res.can_roll_ddV() &&
          this->res.can_roll_dddV()) return unroll_state::allow;

        return unroll_state::force;   // can't roll-up
      }


    std::unique_ptr<atomic_lambda>
    Atilde::compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k,
                           symbol_wrapper& k1, symbol_wrapper& k2, symbol_wrapper& k3, symbol_wrapper& a)
      {
        if(i.get_class() != index_class::field_only) throw tensor_exception("Atilde");
        if(j.get_class() != index_class::field_only) throw tensor_exception("Atilde");
        if(k.get_class() != index_class::field_only) throw tensor_exception("Atilde");

        auto idx_i = this->shared.generate_index<GiNaC::idx>(i);
        auto idx_j = this->shared.generate_index<GiNaC::idx>(j);
        auto idx_k = this->shared.generate_index<GiNaC::idx>(k);

        auto args = this->res.generate_cache_arguments(use_dV | use_ddV | use_dddV, this->printer);
        args += { k1, k2, k3, a };
        args += { idx_i, idx_j, idx_k };

        this->pre_lambda();

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::Atilde_lambda, 0, args, result))
          {
            timing_instrument timer(this->compute_timer);

            auto deriv_i = this->res.generate_deriv_vector(i, this->printer);
            auto deriv_j = this->res.generate_deriv_vector(j, this->printer);
            auto deriv_k = this->res.generate_deriv_vector(k, this->printer);

            auto Vijk = this->res.dddV_resource(i, j, k, this->printer);

            auto Vij  = this->res.ddV_resource(i, j, this->printer);
            auto Vjk  = this->res.ddV_resource(j, k, this->printer);
            auto Vik  = this->res.ddV_resource(i, k, this->printer);

            auto Vi   = this->res.dV_resource(i, this->printer);
            auto Vj   = this->res.dV_resource(j, this->printer);
            auto Vk   = this->res.dV_resource(k, this->printer);

            result = this->expr(idx_i, idx_j, idx_k, Vijk, Vij, Vjk, Vik, Vi, Vj, Vk,
                                deriv_i, deriv_j, deriv_k, k1, k2, k3, a);

            this->cache.store(expression_item_types::Atilde_lambda, 0, args, result);
          }

        return std::make_unique<atomic_lambda>(i, j, k, result, expression_item_types::Atilde_lambda, args, this->shared.generate_working_type());
      }
    
    
    Atilde::Atilde(language_printer& p, cse& cw, expression_cache& c, resources& r,
                   shared_resources& s, boost::timer::cpu_timer& tm, index_flatten& f,
                   index_traits& t)
      : ::Atilde(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        compute_timer(tm),
        cached(false),
        Mp(s.generate_Mp())
      {
      }
    
    
    void Atilde::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("Atilde already cached");

        this->pre_lambda();
        derivs = this->res.generate_deriv_vector(this->printer);
        dV = this->res.dV_resource(this->printer);
        ddV = this->res.ddV_resource(this->printer);
        dddV = this->res.dddV_resource(this->printer);

        this->cached = true;
      }
    
    
    void Atilde::pre_lambda()
      {
        Hsq = this->res.Hsq_resource(this->cse_worker, this->printer);
        eps = this->res.eps_resource(this->cse_worker, this->printer);
      }
    
    
    void Atilde::post()
      {
        if(!this->cached) throw tensor_exception("Atilde not cached");

        // invalidate cache
        this->cached = false;
      }

  }   // namespace canonical
