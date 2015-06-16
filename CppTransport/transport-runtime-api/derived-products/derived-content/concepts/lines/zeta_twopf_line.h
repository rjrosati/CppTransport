//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_twopf_line_H_
#define __zeta_twopf_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_ROOT          "zeta-twopf-line-settings"
#define __CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_DIMENSIONLESS "dimensionless"


namespace transport
	{

		namespace derived_data
			{

				//! general zeta twopf content producer, suitable
				//! for producing content usable in eg. a 2d plot or table.
				//! Note that we derive virtually from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and zeta_twopf_line<>
				template <typename number>
		    class zeta_twopf_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

          public:

				    //! Basic user-facing constructor
				    zeta_twopf_line(const zeta_twopf_list_task<number>& tk);

				    //! Deserialization constructor
				    zeta_twopf_line(Json::Value& reader);

				    virtual ~zeta_twopf_line() = default;


				    // SETTINGS

		      public:

				    //! is this dimensionles?
				    bool is_dimensionless() const { return(this->dimensionless); }

				    //! set dimensionless
				    void set_dimensionless(bool g) { this->dimensionless = g; }


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label(void) const;

				    //! make a non-LaTeX label
				    std::string make_non_LaTeX_label(void) const;


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! Serialize this object
				    virtual void serialize(Json::Value& writer) const override;


				    // INTERNAL DATA

		      protected:

				    //! compute the dimensionless twopf?
				    bool dimensionless;

			    };


				template <typename number>
				zeta_twopf_line<number>::zeta_twopf_line(const zeta_twopf_list_task<number>& tk)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        dimensionless(false)
					{
					}


				template <typename number>
				zeta_twopf_line<number>::zeta_twopf_line(Json::Value& reader)
					: derived_line<number>(reader)  // not called because of virtual inheritance; here to silence Intel compiler warning
					{
						dimensionless = reader[__CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_ROOT][__CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_DIMENSIONLESS].asBool();
					}


				template <typename number>
				std::string zeta_twopf_line<number>::make_LaTeX_label(void) const
					{
						if(this->dimensionless)
							{
								return( std::string(__CPP_TRANSPORT_LATEX_DIMENSIONLESS_PZETA) );
							}
						else
							{
						    return( std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) );
							}
					}


				template <typename number>
				std::string zeta_twopf_line<number>::make_non_LaTeX_label(void) const
					{
						if(this->dimensionless)
							{
								return( std::string(__CPP_TRANSPORT_NONLATEX_DIMENSIONLESS_PZETA) );
							}
						else
							{
						    return( std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL) );
							}
					}


				template <typename number>
				void zeta_twopf_line<number>::serialize(Json::Value& writer) const
					{
						writer[__CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_ROOT][__CPP_TRANSPORT_NODE_PRODUCT_ZETA_TWOPF_LINE_DIMENSIONLESS] = this->dimensionless;
					}


				template <typename number>
				void zeta_twopf_line<number>::write(std::ostream& out)
					{
				    out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_TWOPF << std::endl;
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__zeta_twopf_line_H_