//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include "boost/algorithm/string.hpp"

#include "macro.h"
#include "macro_tokenizer.h"


// **************************************************************************************


macro_agent::macro_agent(translator_data& p, package_group& pkg, std::string pf, std::string speq, std::string spsumeq, unsigned int dm)
  : data_payload(p),
    prefix(std::move(pf)),
    split_equal(std::move(speq)),
    split_sum_equal(std::move(spsumeq)),
    recursion_max(dm),
    recursion_depth(0),
    pre_rule_cache(pkg.get_pre_ruleset()),
    post_rule_cache(pkg.get_post_ruleset()),
    index_rule_cache(pkg.get_index_ruleset())
  {
    assert(recursion_max > 0);

		// pause timers
		timer.stop();
		tokenization_timer.stop();

    if(recursion_max == 0) recursion_max = 1;

    fields     = data_payload.get_number_fields();
    parameters = data_payload.get_number_parameters();
    order      = data_payload.get_index_order();
  }


std::unique_ptr< std::list<std::string> > macro_agent::apply(std::string& line, unsigned int& replacements)
  {
		// if timer is stopped, restart it
		bool stopped = this->timer.is_stopped();
		if(stopped) timer.resume();

		// the result of macro substitution is potentially large, and we'd rather not copy
		// a very large array of strings while moving the result around.
		// So, use a std::unique_ptr<> to manage the result object
    std::unique_ptr< std::list<std::string> > r_list;

    if(++this->recursion_depth < this->recursion_max)
      {
        r_list = this->apply_line(line, replacements);
        --this->recursion_depth;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_RECURSION_DEPTH << "=" << this->recursion_max << ")";

        error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        err_context.warn(msg.str());
      }

		// if timer was stopped, stop it again
		if(stopped) timer.stop();

    return(r_list);
  }


std::unique_ptr< std::list<std::string> > macro_agent::apply_line(const std::string& line, unsigned int& replacements)
  {
    std::unique_ptr< std::list<std::string> > r_list = std::make_unique< std::list<std::string> >();

		// break the line at the split point, if it exists, to get a 'left-hand' side and a 'right-hand' side
    macro_impl::split_string split_result = this->split(line);

		tokenization_timer.resume();
		token_list left_tokens(split_result.left, this->prefix, this->fields, this->parameters, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache, this->data_payload);
		token_list right_tokens(split_result.right, this->prefix, this->fields, this->parameters, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache, this->data_payload);
		tokenization_timer.stop();

    // running total of number of macro replacements
    unsigned int counter = 0;

    // evaluate pre macros and cache the results
		// we'd like to do this only once if possible, because evaluation may be expensive
		counter += left_tokens.evaluate_macros(simple_macro_type::pre);
		counter += right_tokens.evaluate_macros(simple_macro_type::pre);

    // generate assignments for the LHS indices
    assignment_set LHS_assignments(left_tokens.get_indices());

		// generate an assignment for each RHS index; first get RHS indices which are not also LHS indices
    abstract_index_list RHS_indices;
    error_context ctx(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());

    try
      {
        RHS_indices = right_tokens.get_indices() - left_tokens.get_indices();
      }
    catch(index_exception& xe)
      {
        std::ostringstream msg;
        msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
        ctx.error(msg.str());
      }

    assignment_set RHS_assignments(RHS_indices);

    // LHS_assignments may be *empty* (ie size=0) if there are no valid assignments (probably one of the index
    // ranges is empty).
    // If there are no LHS indices then it will report size=1, ie. the trivial index assignment
    // It's important to distinguish these two cases!
		if(LHS_assignments.size() > 0)
			{
        for(std::unique_ptr<assignment_list> LHS_assign : LHS_assignments)
			    {
				    // evaluate LHS macros on this index assignment;
            // only need index- and post-macros; pre-macros were evaluated earlier
				    counter += left_tokens.evaluate_macros(*LHS_assign);
						counter += left_tokens.evaluate_macros(simple_macro_type::post);

						if(RHS_assignments.size() > 1)   // multiple RHS assignments
							{
						    // push the LHS evaluated on this assignment into the output list, if it is non-empty
						    if(left_tokens.size() > 0)
							    {
                    std::string lhs = left_tokens.to_string();
                    if(split_result.type == macro_impl::split_type::sum)       lhs += " = ";
                    if(split_result.type == macro_impl::split_type::sum_equal) lhs += " += ";
						        r_list->push_back(lhs);
							    }

						    // now generate a set of RHS evaluations for this LHS evaluation
                for(std::unique_ptr<assignment_list> RHS_assign : RHS_assignments)
							    {
                    assignment_list total_assignment;
                    try
                      {
                        total_assignment = *LHS_assign + *RHS_assign;
                      }
                    catch(index_exception& xe)
                      {
                        std::ostringstream msg;
                        msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
                        ctx.error(msg.str());
                      }

						        counter += right_tokens.evaluate_macros(total_assignment);
						        counter += right_tokens.evaluate_macros(simple_macro_type::post);

								    // set up replacement right hand side; add trailing ; and , only if the LHS is empty
						        std::string this_line = right_tokens.to_string() + (left_tokens.size() == 0 && split_result.semicolon ? ";" : "") + (left_tokens.size() == 0 && split_result.comma ? "," : "");

						        r_list->push_back(this_line);
							    }

								// add a trailing ; and , if the LHS is nonempty
						    if(left_tokens.size() > 0 && r_list->size() > 0)
							    {
						        if(split_result.semicolon)
							        {
						            r_list->back() += ";";
							        }
						        if(split_result.comma)
							        {
						            r_list->back() += ",";
							        }
							    }
							}
						else if(RHS_assignments.size() == 1)  // just one RHS assignment, so coalesce with LHS
							{
                std::unique_ptr<assignment_list> RHS_assign = *RHS_assignments.begin();

                assignment_list total_assignment;
                try
                  {
                    total_assignment = *LHS_assign + *RHS_assign;
                  }
                catch(index_exception& xe)
                  {
                    std::ostringstream msg;
                    msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
                    ctx.error(msg.str());
                  }

						    counter += right_tokens.evaluate_macros(total_assignment);
						    counter += right_tokens.evaluate_macros(simple_macro_type::post);

								// set up line with macro replacements, and add trailing ; and , if necessary
						    std::string full_line = left_tokens.to_string();
                if(split_result.type == macro_impl::split_type::sum)       full_line += " =";
                if(split_result.type == macro_impl::split_type::sum_equal) full_line += " +=";

                full_line += " " + right_tokens.to_string() + (split_result.semicolon ? ";" : "") + (split_result.comma ? "," : "");
								r_list->push_back(full_line);
							}
			    }
			}
		else
			{
				r_list->push_back(std::string("// Skipped: empty index range (LHS index set is empty)"));
			}

    replacements = counter;

    return(r_list);
	}


macro_impl::split_string macro_agent::split(const std::string& line)
  {
    macro_impl::split_string rval;

    // check for sum split point
    if((rval.split_point = line.find(this->split_equal)) != std::string::npos)
      {
        rval.left  = line.substr(0, rval.split_point);
        rval.right = line.substr(rval.split_point + this->split_equal.length());
        rval.type  = macro_impl::split_type::sum;
      }
    else    // not sum, but possibly sum-equals?
      {
        if((rval.split_point = line.find(this->split_sum_equal)) != std::string::npos)
          {
            rval.left  = line.substr(0, rval.split_point);
            rval.right = line.substr(rval.split_point + this->split_sum_equal.length());
            rval.type  = macro_impl::split_type::sum_equal;
          }
        else    // no split-point; everything counts as the right-hand side
          {
            rval.right = line;
            rval.type  = macro_impl::split_type::none;
          }
      }

    // trim trailing white space on the right-hand side
    boost::algorithm::trim_right(rval.right);

    // check if the last component is a semicolon
    // note std:string::back() and std::string::pop_back() require C++11
    if(rval.right.size() > 0 && rval.right.back() == ';')
      {
        rval.semicolon = true;
        rval.right.pop_back();
      }

    // check if the last component is a comma
    if(rval.right.size() > 0 && rval.right.back() == ',')
      {
        rval.comma = true;
        rval.right.pop_back();
      }

    return(rval);
  }
