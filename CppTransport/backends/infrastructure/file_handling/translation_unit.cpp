//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "core.h"
#include "translation_unit.h"
#include "script.h"
#include "output_stack.h"

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"


// FAIL return code for Bison parser
#ifndef FAIL
#define FAIL (1)
#endif


// lexical analyser data


const std::vector<std::string> keyword_table =
  {
    "name", "author", "tag", "field", "potential", "subexpr", "value",
    "parameter", "latex", "core", "implementation", "model",
    "abserr", "relerr", "stepper", "stepsize",
    "background", "perturbations",
    "indexorder", "left", "right",
    "abs", "step", "sqrt", "sin", "cos", "tan",
    "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
    "asinh", "acosh", "atanh", "exp", "log", "pow", "Li2", "Li", "G_func", "S_func",
    "H_func",
    "zeta_func", "zetaderiv", "tgamma_func", "lgamma_func", "beta_func", "psi_func",
    "factorial", "binomial"
                                 };

const std::vector<enum keyword_type> keyword_map =
  {
    keyword_type::name, keyword_type::author, keyword_type::tag,
    keyword_type::field, keyword_type::potential, keyword_type::subexpr,
    keyword_type::value,
    keyword_type::parameter, keyword_type::latex,
    keyword_type::core, keyword_type::impl, keyword_type::model,
    keyword_type::abserr, keyword_type::relerr, keyword_type::k_stepper,
    keyword_type::stepsize,
    keyword_type::background, keyword_type::perturbations,
    keyword_type::indexorder, keyword_type::left, keyword_type::right,
    keyword_type::f_abs, keyword_type::f_step, keyword_type::f_sqrt,
    keyword_type::f_sin, keyword_type::f_cos, keyword_type::f_tan,
    keyword_type::f_asin, keyword_type::f_acos, keyword_type::f_atan,
    keyword_type::f_atan2,
    keyword_type::f_sinh, keyword_type::f_cosh, keyword_type::f_tanh,
    keyword_type::f_asinh, keyword_type::f_acosh, keyword_type::f_atanh,
    keyword_type::f_exp, keyword_type::f_log, keyword_type::f_pow,
    keyword_type::f_Li2,
    keyword_type::f_Li, keyword_type::f_G, keyword_type::f_S, keyword_type::f_H,
    keyword_type::f_zeta, keyword_type::f_zetaderiv, keyword_type::f_tgamma,
    keyword_type::f_lgamma,
    keyword_type::f_beta, keyword_type::f_psi, keyword_type::f_factorial,
    keyword_type::f_binomial
  };

const std::vector<std::string> character_table =
  {
    "{", "}", "(", ")",
    "[", "]", ",", ".", ":", ";",
    "=", "+", "-@binary", "-@unary", "*", "/", "\\", "~",
    "&", "^", "@", "...", "->"
  };

const std::vector<enum character_type> character_map =
  {
    character_type::open_brace, character_type::close_brace,
    character_type::open_bracket, character_type::close_bracket,
    character_type::open_square, character_type::close_square,
    character_type::comma,
    character_type::period, character_type::colon, character_type::semicolon,
    character_type::equals, character_type::plus,
    character_type::binary_minus, character_type::unary_minus,
    character_type::star, character_type::backslash, character_type::foreslash,
    character_type::tilde, character_type::ampersand, character_type::circumflex,
    character_type::ampersat, character_type::ellipsis,
    character_type::rightarrow
                                         };

// keep track of which characters can precede a unary minus
// this is an open bracket '(', and the binary operators
// which bind tighter: *, /, ^
// plus anything which isn't part of an expression
const std::vector<bool> character_unary_context =
  {
    true, true, true, false,
    true, true, true, true, true, true,
    false, false, false, false, true, true, true, true,
    true, true, true, true, true
  };


static std::string  strip_dot_h(const std::string& pathname);
static std::string  leafname   (const std::string& pathname);


translation_unit::translation_unit(boost::filesystem::path file, finder& p, argument_cache& c, local_environment& e)
  : name(file),
    path(p),
    cache(c),
    env(e),
    parse_failed(false),
    lexstream_payload(file,
                      std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                      std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2),
                      path, cache),
    instream(lexstream_payload,
             keyword_table, keyword_map, character_table, character_map, character_unary_context),
    lexer(instream),
    driver(sym_factory, c, e, error_context(stack,
                                            std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                                            std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2))),
    parser(lexer, driver),
    translator_payload(file,
                       std::bind(&translation_unit::context_error, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&translation_unit::context_warn, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&translation_unit::print_advisory, this, std::placeholders::_1),
                       path, stack, sym_factory, driver, cache),
    outstream(translator_payload)
  {
    // construction of 'instream' lexicalizes the input file
    // the instream object owns the list of lexemes, which persist as long as it exists
    // therefore the lexeme list should be persistent while all transactions involving this unit are active

    // dump lexeme stream to output -- for debugging
    // stream->print(std::cerr);

    // construction of lexer, driver and parser performs syntactic analysis

    if(parser.parse() == FAIL || driver.failed())
	    {
        std::ostringstream msg;
        msg << WARNING_PARSING_FAILED << " " << name;
        this->warn(msg.str());
		    parse_failed = true;
	    }

    // dump results of syntactic analysis -- for debugging
    // in.driver.get_script()->print(std::cerr);

    // cache details about this translation unit

    boost::filesystem::path core_output;
    std::string             core_guard;
    boost::filesystem::path implementation_output;
    std::string             implementation_guard;

    if(cache.core_out().length() > 0 )
      {
        core_output = cache.core_out();
      }
    else
      {
        boost::optional< contexted_value<std::string>& > core = driver.get_script().get_core();
        if(core)
          {
            core_output = this->mangle_output_name(name, this->get_template_suffix(*(*core)));
          }
      }
    core_guard = boost::to_upper_copy(leafname(core_output.string()));
    core_guard.erase(boost::remove_if(core_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), core_guard.end());

    if(cache.implementation_out().length() > 0)
      {
        implementation_output = cache.implementation_out();
      }
    else
      {
        boost::optional< contexted_value<std::string>& > impl = driver.get_script().get_implementation();
        if(impl)
          {
            implementation_output = this->mangle_output_name(name, this->get_template_suffix(*(*impl)));
          }
      }
    implementation_guard = boost::to_upper_copy(leafname(implementation_output.string()));
    implementation_guard.erase(boost::remove_if(implementation_guard, boost::is_any_of(INVALID_GUARD_CHARACTERS)), implementation_guard.end());

    this->translator_payload.set_core_implementation(core_output, core_guard, implementation_output, implementation_guard);
  }


// ******************************************************************


unsigned int translation_unit::apply()
  {
    unsigned int rval = 0;

		if(this->parse_failed) return rval;

    const script& s = this->driver.get_script();

    boost::optional< contexted_value<std::string>& > core = s.get_core();
    if(core)
      {
        rval += this->outstream.translate(*(*core), (*core).get_declaration_point(), this->translator_payload.get_core_output().string(), process_core);
      }
    else
      {
        this->error(ERROR_NO_CORE_TEMPLATE);
//        exit(EXIT_FAILURE);
      }

    boost::optional< contexted_value<std::string>& > impl = s.get_implementation();
    if(impl)
      {
        rval += this->outstream.translate(*(*impl), (*core).get_declaration_point(), this->translator_payload.get_implementation_output().string(), process_implementation);
      }
    else
      {
        this->error(ERROR_NO_IMPLEMENTATION_TEMPLATE);
//        exit(EXIT_FAILURE);
      }

    return(rval);
  }


// ******************************************************************


boost::filesystem::path translation_unit::mangle_output_name(const boost::filesystem::path& input, const std::string& tag)
  {
    size_t      pos = 0;
    std::string output;

    boost::filesystem::path h_extension(".h");

    boost::filesystem::path stem = input.stem();
    boost::filesystem::path leaf = stem.leaf().string() + "_" + tag;

    return(leaf.replace_extension(h_extension));
  }


std::string translation_unit::get_template_suffix(std::string input)
  {
    size_t      pos = 0;
    std::string output;

    if((pos = input.find(TEMPLATE_TAG_SUFFIX)) != std::string::npos) output = input.erase(0, pos+1);
    else                                                             output = input;

    return(output);
  }


void translation_unit::print_advisory(const std::string& msg)
	{
		if(this->cache.verbose())
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << msg << '\n';
			}
	}


void translation_unit::error(const std::string& msg)
	{
		::error(msg, this->cache, this->env);
	}


void translation_unit::warn(const std::string& msg)
	{
		::warn(msg, this->cache, this->env);
	}


void translation_unit::context_error(const std::string& msg, const error_context& ctx)
  {
    ::error(msg, this->cache, this->env, ctx);
  }


void translation_unit::context_warn(const std::string& msg, const error_context& ctx)
  {
    ::warn(msg, this->cache, this->env, ctx);
  }


static std::string strip_dot_h(const std::string& pathname)
  {
    std::string rval;

    if(pathname.substr(pathname.length() - 3) == ".h")
      {
        rval = pathname.substr(0, pathname.length() - 3);
      }
    else if(pathname.substr(pathname.length() - 5) == ".hpp")
      {
        rval = pathname.substr(0, pathname.length() - 5);
      }
    else
      {
        rval = pathname;
      }

    return(rval);
  }


static std::string leafname(const std::string& pathname)
  {
    std::string rval = pathname;
    size_t      pos;

    while((pos = rval.find("/")) != std::string::npos)
      {
        rval.erase(0, pos+1);
      }

    while((pos = rval.find(".")) != std::string::npos)
      {
        rval.erase(pos, std::string::npos);
      }

    return(rval);
  }
