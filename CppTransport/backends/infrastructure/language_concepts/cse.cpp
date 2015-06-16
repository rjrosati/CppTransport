//
// Created by David Seery on 13/11/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

// based on GinacPrint by Doug Baker
// original copyright notice:

//***************************************************************************
//* @file GinacPrint.cc
//*
// * Source file for a class that prints GiNaC expressions in a form that
//* is efficient to compile and efficient to run.  The main work this
//* class does is identifying shared sub-expressions to generate code
//* that only computes them once.
//*
//* See GinacPrint.hh for details on usage.
//*
//* Copyright 2010
//*
//* National Robotics Engineering Center, Carnegie Mellon University
//* 10 40th Street, Pittsburgh, PA 15201
//* www.rec.ri.cmu.edu
//*
//* This program is free software; you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation; either version 2 of the License, or
//* (at your option) any later version.
//*
//* This program is distributed in the hope that it will be useful, but
//* WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//* General Public License for more details.
//*
//* This notice must appear in all copies of this file and its derivatives.
//*
//* History of Significant Contributions (don't put commit logs here):
//* 2010-05-20 Doug Baker, cape1232@yahoo.com: Created.
//*
//***************************************************************************


#include <iostream>
#include <sstream>
#include <map>
#include <assert.h>
#include <functional>

#include "msg_en.h"

#include "error.h"
#include "cse.h"


// **********************************************************************


void cse::clear()
  {
    this->symbols.clear();
    this->decls.clear();

    this->serial_number++;
  }


// **********************************************************************


void cse::parse(const GiNaC::ex& expr)
  {
		timer.resume();

    for(GiNaC::const_postorder_iterator t = expr.postorder_begin(); t != expr.postorder_end(); ++t)
      {
        symbol_f symf = std::bind(&cse::get_symbol_no_tag, this, std::placeholders::_1);

        std::string e = this->print(*t, symf);
        symbol_record& record = this->symbols[e];

        if(record.filled == false)
          {
            record.filled = true;
            record.symbol = this->make_symbol();
            record.target = e;
          }
      }

		timer.stop();
  }


std::string cse::temporaries(const std::string& t)
  {
    std::ostringstream out;
    bool ok = true;

    size_t lhs_pos;
    size_t rhs_pos;

    if((lhs_pos = t.find("$1")) == std::string::npos)
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_LHS << " '" << t << "'";
        error(msg.str());
        ok = false;
      }
    if((rhs_pos = t.find("$2")) == std::string::npos)
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_RHS << " '" << t << "'";
        error(msg.str());
        ok = false;
      }

    if(ok)
      {
        // deposit each declaration into the output stream
        for(size_t i = 0; i < this->decls.size(); ++i)
          {
            std::string line = t;

            // replace LHS and RHS macros in the template
            if((lhs_pos = line.find("$1")) != std::string::npos) line.replace(lhs_pos, (size_t)2, this->decls[i].first);
            if((rhs_pos = line.find("$2")) != std::string::npos) line.replace(rhs_pos, (size_t)2, this->decls[i].second);
            out << line << std::endl;
          }
      }
    else
      {
        out << t << std::endl;
      }

    return(out.str());
  }


// **********************************************************************


std::string cse::get_symbol_no_tag(const GiNaC::ex& expr)
  {
    symbol_f symf = std::bind(&cse::get_symbol_no_tag, this, std::placeholders::_1);

    std::string e = this->print(expr, symf);
    symbol_record& record = this->symbols[e];

    std::string rval = e;
    if(record.filled)
      {
        rval = record.symbol;
      }

    return(rval);
  }


std::string cse::get_symbol_and_tag(const GiNaC::ex& expr)
  {
    symbol_f symf = std::bind(&cse::get_symbol_and_tag, this, std::placeholders::_1);

    std::string e = this->print(expr, symf);
    symbol_record& record = this->symbols[e];

    std::string rval = e;
    if(record.filled)
      {
        rval = record.symbol;
        if(!record.written)
          {
            this->decls.push_back(std::make_pair(record.symbol, record.target));
            record.written = true;
          }
      }

    return(rval);
  }


std::string cse::make_symbol()
  {
    std::ostringstream s;

    s << this->temporary_name_kernel << "_" << serial_number << "_" << symbol_counter;
    symbol_counter++;

    return(s.str());
  }


// **********************************************************************


cse_map::cse_map(std::vector<GiNaC::ex>* l, cse* c)
  : list(l), cse_worker(c)
  {
    assert(l != nullptr);
    assert(c != nullptr);

		// if doing CSE, parse the whole vector of expressions
    if(cse_worker->get_perform_cse())
      {
        // parse each component of the container
        for(int i = 0; i < list->size(); ++i)
          {
            cse_worker->parse((*list)[i]);
          }
      }
  }


cse_map::~cse_map()
  {
    delete list;
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    if(index < this->list->size())
      {
        if(this->cse_worker->get_perform_cse())
          {
            rval = this->cse_worker->get_symbol_and_tag((*this->list)[index]);
          }
        else
          {
            rval = (this->cse_worker->get_ginac_printer()).ginac((*this->list)[index]);
          }
      }

    return(rval);
  }