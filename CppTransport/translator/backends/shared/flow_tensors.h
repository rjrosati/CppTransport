//
// Created by David Seery on 04/12/2013.
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


#ifndef CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
#define CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_phase_indices.h"
#include "cse_map_field_indices.h"


namespace macro_packages
  {

    constexpr unsigned int POTENTIAL_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int HUBBLESQ_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int EPSILON_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int PARAMETER_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PARAMETER_TOTAL_INDICES = 1;

    constexpr unsigned int FIELD_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int FIELD_TOTAL_INDICES = 1;
    
    constexpr unsigned int MOMENTA_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int MOMENTA_TOTAL_INDICES = 1;

    constexpr unsigned int COORDINATE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int COORDINATE_TOTAL_INDICES = 1;

    constexpr unsigned int SR_VELOCITY_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int SR_VELOCITY_TOTAL_INDICES = 1;

    constexpr unsigned int DV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DV_TOTAL_INDICES = 1;

    constexpr unsigned int DDV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DDV_TOTAL_INDICES = 2;

    constexpr unsigned int DDDV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DDDV_TOTAL_INDICES = 3;


    class replace_V : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_V(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : replacement_rule_simple(std::move(n), POTENTIAL_TOTAL_ARGUMENTS),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            Hubble_obj = f.make_Hubble(prn, cw);
          }

        //! destructor
        virtual ~replace_V() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

      };


    class replace_Hsq : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_Hsq(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
        : replacement_rule_simple(std::move(n), HUBBLESQ_TOTAL_ARGUMENTS),
          cse_worker(cw),
          lambda_mgr(lm)
          {
            Hubble_obj = f.make_Hubble(prn, cw);
          }

        //! destructor
        virtual ~replace_Hsq() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

      };


    class replace_eps : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_eps(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
        : replacement_rule_simple(std::move(n), EPSILON_TOTAL_ARGUMENTS),
          cse_worker(cw),
          lambda_mgr(lm)
          {
            Hubble_obj = f.make_Hubble(prn, cw);
          }

        //! destructor
        virtual ~replace_eps() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

      };


    class replace_eta : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_eta(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : replacement_rule_simple(std::move(n), EPSILON_TOTAL_ARGUMENTS),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            Hubble_obj = f.make_Hubble(prn, cw);
          }

        //! destructor
        virtual ~replace_eta() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

      };


    class replace_parameter : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_parameter(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : replacement_rule_index(std::move(n), PARAMETER_TOTAL_ARGUMENTS, PARAMETER_TOTAL_INDICES,
                                   std::vector<index_class>({ index_class::parameter })),
            cse_worker(cw),
            lambda_mgr(lm),
            printer(prn),
            fl(f.make_flatten())
          {
            parameter_tensor = f.make_parameters(prn, cw);
          }

        //! destructor
        virtual ~replace_parameter() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->parameter_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        std::string unroll(const macro_argument_list& args, const index_literal_assignment& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;
    
        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! release
        void post_hook(const macro_argument_list&) override { this->map.release(); }
    
    
    
        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;
    
        //! parameter tensor
        std::unique_ptr<parameters> parameter_tensor;
        
        //! map object holds components of tensor after CSE
        std::unique_ptr<cse_map> map;
    
        //! index flattener
        index_flatten fl;

      };


    class replace_field : public cse_map_field1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_field(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field1(std::move(n), FIELD_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            field_tensor = f.make_fields(prn, cw);
          }

        //! destructor
        virtual ~replace_field() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->field_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:
    
        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;
    
        //! field tensor
        std::unique_ptr<fields> field_tensor;

      };
    
    
    class replace_momenta : public cse_map_field1
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        replace_momenta(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field1(std::move(n), MOMENTA_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            momenta_tensor = f.make_momenta(prn, cw);
          }
        
        //! destructor
        virtual ~replace_momenta() = default;
        
        
        // INTERFACE
      
      public:
        
        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->momenta_tensor->get_unroll(idx_list); }
        
        
        // INTERNAL API
      
      protected:
        
        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! CSE worker
        cse& cse_worker;
        
        //! lambda manager
        lambda_manager& lambda_mgr;
        
        //! language printer
        language_printer& printer;
        
        //! field tensor
        std::unique_ptr<momenta> momenta_tensor;
        
      };


    class replace_coordinate : public cse_map_phase1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_coordinate(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_phase1(std::move(n), COORDINATE_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            coordinate_tensor = f.make_coordinates(prn, cw);
          }

        //! destructor
        virtual ~replace_coordinate() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->coordinate_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;
        
        //! coordinate tensor
        std::unique_ptr<coordinates> coordinate_tensor;

      };


    class replace_SR_velocity : public cse_map_field1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_SR_velocity(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field1(std::move(n), SR_VELOCITY_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            SR_velocity_tensor = f.make_SR_velocity(prn, cw);
          }

        //! destructor
        virtual ~replace_SR_velocity() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->SR_velocity_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! SR_velocity object
        std::unique_ptr<SR_velocity> SR_velocity_tensor;

      };


    class replace_dV : public cse_map_field1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dV(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field1(std::move(n), DV_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            dV_tensor = f.make_dV(prn, cw);
          }

        //! destructor
        virtual ~replace_dV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->dV_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! dV object
        std::unique_ptr<dV> dV_tensor;

      };


    class replace_ddV : public cse_map_field2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_ddV(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field2(std::move(n), DDV_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            ddV_tensor = f.make_ddV(prn, cw);
          }

        //! destructor
        virtual ~replace_ddV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->ddV_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! ddV object
        std::unique_ptr<ddV> ddV_tensor;

      };


    class replace_dddV : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dddV(std::string n, tensor_factory& f, cse& cw, lambda_manager& lm, language_printer& prn)
          : cse_map_field3(std::move(n), DDDV_TOTAL_ARGUMENTS, f.make_flatten()),
            printer(prn),
            cse_worker(cw),
            lambda_mgr(lm)
          {
            dddV_tensor = f.make_dddV(prn, cw);
          }

        //! destructor
        virtual ~replace_dddV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        unroll_state get_unroll(const index_literal_list& idx_list) const override { return this->dddV_tensor->get_unroll(idx_list); }


        // INTERNAL API

      protected:

        //! evaluate
        void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

        //! evaluate
        std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! dddV object
        std::unique_ptr<dddV> dddV_tensor;

      };


    class flow_tensors : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        flow_tensors(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~flow_tensors() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
