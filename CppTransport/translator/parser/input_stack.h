//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_INPUT_STACK_H
#define CPPTRANSPORT_INPUT_STACK_H


#include <string>
#include <deque>

#include "filestack.h"

#include "boost/filesystem/operations.hpp"


class input_stack: public filestack_derivation_helper<input_stack>
  {
  public:

    // data structure for tracking the source of any lexeme
    struct inclusion
      {
        boost::filesystem::path  name;
        unsigned int             line;
      };

    virtual ~input_stack() = default;

    void         push                  (const boost::filesystem::path name);

    virtual void         set_line      (unsigned int line) override;
    virtual unsigned int increment_line() override;
    virtual unsigned int get_line      () const override;

    virtual void         pop           () override;

    virtual std::string  write         (size_t level) const override;
    virtual std::string  write         () const override;

  protected:

    std::deque<struct inclusion> inclusions;

  };


#endif //CPPTRANSPORT_INPUT_STACK_H