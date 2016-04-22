//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef __time_series_table_H_
#define __time_series_table_H_


#include "transport-runtime/derived-products/line-collections/line_asciitable.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"


namespace transport
	{

		namespace derived_data
			{

				//! time-series-table is a convenience class which defines a line-asciitable with
				//! sensible defaults for time series

				template <typename number>
		    class time_series_table: public line_asciitable<number>
			    {

		      public:

				    // CONSTRUCTOR, DESTRUCTOR

						//! Basic user-facing constructor. Accepts a filename.
				    time_series_table(const std::string& name, const boost::filesystem::path& filename);

				    virtual ~time_series_table() = default;


				    // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels(bool x_label_set=true) override;

			    };


				template <typename number>
				time_series_table<number>::time_series_table(const std::string& name, const boost::filesystem::path& filename)
					: line_asciitable<number>(name, filename)
					{
						apply_default_labels();
					}


				template <typename number>
				void time_series_table<number>::apply_default_labels(bool x_label_set)
					{
				    axis_value x_axis = this->get_x_axis_value();

						if(x_label_set)
							{
						    switch(x_axis)
							    {
						        case axis_value::unset_axis:
						        case axis_value::efolds_axis:
							        this->internal_set_x_label(CPPTRANSPORT_PRODUCT_TIME_SERIES_TABLE_X_LABEL_NOLATEX);
							        break;

						        default:
							        assert(false);
							    }

							}
					}

			}   // namespace derived_data

	}   // namespace transport



#endif //__time_series_table_H_