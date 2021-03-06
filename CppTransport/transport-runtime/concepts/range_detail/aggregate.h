//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_RANGE_AGGREGATION_H
#define CPPTRANSPORT_RANGE_AGGREGATION_H


#include <limits>

#include "transport-runtime/concepts/range_detail/common.h"
#include "transport-runtime/concepts/range_detail/abstract.h"

#define CPPTRANSPORT_NODE_SUBRANGE_ARRAY "subranges"


namespace transport
	{

    template <typename value=double> class aggregate_range;

		template <typename value>
		aggregate_range<value> operator+(const aggregate_range<value>& lhs, const range<value>& rhs);

		template <typename value>
		aggregate_range<value> operator+(const range<value>& lhs, const range<value>& rhs);

		// forward-declare helper
		namespace range_helper
			{
				template <typename value> std::unique_ptr< range<value> > deserialize(Json::Value& reader);
			}

		template <typename value>
		class aggregate_range: public range<value>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Construct an aggregation range
				aggregate_range();

				//! Construct an aggregation range from a given subrange
				aggregate_range(const range<value>& a);

				//! Construct an aggregation range from a pair of subranges
				aggregate_range(const range<value>& a, const range<value>& b);

				//! Deserialization constructor
				aggregate_range(Json::Value& reader);

        //! Provide explicit copy constructor to handle deep move on std::unique_ptr
        aggregate_range(const aggregate_range<value>& obj);


				// OVERLOAD ARITHMETIC OPERATORS FOR CONVENIENCE

      public:

				//! assignment
				aggregate_range<value>& operator=(const range<value>& rhs) { this->subrange_list.clear(); this->add_subrange(rhs); return(*this); }

				//! compound addition
				aggregate_range<value>& operator+=(const range<value>& rhs) { this->add_subrange(rhs); return(*this); }

				//! addition
				friend aggregate_range<value> operator+ <>(const aggregate_range<value>& lhs, const range<value>& rhs);
				friend aggregate_range<value> operator+ <>(const range<value>& lhs, const range<value>& rhs);


				// INTERFACE

      public:

		    //! Get minimum entry
		    virtual value get_min()                      override       { if(this->dirty) this->populate_grid(); return(this->min); }

		    //! Get maximum entry
		    virtual value get_max()                      override       { if(this->dirty) this->populate_grid(); return(this->max); }

		    //! Get number of steps
		    virtual unsigned int get_steps()             override       { if(this->dirty) this->populate_grid(); return(this->steps); }

		    //! Get number of entries
		    virtual unsigned int size()                  override       { if(this->dirty) this->populate_grid(); return(this->grid.size()); }

		    //! Is a simple, linear range?
		    virtual bool is_simple_linear()              const override { return(false); }

		    //! Get grid of entries
		    virtual const std::vector<value>& get_grid() override       { if(this->dirty) this->populate_grid(); return(this->grid); }

		    value operator[](unsigned int d)             override;


				// ADD RANGES TO THE AGGREGATION

		  public:

				//! add a subrange
				aggregate_range<value>& add_subrange(const range<value>& s);

        //! get number of subranges
        unsigned int get_number_subranges() const { return(this->subrange_list.size()); }


		    // POPULATE GRID

		  protected:

		    //! fill out the grid entries
		    void populate_grid(void);


		    // CLONE -- implements a 'range<>' interface

		  public:

		    virtual aggregate_range<value>* clone() const override { return new aggregate_range<value>(dynamic_cast<const aggregate_range<value>&>(*this)); }


		    // SERIALIZATION INTERFACE -- implements a 'serializable' interface

		  public:

		    //! Serialize this object
		    virtual void serialize(Json::Value& writer) const override;


				// INTERNAL DATA

		  protected:

				//! dirty flag - if set, grid requires recalculation
				bool dirty;

				//! Minimium value
				value min;

		    //! Maximum value
		    value max;

		    //! Number of steps
		    unsigned int steps;

		    //! Grid of values
		    std::vector<value> grid;

        //! set up typename alias for list of pointers to constituent objects
        typedef typename std::list< std::unique_ptr< range<value> > > subrange_list_type;

				//! List of subranges; use std::unique_ptr to manage lifetime of instances
				subrange_list_type subrange_list;

			};


		template <typename value>
		aggregate_range<value> operator+(const aggregate_range<value>& lhs, const range<value>& rhs)
			{
		    return(aggregate_range<value>(lhs) += rhs);
			}


		template <typename value>
		aggregate_range<value> operator+(const range<value>& lhs, const range<value>& rhs)
			{
				return aggregate_range<value>(lhs, rhs);
			}


		template <typename value>
		aggregate_range<value>::aggregate_range()
			: min(std::numeric_limits<double>::max()),
        max(-std::numeric_limits<double>::max()),
        steps(0),
				dirty(true)
			{
			}


		template <typename value>
		aggregate_range<value>::aggregate_range(const range<value>& a)
			: aggregate_range<value>()
			{
		    this->subrange_list.emplace_back(a.clone());
			}


    template <typename value>
    aggregate_range<value>::aggregate_range(const range<value>& a, const range<value>& b)
	    : aggregate_range<value>()
	    {
        this->subrange_list.emplace_back(a.clone());
        this->subrange_list.emplace_back(b.clone());
	    }


		template <typename value>
		aggregate_range<value>::aggregate_range(Json::Value& reader)
			: aggregate_range<value>()
			{
		    Json::Value array = reader[CPPTRANSPORT_NODE_SUBRANGE_ARRAY];
				assert(array.isArray());

				for(Json::Value::iterator t = array.begin(); t != array.end(); ++t)
					{
						this->subrange_list.emplace_back(range_helper::deserialize<value>(*t));
					}

				this->populate_grid();
			}


    template <typename value>
    aggregate_range<value>::aggregate_range(const aggregate_range<value>& obj)
      : dirty(true),
        min(obj.min),
        max(obj.max),
        steps(obj.steps)
      {
        for(typename subrange_list_type::const_iterator t = obj.subrange_list.begin(); t != obj.subrange_list.end(); ++t)
          {
            if(*t)
              {
                subrange_list.emplace_back((*t)->clone());
              }
          }
      }


		template <typename value>
		void aggregate_range<value>::serialize(Json::Value& writer) const
			{
				writer[CPPTRANSPORT_NODE_RANGE_TYPE] = std::string(CPPTRANSPORT_NODE_RANGE_AGGREGATE);

		    Json::Value array(Json::arrayValue);

				for(typename subrange_list_type::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); ++t)
			    {
				    Json::Value obj(Json::objectValue);
				    (*t)->serialize(obj);
				    array.append(obj);
			    }

				writer[CPPTRANSPORT_NODE_SUBRANGE_ARRAY] = array;
			}


		template <typename value>
		aggregate_range<value>& aggregate_range<value>::add_subrange(const range<value>& s)
			{
				this->subrange_list.emplace_back(s.clone());
				this->dirty = true;

        return *this;
			}


    template <typename value>
    value aggregate_range<value>::operator[](unsigned int d)
	    {
		    if(this->dirty) this->populate_grid();

        assert(d < this->grid.size());
        if(d < this->grid.size())
	        {
            return (this->grid[d]);
	        }
        else
	        {
            throw std::out_of_range(CPPTRANSPORT_RANGE_RANGE);
	        }
	    }


		template <typename value>
		void aggregate_range<value>::populate_grid()
			{
		    this->grid.clear();
		    this->min   = std::numeric_limits<double>::max();
		    this->max   = -std::numeric_limits<double>::max();
		    this->steps = 0;

				// splice all subranges together
				for(typename std::list< std::unique_ptr< range<value> > >::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); ++t)
			    {
				    if((*t)->get_max() > this->max) this->max = (*t)->get_max();
				    if((*t)->get_min() < this->min) this->min = (*t)->get_min();

				    std::vector<value> temp = (*t)->get_grid();
				    this->grid.reserve(this->grid.size() + temp.size());
				    this->grid.insert(this->grid.end(), temp.begin(), temp.end());
			    }

				// sort resulting grid into order and remove duplicates
		    std::sort(this->grid.begin(), this->grid.end());
        auto last = std::unique(this->grid.begin(), this->grid.end(), aggregation_range_impl::DuplicateRemovalPredicate<value>(1E-10));
		    this->grid.erase(last, this->grid.end());

				this->steps = this->grid.size()+1;
				this->dirty = false;
			}


    template <typename value, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, aggregate_range<value>& obj)
      {
        const std::vector<value>& grid = obj.get_grid();

        out << CPPTRANSPORT_AGGREGATION_RANGE_A << obj.get_number_subranges() << " ";
        out << CPPTRANSPORT_AGGREGATION_RANGE_B << '\n';

        out << CPPTRANSPORT_AGGREGATION_RANGE_C << '\n';
        for(unsigned int i = 0; i < grid.size(); ++i)
          {
            out << i << ". " << grid[i] << '\n';
          }

        return(out);
      }

	}


#endif //CPPTRANSPORT_RANGE_AGGREGATION_H
