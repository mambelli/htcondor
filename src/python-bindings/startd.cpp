
// Note - python_bindings_common.h must be included before condor_common to avoid
// re-definition warnings.
#include "python_bindings_common.h"

# if defined(__APPLE__)
# undef HAVE_SSIZE_T
# include <pyport.h>
# endif

#include "condor_common.h"
#include <boost/python/overloads.hpp>

#include "enum_utils.h"
#include "condor_attributes.h"
#include "dc_startd.h"
#include "globus_utils.h"
#include "classad/source.h"

#include "old_boost.h"
#include "module_lock.h"
#include "classad_wrapper.h"
#include "history_iterator.h"

using namespace boost::python;

/*
 * Object representing a claim on a remote startd.
 *
 * Purpose is twofold: one, to represent 'full' claims as found in the
 * collector.  Two, to provide a python binding for the COD API.
 */
struct Startd
{


    Startd() {}


    Startd(boost::python::object ad_obj)
    {
        ClassAdWrapper ad = boost::python::extract<ClassAdWrapper>(ad_obj);
        if (!ad.EvaluateAttrString(ATTR_MY_ADDRESS, m_addr))
        {
            THROW_EX(ValueError, "No contact string in ClassAd");
        }
    }

    std::string
    drain_jobs(int how_fast=DRAIN_GRACEFUL, bool resume_on_completion=false, boost::python::object check_obj=boost::python::object(""), boost::python::object start_obj = boost::python::object() )
    {
        std::string check_expr;
        boost::python::extract<std::string> expr_extract(check_obj);
        if (expr_extract.check())
        {
            check_expr = expr_extract();
        }
        else
        {
            classad::ClassAdUnParser printer;
            classad_shared_ptr<classad::ExprTree> expr(convert_python_to_exprtree(check_obj));
            printer.Unparse(check_expr, expr.get());
        }

		std::string start_expr;
		boost::python::extract<std::string> start_extract( start_obj );
		if( start_extract.check() ) {
			start_expr = start_extract();
		} else {
			classad::ClassAdUnParser printer;
			classad_shared_ptr<classad::ExprTree> expr(convert_python_to_exprtree(start_obj));
			printer.Unparse( start_expr, expr.get());
		}

        std::string request_id;

        DCStartd startd(m_addr.c_str());
        bool rval = startd.drainJobs(how_fast, resume_on_completion, check_expr.c_str(), start_expr.c_str(), request_id);
        if (!rval) {THROW_EX(RuntimeError, "Startd failed to begin draining jobs.");}
        return request_id;
    }

    void
    cancel_drain_jobs(boost::python::object rid=boost::python::object())
    {
        const char * request_id = NULL;
        std::string request_id_str;
        if (rid.ptr() == Py_None)
        {
            request_id = NULL;
        }
        else
        {
            request_id_str = boost::python::extract<std::string>(rid);
            request_id = request_id_str.c_str();
        }

        DCStartd startd(m_addr.c_str());
        bool rval = startd.cancelDrainJobs( request_id );
        if (!rval) {THROW_EX(RuntimeError, "Startd failed to cancel draining jobs.");}
    }

	boost::shared_ptr<HistoryIterator> history(boost::python::object requirement, boost::python::list projection = boost::python::list(), int match = -1, boost::python::object since = boost::python::object())
	{
		return history_query(requirement, projection, match, since, GET_HISTORY, m_addr);
	}

private:

    std::string m_addr;
};

// declare a formal enum so that python doesn't think all ints are DrainTypes (sigh)
enum DrainTypes {
	DrainFast=DRAIN_FAST,
	DrainGraceful=DRAIN_GRACEFUL,
	DrainQuick=DRAIN_QUICK,
};

void
export_startd()
{
    boost::python::enum_<DrainTypes>("DrainTypes",
            R"C0ND0R(
            Draining policies that can be sent to a ``condor_startd``.

            The values of the enumeration are:

            .. attribute:: Fast
            .. attribute:: Graceful
            .. attribute:: Quick
            )C0ND0R")
        .value("Fast", (DrainTypes)DRAIN_FAST)
        .value("Graceful", (DrainTypes)DRAIN_GRACEFUL)
        .value("Quick", (DrainTypes)DRAIN_QUICK)
        ;

#if BOOST_VERSION >= 103400
    #define SELFARG boost::python::arg("self"),
#else
    #define SELFARG
#endif

    boost::python::class_<Startd>("Startd",
            R"C0ND0R(
            A class that represents a Startd.
            )C0ND0R",
        boost::python::init<boost::python::object>(
            R"C0ND0R(
            :param ad: A ClassAd describing the claim and the startd location.
                If omitted, the local startd is assumed.
            :type ad: :class:`~classad.ClassAd`
            )C0ND0R",
            (boost::python::arg("self"), boost::python::arg("ad")=boost::python::object())))
        .def(boost::python::init<>(boost::python::args("self")))
        .def("drainJobs", &Startd::drain_jobs,
            R"C0ND0R(
            Begin draining jobs from the startd.

            :param drain_type: How fast to drain the jobs.  Defaults to ``DRAIN_GRACEFUL`` if not specified.
            :type drain_type: :class:`DrainTypes`
            :param bool resume_on_completion: Whether the startd should start accepting jobs again
                once draining is complete.  Otherwise, it will remain in the drained state.
                Defaults to ``False``.
            :param check_expr: An expression string that must evaluate to ``true`` for all slots for
                draining to begin. Defaults to ``'true'``.
            :type check_expr: str or :class:`ExprTree`
            :param start_expr: The expression that the startd should use while draining.
            :type start_expr: str or :class:`ExprTree`
            :return: An opaque request ID that can be used to cancel draining via :meth:`Startd.cancelDrainJobs`
            :rtype: str
            )C0ND0R",
               (SELFARG
                boost::python::arg("drain_type")=DRAIN_GRACEFUL,
                boost::python::arg("resume_on_completion")=false,
                boost::python::arg("check_expr")="true",
                boost::python::arg("start_expr")="false"
               )
            )
        .def("cancelDrainJobs", &Startd::cancel_drain_jobs,
            R"C0ND0R(
            Cancel a draining request.

            :param str request_id: Specifies a draining request to cancel.  If not specified, all
                draining requests for this startd are canceled.
            )C0ND0R",
               (SELFARG
                boost::python::arg("request_id")=""
               )
           )
        .def("history", &Startd::history,
            R"C0ND0R(
            Fetch history records from the ``condor_startd`` daemon.

            :param requirements: Query constraint; only jobs matching this constraint will be returned;
                defaults to ``'true'``.
            :type constraint: str or :class:`class.ExprTree`
            :param projection: Attributes that are to be included for each returned job.
                The empty list causes all attributes to be included.
            :type projection: list[str]
            :param int match: An limit on the number of jobs to include; the default (``-1``)
                indicates to return all matching jobs.
            :return: All matching ads in the Schedd history, with attributes according to the
                ``projection`` keyword.
            :rtype: :class:`HistoryIterator`
            )C0ND0R",
               (SELFARG
                boost::python::arg("requirements"), boost::python::arg("projection"), boost::python::arg("match")=-1,
                boost::python::arg("since")=boost::python::object())
            )

        ;

}

