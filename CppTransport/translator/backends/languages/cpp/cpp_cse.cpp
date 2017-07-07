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


#include <string>
#include <sstream>

#include "cse.h"
#include "cpp_cse.h"
#include "error.h"
#include "msg_en.h"


namespace cpp
  {
    
    const std::map< std::string, std::string > func_convert
      {
        {"abs", "std::abs"},
        {"sqrt", "std::sqrt"},
        {"sin", "std::sin"},
        {"cos", "std::cos"},
        {"tan", "std::tan"},
        {"asin", "std::asin"},
        {"acos", "std::acos"},
        {"atan", "std::atan"},
        {"atan2", "std::atan2"},
        {"sinh", "std::sinh"},
        {"cosh", "std::cosh"},
        {"tanh", "std::tanh"},
        {"asinh", "std::asinh"},
        {"acosh", "std::acosh"},
        {"atanh", "std::atanh"},
        {"exp", "std:exp"},
        {"log", "std::log"},
        {"pow", "std::pow"},
        {"tgamma", "std::tgamma"},
        {"lgamma", "std::lgamma"}
      };
    

    std::string cpp_cse::print(const GiNaC::ex& expr, bool use_count)
      {
        std::string name;
    
        if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
        else name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

        if     (name == "numeric") return this->printer.ginac(expr);
        else if(name == "symbol")  return this->printer.ginac(expr);
        else if(name == "add")     return this->print_operands(expr, "+", use_count);
        else if(name == "mul")     return this->print_operands(expr, "*", use_count);
        else if(name == "power")   return this->print_power(expr, use_count);

        // not a standard operation, so assume it must be a special function
        // look up it's C++ form in func_map, and then format its arguments,
        // taking care to keep track of use counts
        
        auto t = func_convert.find(name);
        if(t == func_convert.end())
          {
            std::ostringstream msg;
            msg << ERROR_UNIMPLEMENTED_MATHS_FUNCTION << " '" << name << "'";
            throw cse_exception(msg.str());
          }

        std::string rval{t->second};
        rval.append("(");
        rval.append(this->print_operands(expr, ",", use_count));
        rval.append(")");

        return rval;
      }


    std::string cpp_cse::print_operands(const GiNaC::ex& expr, std::string op, bool use_count)
      {
        std::string rval;

        unsigned int c = 0;
        for(auto t = expr.begin(); t != expr.end(); ++t)
          {
            if(c > 0) rval.append(op);
    
            if(use_count) rval.append(this->get_symbol_with_use_count(*t));
            else          rval.append(this->get_symbol_without_use_count(*t));

            ++c;
          }
        
        return rval;
      }


    // treat powers specially, because it's better to unroll them and
    // avoid a function call if we have an exact integral power
    std::string cpp_cse::print_power(const GiNaC::ex& expr, bool use_count)
      {
        std::ostringstream out;
        size_t n = expr.nops();

        if(n != 2)
          {
            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.error(ERROR_CSE_POWER_ARGUMENTS);
            out << "std::pow(" << this->print_operands(expr, ",", use_count) << ")";
          }
        else
          {
            const GiNaC::ex& exp_generic = expr.op(1);

            if(GiNaC::is_a<GiNaC::numeric>(exp_generic))
              {
                const auto& exp_numeric = GiNaC::ex_to<GiNaC::numeric>(exp_generic);

                std::string sym;
                if(use_count) sym = this->get_symbol_with_use_count(expr.op(0));
                else          sym = this->get_symbol_without_use_count(expr.op(0));

                if(GiNaC::is_integer(exp_numeric))
                  {

                    if(GiNaC::is_nonneg_integer(exp_numeric))
                      {
                        if      (exp_numeric.to_int() == 0) out << "1.0";
                        else if (exp_numeric.to_int() == 1) out << sym;
                        else if (exp_numeric.to_int() == 2) out << sym << "*" << sym;
                        else if (exp_numeric.to_int() == 3) out << sym << "*" << sym << "*" << sym;
                        else if (exp_numeric.to_int() == 4) out << sym << "*" << sym << "*" << sym << "*" << sym;
                        else                                out << "std::pow(" << sym << "," << exp_numeric.to_int() << ")";
                      }
                    else  // negative integer
                      {
                        out << "1.0/";
                        if      (exp_numeric.to_int() == -0) out << "1.0";
                        else if (exp_numeric.to_int() == -1) out << sym;
                        else if (exp_numeric.to_int() == -2) out << "(" << sym << "*" << sym << ")";
                        else if (exp_numeric.to_int() == -3) out << "(" << sym << "*" << sym << "*" << sym << ")";
                        else if (exp_numeric.to_int() == -4) out << "(" << sym << "*" << sym << "*" << sym << "*" << sym << ")";
                        else                                 out << "std::pow(" << sym << "," << -exp_numeric.to_int() << ")";
                      }
                  }
                else  // not an integer
                  {
                    std::string sym1;

                    if(use_count) sym1 = this->get_symbol_with_use_count(expr.op(1));
                    else          sym1 = this->get_symbol_without_use_count(expr.op(1));

                    out << "std::pow(" << sym << "," << sym1 << ")";
                  }
              }
          }

        return(out.str());
      }

  } // namespace cpp
