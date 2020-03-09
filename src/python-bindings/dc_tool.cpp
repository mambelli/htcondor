
// Note - python_bindings_common.h must be included before condor_common to avoid
// re-definition warnings.
#include "python_bindings_common.h"
#include "condor_common.h"

#include "daemon.h"
#include "daemon_types.h"
#include "condor_commands.h"
#include "condor_attributes.h"
#include "compat_classad.h"
#include "condor_config.h"
#include "subsystem_info.h"

#include "classad_wrapper.h"
#include "old_boost.h"
#include "module_lock.h"

using namespace boost::python;

enum DaemonCommands
{
  DDAEMONS_ON = DAEMONS_ON,
  DDAEMONS_OFF = DAEMONS_OFF,
  DDAEMONS_OFF_FAST = DAEMONS_OFF_FAST,
  DDAEMONS_OFF_PEACEFUL = DAEMONS_OFF_PEACEFUL,
  DDAEMON_ON = DAEMON_ON,
  DDAEMON_OFF = DAEMON_OFF,
  DDAEMON_OFF_FAST = DAEMON_OFF_FAST,
  DDAEMON_OFF_PEACEFUL = DAEMON_OFF_PEACEFUL,
  DDC_OFF_FAST = DC_OFF_FAST,
  DDC_OFF_PEACEFUL = DC_OFF_PEACEFUL,
  DDC_OFF_GRACEFUL = DC_OFF_GRACEFUL,
  DDC_SET_PEACEFUL_SHUTDOWN = DC_SET_PEACEFUL_SHUTDOWN,
  DDC_SET_FORCE_SHUTDOWN = DC_SET_FORCE_SHUTDOWN,
  DDC_OFF_FORCE = DC_OFF_FORCE,
  DDC_RECONFIG_FULL = DC_RECONFIG_FULL,
  DRESTART = RESTART,
  DRESTART_PEACEFUL = RESTART_PEACEFUL
};

enum LogLevel
{
  DALWAYS = D_ALWAYS,
  DERROR = D_ERROR,
  DSTATUS = D_STATUS,
  DJOB = D_JOB,
  DMACHINE = D_MACHINE,
  DCONFIG = D_CONFIG,
  DPROTOCOL = D_PROTOCOL,
  DPRIV = D_PRIV,
  DDAEMONCORE = D_DAEMONCORE,
  DSECURITY = D_SECURITY,
  DNETWORK = D_NETWORK,
  DHOSTNAME = D_HOSTNAME,
  DAUDIT = D_AUDIT,
  DTERSE = D_TERSE,
  DVERBOSE = D_VERBOSE,
  DFULLDEBUG = D_FULLDEBUG,
  DBACKTRACE = D_BACKTRACE,
  DIDENT = D_IDENT,
  DSUBSECOND = D_SUB_SECOND,
  DTIMESTAMP = D_TIMESTAMP,
  DPID = D_PID,
  DNOHEADER = D_NOHEADER
};

void send_command(const ClassAdWrapper & ad, DaemonCommands dc, const std::string &target="")
{
    std::string addr;
    if (!ad.EvaluateAttrString(ATTR_MY_ADDRESS, addr))
    {
        PyErr_SetString(PyExc_ValueError, "Address not available in location ClassAd.");
        throw_error_already_set();
    }
    std::string ad_type_str;
    if (!ad.EvaluateAttrString(ATTR_MY_TYPE, ad_type_str))
    {
        PyErr_SetString(PyExc_ValueError, "Daemon type not available in location ClassAd.");
        throw_error_already_set();
    }
    int ad_type = AdTypeFromString(ad_type_str.c_str());
    if (ad_type == NO_AD)
    {
        printf("ad type %s.\n", ad_type_str.c_str());
        PyErr_SetString(PyExc_ValueError, "Unknown ad type.");
        throw_error_already_set();
    }
    daemon_t d_type;
    switch (ad_type) {
    case MASTER_AD: d_type = DT_MASTER; break;
    case STARTD_AD: d_type = DT_STARTD; break;
    case SCHEDD_AD: d_type = DT_SCHEDD; break;
    case NEGOTIATOR_AD: d_type = DT_NEGOTIATOR; break;
    case COLLECTOR_AD: d_type = DT_COLLECTOR; break;
    default:
        d_type = DT_NONE;
        PyErr_SetString(PyExc_ValueError, "Unknown daemon type.");
        throw_error_already_set();
    }

    ClassAd ad_copy; ad_copy.CopyFrom(ad);
    Daemon d(&ad_copy, d_type, NULL);
    bool result;
    {
    condor::ModuleLock ml;
    result = !d.locate();
    }
    if (result)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to locate daemon.");
        throw_error_already_set();
    }
    ReliSock sock;
    {
    condor::ModuleLock ml;
    result = !sock.connect(d.addr());
    }
    if (result)
    {
        PyErr_SetString(PyExc_RuntimeError, "Unable to connect to the remote daemon");
        throw_error_already_set();
    }
    {
    condor::ModuleLock ml;
    result = !d.startCommand(dc, &sock, 0, NULL);
    }
    if (result)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to start command.");
        throw_error_already_set();
    }
    if (target.size())
    {
        std::string target_to_send = target;
        if (!sock.code(target_to_send))
        {
            PyErr_SetString(PyExc_RuntimeError, "Failed to send target.");
            throw_error_already_set();
        }
        if (!sock.end_of_message())
        {
            PyErr_SetString(PyExc_RuntimeError, "Failed to send end-of-message.");
            throw_error_already_set();
        }
    }
    sock.close();
}


void send_alive(boost::python::object ad_obj=boost::python::object(), boost::python::object pid_obj=boost::python::object(), boost::python::object timeout_obj=boost::python::object())
{
    std::string addr;
    if (ad_obj.ptr() == Py_None)
    {
        char *inherit_var = getenv("CONDOR_INHERIT");
        if (!inherit_var) {THROW_EX(RuntimeError, "No location specified and $CONDOR_INHERIT not in Unix environment.");}
        std::string inherit(inherit_var);
        boost::python::object inherit_obj(inherit);
        boost::python::object inherit_split = inherit_obj.attr("split")();
        if (py_len(inherit_split) < 2) {THROW_EX(RuntimeError, "$CONDOR_INHERIT Unix environment variable malformed.");}
        addr = boost::python::extract<std::string>(inherit_split[1]);
    }
    else
    {
        const ClassAdWrapper ad = boost::python::extract<ClassAdWrapper>(ad_obj);
        if (!ad.EvaluateAttrString(ATTR_MY_ADDRESS, addr))
        {
            THROW_EX(ValueError, "Address not available in location ClassAd.");
        }
    }
    int pid = getpid();
    if (pid_obj.ptr() != Py_None)
    {
        pid = boost::python::extract<int>(pid_obj);
    }
    int timeout;
    if (timeout_obj.ptr() == Py_None)
    {
        timeout = param_integer("NOT_RESPONDING_TIMEOUT");
    }
    else
    {
        timeout = boost::python::extract<int>(timeout_obj);
    }
    if (timeout < 1) {timeout = 1;}

    classy_counted_ptr<Daemon> daemon = new Daemon(DT_ANY, addr.c_str());
    classy_counted_ptr<ChildAliveMsg> msg = new ChildAliveMsg(pid, timeout, 0, 0, true);

    {
        condor::ModuleLock ml;
        daemon->sendBlockingMsg(msg.get());
    }
        if (msg->deliveryStatus() != DCMsg::DELIVERY_SUCCEEDED)
        {
            THROW_EX(RuntimeError, "Failed to deliver keepalive message.");
        }
}


void
enable_debug()
{
    dprintf_make_thread_safe(); // make sure that any dprintf's we do are thread safe on Linux (they always are on Windows)
    dprintf_set_tool_debug(get_mySubSystem()->getName(), 0);
}


void
enable_log()
{
    dprintf_make_thread_safe(); // make sure that any dprintf's we do are thread safe on Linux (they always are on Windows)
    dprintf_config(get_mySubSystem()->getName());
}


void
set_subsystem(std::string subsystem, SubsystemType type=SUBSYSTEM_TYPE_AUTO)
{
    set_mySubSystem(subsystem.c_str(), type);
}


static void
dprintf_wrapper2(int level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _condor_dprintf_va(level, static_cast<DPF_IDENT>(0), fmt, args);
    va_end(args);
}


void
dprintf_wrapper(int level, std::string msg)
{
    dprintf_wrapper2(level, "%s\n", msg.c_str());
}


BOOST_PYTHON_FUNCTION_OVERLOADS(send_command_overloads, send_command, 2, 3);

void
export_dc_tool()
{
    enum_<DaemonCommands>("DaemonCommands",
            R"C0ND0R(
            An enumeration of various state-changing commands that can be sent to a HTCondor daemon using :func:`send_command`.

            The values of the enumeration are:

            .. attribute:: DaemonOn
            .. attribute:: DaemonOff
            .. attribute:: DaemonOffFast
            .. attribute:: DaemonOffPeaceful
            .. attribute:: DaemonsOn
            .. attribute:: DaemonsOff
            .. attribute:: DaemonsOffFast
            .. attribute:: DaemonsOffPeaceful
            .. attribute:: OffFast
            .. attribute:: OffForce
            .. attribute:: OffGraceful
            .. attribute:: OffPeaceful
            .. attribute:: Reconfig
            .. attribute:: Restart
            .. attribute:: RestartPeacful
            .. attribute:: SetForceShutdown
            .. attribute:: SetPeacefulShutdown
            )C0ND0R")
        .value("DaemonsOn", DDAEMONS_ON)
        .value("DaemonsOff", DDAEMONS_OFF)
        .value("DaemonsOffFast", DDAEMONS_OFF_FAST)
        .value("DaemonsOffPeaceful", DDAEMONS_OFF_PEACEFUL)
        .value("DaemonOn", DDAEMON_ON)
        .value("DaemonOff", DDAEMON_OFF)
        .value("DaemonOffFast", DDAEMON_OFF_FAST)
        .value("DaemonOffPeaceful", DDAEMON_OFF_PEACEFUL)
        .value("OffGraceful", DDC_OFF_GRACEFUL)
        .value("OffPeaceful", DDC_OFF_PEACEFUL)
        .value("OffFast", DDC_OFF_FAST)
        .value("OffForce", DDC_OFF_FORCE)
        .value("SetPeacefulShutdown", DDC_SET_PEACEFUL_SHUTDOWN)
        .value("SetForceShutdown", DDC_SET_FORCE_SHUTDOWN)
        .value("Reconfig", DDC_RECONFIG_FULL)
        .value("Restart", DRESTART)
        .value("RestartPeacful", DRESTART_PEACEFUL)
        ;

    enum_<SubsystemType>("SubsystemType",
            R"C0ND0R(
            An enumeration of known subsystem names.

            The values of the enumeration are:

            .. attribute:: Collector
            .. attribute:: Daemon
            .. attribute:: Dagman
            .. attribute:: GAHP
            .. attribute:: Job
            .. attribute:: Master
            .. attribute:: Negotiator
            .. attribute:: Schedd
            .. attribute:: Shadow
            .. attribute:: SharedPort
            .. attribute:: Startd
            .. attribute:: Starter
            .. attribute:: Submit
            .. attribute:: Tool
            )C0ND0R")
        .value("Master", SUBSYSTEM_TYPE_MASTER)
        .value("Collector", SUBSYSTEM_TYPE_COLLECTOR)
        .value("Negotiator", SUBSYSTEM_TYPE_NEGOTIATOR)
        .value("Schedd", SUBSYSTEM_TYPE_SCHEDD)
        .value("Shadow", SUBSYSTEM_TYPE_SHADOW)
        .value("Startd", SUBSYSTEM_TYPE_STARTD)
        .value("Starter", SUBSYSTEM_TYPE_STARTER)
        .value("GAHP", SUBSYSTEM_TYPE_GAHP)
        .value("Dagman", SUBSYSTEM_TYPE_DAGMAN)
        .value("SharedPort", SUBSYSTEM_TYPE_SHARED_PORT)
        .value("Daemon", SUBSYSTEM_TYPE_DAEMON)
        .value("Tool", SUBSYSTEM_TYPE_TOOL)
        .value("Submit", SUBSYSTEM_TYPE_SUBMIT)
        .value("Job", SUBSYSTEM_TYPE_JOB)
        ;

    enum_<LogLevel>("LogLevel",
            R"C0ND0R(
            The log level attribute to use with :func:`log`.  Note that HTCondor
            mixes both a class (debug, network, all) and the header format (Timestamp,
            PID, NoHeader) within this enumeration.

            The values of the enumeration are:

            .. attribute:: Always
            .. attribute:: Audit
            .. attribute:: Config
            .. attribute:: DaemonCore
            .. attribute:: Error
            .. attribute:: FullDebug
            .. attribute:: Hostname
            .. attribute:: Job
            .. attribute:: Machine
            .. attribute:: Network
            .. attribute:: NoHeader
            .. attribute:: PID
            .. attribute:: Priv
            .. attribute:: Protocol
            .. attribute:: Security
            .. attribute:: Status
            .. attribute:: SubSecond
            .. attribute:: Terse
            .. attribute:: Timestamp
            .. attribute:: Verbose
            )C0ND0R")
        .value("Always", DALWAYS)
        .value("Error", DERROR)
        .value("Status", DSTATUS)
        .value("Job", DJOB)
        .value("Machine", DMACHINE)
        .value("Config", DCONFIG)
        .value("Protocol", DPROTOCOL)
        .value("Priv", DPRIV)
        .value("DaemonCore", DDAEMONCORE)
        .value("Security", DSECURITY)
        .value("Network", DNETWORK)
        .value("Hostname", DHOSTNAME)
        .value("Audit", DAUDIT)
        .value("Terse", DTERSE)
        .value("Verbose", DVERBOSE)
        .value("FullDebug", DFULLDEBUG)
        .value("SubSecond", DSUBSECOND)
        .value("Timestamp", DTIMESTAMP)
        .value("PID", DPID)
        .value("NoHeader", DNOHEADER)
        ;

    def("send_command", send_command, send_command_overloads(
        R"C0ND0R(
        Send a command to an HTCondor daemon specified by a location ClassAd.

        :param ad: Specifies the location of the daemon (typically, found by using :meth:`Collector.locate`).
        :type ad: :class:`~classad.ClassAd`
        :param dc: A command type
        :type dc: :class:`DaemonCommands`
        :param str target: An additional command to send to a daemon. Some commands
            require additional arguments; for example, sending ``DaemonOff`` to a
            ``condor_master`` requires one to specify which subsystem to turn off.
        )C0ND0R",
        boost::python::args("ad", "dc", "target")))
        ;

    def("send_alive", send_alive,
        R"C0ND0R(
        Send a keep alive message to an HTCondor daemon.

        This is used when the python process is run as a child daemon under
        the ``condor_master``.

        :param ad: A :class:`~classad.ClassAd` specifying the location of the daemon.
            This ad is typically found by using :meth:`Collector.locate`.
        :type ad: :class:`~classad.ClassAd`
        :param int pid: The process identifier for the keep alive. The default value of
            ``None`` uses the value from :func:`os.getpid`.
        :param int timeout: The number of seconds that this keep alive is valid. If a
            new keep alive is not received by the condor_master in time, then the
            process will be terminated. The default value is controlled by configuration
            variable ``NOT_RESPONDING_TIMEOUT``.
        )C0ND0R",
        (boost::python::arg("ad") = boost::python::object(), boost::python::arg("pid")=boost::python::object(), boost::python::arg("timeout")=boost::python::object()))
        ;

    def("set_subsystem", set_subsystem,
        R"C0ND0R(
        Set the subsystem name for the object.

        The subsystem is primarily used for the parsing of the HTCondor configuration file.

        :param str name: The subsystem name.
        :param daemon_type: The HTCondor daemon type. The default value of Auto infers the type from the name parameter.
        :type daemon_type: :class:`SubsystemType`
        )C0ND0R",
        (boost::python::arg("subsystem"), boost::python::arg("type")=SUBSYSTEM_TYPE_AUTO))
        ;

    def("enable_debug", enable_debug,
        R"C0ND0R(
        Enable debugging output from HTCondor, where output is sent to ``stderr``.
        The logging level is controlled by the ``TOOL_DEBUG`` parameter.
        )C0ND0R");
    def("enable_log", enable_log,
        R"C0ND0R(
        Enable debugging output from HTCondor, where output is sent to a file.
        The log level is controlled by the parameter ``TOOL_DEBUG``, and the
        file used is controlled by ``TOOL_LOG``.
        )C0ND0R");

    def("log", dprintf_wrapper,
        R"C0ND0R(
        Log a message using the HTCondor logging subsystem.

        :param level: The log category and formatting indicator. Multiple LogLevel enum attributes may be OR'd together.
        :type level: :class:`LogLevel`
        :param str msg: A message to log.
        )C0ND0R",
        boost::python::args("level", "msg"))
        ;

    if ( ! has_mySubSystem()) { set_mySubSystem("TOOL", SUBSYSTEM_TYPE_TOOL); }
    dprintf_pause_buffering();
}
