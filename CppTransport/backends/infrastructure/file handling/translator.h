//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __translator_H_
#define __translator_H_


#include "buffer.h"
#include "output_stack.h"


#include "ginac_cache.h"


enum expression_item_types
	{
    sr_U_item, U1_item, U2_item, U3_item, A_item, B_item, C_item, M_item, zxfm1_item, zxfm2_item, dN1_item, dN2_item
	};


// need forward reference to avoid circularity
class translation_unit;

typedef std::function<std::string(const std::string&)> filter_function;

class translator
  {

  public:

		// constructor
    translator(translation_unit* tu);

		// destructor
    ~translator()
	    {
        std::ostringstream msg;
        msg << this->cache.get_hits() << " " << MESSAGE_EXPRESSION_CACHE_HITS << ", " << this->cache.get_misses() << " " << MESSAGE_EXPRESSION_CACHE_MISSES;
        this->print_advisory(msg.str());
	    }


		// INTERFACE

  public:

    // translate, using the data in the supplied translation_unit, from
    // the template 'in' to the template 'out'
    // sometimes we want to supply a buffer, because we share buffers
    // across different input sources
    // otherwise, translate() creates a suitable buffer internally
    unsigned int translate(const std::string in, const std::string out, enum process_type type, filter_function* filter=nullptr);
    unsigned int translate(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter=nullptr);

		void print_advisory(const std::string& msg);

  protected:

    // internal API to process a file
    unsigned int process(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter);

    // do the heavy lifting of actual processing
    unsigned int apply();

    // parse the header line from a template, tokenizing it into 'backend' and 'minimum version' data
    void parse_header_line(const std::string in, const std::string line, std::string& backend, double& minver);


		// INTERNAL DATA

  private:

		//! parent translation unit
    translation_unit* unit;

		//! expression cache for this translation unit
		ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE> cache;

  };


#endif //__translator_H_
