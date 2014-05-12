//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <iostream>
#include <string>

#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"

#include "boost/filesystem/operations.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A  "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B  "_%3N.log"


namespace transport
  {

    // forward-declare model to avoid circular inclusion
    template <typename number> class model;

    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    template <typename number>
    class repository
      {

      public:

        //! Types needed for logging
        typedef enum { normal, notification, warning, error, critical } log_severity_level;
        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;

		    //! Read-only/Read-write access to the repository
		    typedef enum { readonly, readwrite } access_type;


		    // DATA CONTAINER WRITE HANDLE

        //! Integration container writer: forms a handle for a data container when writing the output of an integration

        class integration_writer
          {

          public:

            //! Construct an integration container object.
            //! After creation it is not yet associated with anything in the data_manager backend; that must be done later
		        //! by the task_manager, which can depute a data_manager object of its choice to do the work.
            integration_writer(const boost::filesystem::path& dir, const boost::filesystem::path& data,
                               const boost::filesystem::path& log, const boost::filesystem::path& task,
                               const boost::filesystem::path& temp, unsigned int n, unsigned int w)
              : path_to_directory(dir), path_to_data_container(data),
                path_to_log_directory(log), path_to_taskfile(task),
                path_to_temp_directory(temp),
                serial_number(n), worker_number(w),
                data_manager_handle(nullptr), data_manager_taskfile(nullptr)
              {
                std::ostringstream log_file;
                log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;
                boost::filesystem::path log_path = path_to_log_directory / log_file.str();

                boost::shared_ptr< boost::log::core > core = boost::log::core::get();

                std::ostringstream log_file_path;
                boost::shared_ptr< boost::log::sinks::text_file_backend > backend =
                                     boost::make_shared< boost::log::sinks::text_file_backend >( boost::log::keywords::file_name = log_path.string() );

		            // enable auto-flushing of log entries
		            // this degrades performance, but we are not writing many entries and they
		            // will not be lost in the event of a crash
		            backend->auto_flush(true);

                // Wrap it into the frontend and register in the core.
                // The backend requires synchronization in the frontend.
                this->log_sink = boost::shared_ptr< sink_t >(new sink_t(backend));

                core->add_sink(this->log_sink);

                boost::log::add_common_attributes();
              }

            //! Destroy an integration container object
            ~integration_writer()
              {
		            // remove logging objects
                boost::shared_ptr< boost::log::core > core = boost::log::core::get();

                core->remove_sink(this->log_sink);
              }

            //! Set data_manager handle for data container
            template <typename data_manager_type>
            void set_data_manager_handle(data_manager_type data)
              {
                this->data_manager_handle = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
              }

            //! Return data_manager handle for data container

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_handle(data_manager_type* data)
              {
                if(this->data_manager_handle == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_INTCTR_UNSETHANDLE);
                *data = static_cast<data_manager_type>(this->data_manager_handle);
              }

            //! Set data_manager handle for taskfile
            template <typename data_manager_type>
            void set_data_manager_taskfile(data_manager_type data)
              {
                this->data_manager_taskfile = static_cast<void*>(data);   // will fail if data_manager_type not (static)-castable to void*
              }

            //! Return data_manager handle for taskfile

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_taskfile(data_manager_type* data)
              {
                if(this->data_manager_taskfile == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_INTCTR_UNSETTASK);
                *data = static_cast<data_manager_type>(this->data_manager_taskfile);
              }

            //! Return logger
            boost::log::sources::severity_logger<log_severity_level>& get_log() { return(this->log_source); }

            //! Return path to data container
            const boost::filesystem::path& data_container_path() { return(this->path_to_data_container); }

            //! Return path to log directory
            const boost::filesystem::path& log_directory_path() { return(this->path_to_log_directory); }

            //! Return path to task-data container
            const boost::filesystem::path& taskfile_path() { return(this->path_to_taskfile); }

            //! Return path to directory for temporary files
            const boost::filesystem::path& temporary_files_path() { return(this->path_to_temp_directory); }


		        // INTERNAL DATA

          private:

            const boost::filesystem::path path_to_directory;

            const boost::filesystem::path path_to_data_container;
            const boost::filesystem::path path_to_log_directory;
            const boost::filesystem::path path_to_taskfile;
            const boost::filesystem::path path_to_temp_directory;

            const unsigned int            serial_number;
            const unsigned int            worker_number;

            void*                         data_manager_handle;
            void*                         data_manager_taskfile;

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr< sink_t > log_sink;
          };


        // OUTPUT GROUPS

        //! Output group descriptor. Used to enumerate the output groups available for a particular task
        class output_group
	        {

          public:

            //! Create an output_group descriptor
            output_group(unsigned int sn, const std::string& be, const std::string& path, const std::string& ctr,
                         const std::string& ctime, bool lock, const std::list<std::string>& nt)
	            : serial(sn), backend(be), output_path(path), database_path(ctr),
	              created(boost::posix_time::time_from_string(ctime)), locked(lock), notes(nt)
	            {
	            }

            //! Destroy an output_group descriptor
            ~output_group() = default;

		        //! Write self to output stream
		        void write(std::ostream& out) const
			        {
		            out << __CPP_TRANSPORT_OUTPUT_GROUP_SERIAL << " = " << this->serial
			            << " (" << __CPP_TRANSPORT_OUTPUT_GROUP_BACKEND << " '" << this->backend << "'";
		            if(this->locked) out << ", " << __CPP_TRANSPORT_OUTPUT_GROUP_LOCKED;
		            out << ")" << std::endl;
		            out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_ROOT << " = " << this->output_path << std::endl;
		            out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_DATA << " = " << this->database_path << std::endl;
		            out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_CREATED << " = " << this->created << std::endl;

		            unsigned int count = 0;
		            for(std::list<std::string>::const_iterator t = this->notes.begin(); t != this->notes.end(); t++)
			            {
		                out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_NOTE << " " << count << std::endl;
		                out << "    " << *t << std::endl;
		                count++;
			            }
			        }

            // INTERFACE

          public:

            //! Get name of backend used to generate this output group
            const std::string& get_backend() { return(this->backend); }

            //! Get creation time
            const boost::posix_time::ptime& get_creation_time() { return(this->created); }

            //! Get locked flag
            bool get_lock_status() { return(this->locked); }

            //! Get notes
            const std::list<std::string>& get_notes() { return(this->notes); }

            // PRIVATE DATA

          private:

            //! Internal serial number. Not user-visible.
            unsigned int serial;

            //! Name of integration backend used to generate this output group
            std::string backend;

            //! Path of parent directory containing the output, usually residing within the repository
            boost::filesystem::path output_path;

            //! Path to output database
            boost::filesystem::path database_path;

            //! Creation time
            boost::posix_time::ptime created;

            //! Flag indicating whether or not this output group is locked
            bool locked;

            //! Array of strings representing notes attached to this group
            std::list<std::string> notes;
	        };


		    // DATA CONTAINER READ HANDLE

		    //! Integration container reader: forms a handle for a data container when reading the an integration from the database

		    class integration_reader
			    {

		      public:

				    //! Construct an integration reader object.
				    //! After creation, the data container is not yet open; that must be done later
				    //! by the task_manager which can depute a data_manager object of its choice to do the work.
				    integration_reader(const output_group& og)
				      : group(og)
					    {
					    }


				    // INTERNAL DATA

		      private:

				    output_group& group;

			    };


        // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! Create a repository object
		    repository(const std::string& path, access_type mode)
		      : root_path(path), access_mode(mode)
			    {
			    }

        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository()
          {
          }


        // INTERFACE -- ADMIN DATA

      public:

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() { return(this->root_path); };

		    //! Get access mode
		    const access_type& get_access_mode() { return(this->access_mode); }


        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void write_package(const initial_conditions<number>& ics, const model<number>* m) = 0;

        //! Write a threepf integration task to the integration database.
        virtual void write_integration(const twopf_task<number>& t, const model<number>* m) = 0;
        //! Write a twopf integration task to the integration database
        virtual void write_integration(const threepf_task<number>& t, const model<number>* m) = 0;


        // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE

      public:

        //! Query the database for a named task, and reconstruct it (and a suitable model object
		    //! which can process it) using the supplied model finder.
		    //! If successful, a pointer to the task is returned and a pointer to the model object
		    //! which can process it is written to the reference parameter m.
        virtual task<number>* query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder) = 0;


        // INTERFACE -- ADD OUTPUT TO TASKS

      public:

        //! Insert a record for new twopf output in the task database, and set up paths to a suitable data container
        virtual integration_writer integration_new_output(twopf_task<number>* tk,
                                                             const std::string& backend, unsigned int worker) = 0;
        //! Insert a record for new threepf output in the task database, and set up paths to a suitable data container
        virtual integration_writer integration_new_output(threepf_task<number>* tk,
                                                             const std::string& backend, unsigned int worker) = 0;


		    // INTERFACE - QUERY FOR OUTPUT FROM A TASK

      public:

		    //! Enumerate the output groups available from a named task
				virtual std::list<output_group> enumerate_task_output(const std::string& name) = 0;


				// PRIVATE DATA

      protected:

		    //! Access mode
        const access_type access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;

	    };


    // output an output_group descriptor to a standard stream
		template <typename number>
    std::ostream& operator<<(std::ostream& out, const typename repository<number>::output_group& group)
	    {
		    group.write(out);
        return(out);
	    }


  }   // namespace transport



#endif //__repository_H_
