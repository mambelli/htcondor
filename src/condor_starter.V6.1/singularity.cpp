
#include "condor_common.h"

#include "singularity.h"

#include <vector>

#include "condor_config.h"
#include "my_popen.h"
#include "CondorError.h"
#include "basename.h"

using namespace htcondor;


bool Singularity::m_enabled = false;
bool Singularity::m_probed = false;
int  Singularity::m_default_timeout = 120;
MyString Singularity::m_singularity_version;


static bool find_singularity(std::string &exec)
{
#ifdef LINUX
	std::string singularity;
	if (!param(singularity, "SINGULARITY")) {
		dprintf(D_ALWAYS | D_FAILURE, "SINGULARITY is undefined.\n");
		return false;
	}
	exec = singularity;
	return true;
#else
	(void) exec;
	return false;
#endif
}


bool
Singularity::advertise(ClassAd &ad)
{
	if (m_enabled && ad.InsertAttr("HasSingularity", true)) {
		return false;
	}
	if (!m_singularity_version.empty() && ad.InsertAttr("SingularityVersion", m_singularity_version)) {
		return false;
	}
	return true;
}


bool
Singularity::enabled()
{
	CondorError err;
	return detect(err);
}

const char *
Singularity::version()
{
	CondorError err;
	if (!detect(err)) {return NULL;}
	return m_singularity_version.c_str();
}

bool
Singularity::detect(CondorError &err)
{
	if (m_probed) {return m_enabled;}

	m_probed = true;
	ArgList infoArgs;
	std::string exec;
	if (!find_singularity(exec)) {
		return false;
	}
	infoArgs.AppendArg(exec);
	infoArgs.AppendArg("--version");

	MyString displayString;
	infoArgs.GetArgsStringForLogging( & displayString );
	dprintf(D_FULLDEBUG, "Attempting to run: '%s %s'.\n", exec.c_str(), displayString.c_str());

	MyPopenTimer pgm;
	if (pgm.start_program(infoArgs, true, NULL, false) < 0) {
		// treat 'file not found' as not really error
		int d_level = D_FULLDEBUG;
		if (pgm.error_code() != ENOENT) {
			d_level = D_ALWAYS | D_FAILURE;
			err.pushf("Singularity::detect", 1, "Failed to run '%s' errno = %d %s.", displayString.c_str(), pgm.error_code(), pgm.error_str());
		}
		dprintf(d_level, "Failed to run '%s' errno=%d %s.\n", displayString.c_str(), pgm.error_code(), pgm.error_str() );
		return false;
	}

	int exitCode;
	if ( ! pgm.wait_for_exit(m_default_timeout, &exitCode) || exitCode != 0) {
		pgm.close_program(1);
		MyString line;
		line.readLine(pgm.output(), false); line.chomp();
		dprintf( D_ALWAYS, "'%s' did not exit successfully (code %d); the first line of output was '%s'.\n", displayString.c_str(), exitCode, line.c_str());
		err.pushf("Singularity::detect", 2, "'%s' exited with status %d", displayString.c_str(), exitCode);
		return false;
	}

	m_singularity_version.readLine(pgm.output(), false);
	m_singularity_version.chomp();
	dprintf( D_FULLDEBUG, "[singularity version] %s\n", m_singularity_version.c_str() );
	if (IsFulldebug(D_ALWAYS)) {
		MyString line;
		while (line.readLine(pgm.output(), false)) {
			line.readLine(pgm.output(), false);
			line.chomp();
			dprintf( D_FULLDEBUG, "[singularity info] %s\n", line.c_str() );
		}
	}

	m_enabled = ! m_singularity_version.empty();

	return true;
}

bool
Singularity::job_enabled(ClassAd &machineAd, ClassAd &jobAd)
{
	return param_boolean("SINGULARITY_JOB", false, false, &machineAd, &jobAd);
}


Singularity::result
Singularity::setup(ClassAd &machineAd,
		ClassAd &jobAd,
		std::string &exec,
		ArgList &job_args,
		const std::string &job_iwd,
		const std::string &execute_dir,
		Env &job_env)
{
	ArgList sing_args;

	if (!param_boolean("SINGULARITY_JOB", false, false, &machineAd, &jobAd)) {return Singularity::DISABLE;}

	if (!enabled()) {
		dprintf(D_ALWAYS, "Singularity job has been requested but singularity does not appear to be configured on this host.\n");
		return Singularity::FAILURE;
	}
	std::string sing_exec_str;
	if (!find_singularity(sing_exec_str)) {
		return Singularity::FAILURE;
	}

	std::string image;
	if (!param_eval_string(image, "SINGULARITY_IMAGE_EXPR", "SingularityImage", &machineAd, &jobAd)) {
		dprintf(D_ALWAYS, "Singularity support was requested but unable to determine the image to use.\n");
		return Singularity::FAILURE;
	}

	std::string target_dir;
	bool has_target = param(target_dir, "SINGULARITY_TARGET_DIR") && !target_dir.empty();

	job_args.RemoveArg(0);
	std::string orig_exec_val = exec;
	if (has_target && (orig_exec_val.compare(0, execute_dir.length(), execute_dir) == 0)) {
		exec = target_dir + "/" + orig_exec_val.substr(execute_dir.length());
		dprintf(D_FULLDEBUG, "Updated executable path to %s for target directory mode.\n", exec.c_str());
	}
	sing_args.AppendArg(sing_exec_str.c_str());
	sing_args.AppendArg("exec");

	// Bind
	// Mount under scratch
	std::string scratch;
	if (!param_eval_string(scratch, "MOUNT_UNDER_SCRATCH", "", &jobAd)) {
		param(scratch, "MOUNT_UNDER_SCRATCH");
	}
	if (scratch.length() > 0) {
		StringList scratch_list(scratch.c_str());
		scratch_list.rewind();
		char *next_dir;
		while ( (next_dir=scratch_list.next()) ) {
			if (!*next_dir) {
				scratch_list.deleteCurrent();
				continue;
			}
			sing_args.AppendArg("-S");
			sing_args.AppendArg(next_dir);
		}
	}
	if (job_iwd != execute_dir) {
		sing_args.AppendArg("-B");
		sing_args.AppendArg(job_iwd.c_str());
	}
	// When overlayfs is unavailable, singularity cannot bind-mount a directory that
	// does not exist in the container.  Hence, we allow a specific fixed target directory
	// to be used instead.
	std::string bind_spec = execute_dir;
	if (has_target) {
		bind_spec += ":";
		bind_spec += target_dir;
		// Only change PWD to our new target dir if that's where we should startup.
		if (job_iwd == execute_dir) {
			sing_args.AppendArg("--pwd");
			sing_args.AppendArg(target_dir.c_str());
		}
		// Update the environment variables
		retargetEnvs(job_env, target_dir, execute_dir);

	}
	sing_args.AppendArg("-B");
	sing_args.AppendArg(bind_spec.c_str());

	if (param_eval_string(bind_spec, "SINGULARITY_BIND_EXPR", "SingularityBind", &machineAd, &jobAd)) {
		dprintf(D_FULLDEBUG, "Parsing bind mount specification for singularity: %s\n", bind_spec.c_str());
		StringList binds(bind_spec.c_str());
		binds.rewind();
		char *next_bind;
		while ( (next_bind=binds.next()) ) {
			sing_args.AppendArg("-B");
			sing_args.AppendArg(next_bind);
		}
	}

	if (!param_boolean("SINGULARITY_MOUNT_HOME", false, false, &machineAd, &jobAd)) {
		sing_args.AppendArg("--no-home");
	}

	MyString args_error;
	char *tmp = param("SINGULARITY_EXTRA_ARGUMENTS");
	if(!sing_args.AppendArgsV1RawOrV2Quoted(tmp,&args_error)) {
		dprintf(D_ALWAYS,"singularity: failed to parse extra arguments: %s\n",
		args_error.Value());
		free(tmp);
		return Singularity::FAILURE;
	}
	if (tmp) free(tmp);

	// if the startd has assigned us a gpu, add --nv to the sing exec
	// arguments to mount the nvidia devices
	std::string assignedGpus;
	machineAd.LookupString("AssignedGPUs", assignedGpus);
	if  (assignedGpus.length() > 0) {
		sing_args.AppendArg("--nv");
	}

	sing_args.AppendArg("-C");
	sing_args.AppendArg(image.c_str());

	sing_args.AppendArg(exec.c_str());
	sing_args.AppendArgsFromArgList(job_args);

	MyString args_string;
	job_args = sing_args;
	job_args.GetArgsStringForDisplay(&args_string, 1);
	exec = sing_exec_str;
	dprintf(D_FULLDEBUG, "Arguments updated for executing with singularity: %s %s\n", exec.c_str(), args_string.Value());

	Singularity::convertEnv(&job_env);
	return Singularity::SUCCESS;
}

static bool
envToList(void *list, const MyString &Name, const MyString & /*value*/) {
	std::list<std::string> *slist = (std::list<std::string> *)list;
	slist->push_back(std::string(Name));
	return true;
}

bool
Singularity::retargetEnvs(Env &job_env, const std::string &target_dir, const std::string &execute_dir) {
	
	// if SINGULARITY_TARGET_DIR is set, we need to reset
	// all the job's environment variables that refer to the scratch dir

	job_env.SetEnv("_CONDOR_SCRATCH_DIR", target_dir.c_str());
	job_env.SetEnv("TEMP", target_dir.c_str());
	job_env.SetEnv("TMP", target_dir.c_str());
	job_env.SetEnv("TMPDIR", target_dir.c_str());
	std::string chirp = target_dir + "/.chirp.config";
	std::string machine_ad = target_dir + "/.machine.ad";
	std::string job_ad = target_dir + "/.job.ad";
	job_env.SetEnv("_CONDOR_CHIRP_CONFIG", chirp.c_str());
	job_env.SetEnv("_CONDOR_MACHINE_AD", machine_ad.c_str());
	job_env.SetEnv("_CONDOR_JOB_AD", job_ad.c_str());
	MyString proxy_file;
	if ( job_env.GetEnv( "X509_USER_PROXY", proxy_file ) &&
	     strncmp( execute_dir.c_str(), proxy_file.Value(),
	      execute_dir.length() ) == 0 ) {
		std::string new_proxy = target_dir + "/" + condor_basename( proxy_file.Value() );
		job_env.SetEnv( "X509_USER_PROXY", new_proxy.c_str() );
	}
	return true;
}
bool 
Singularity::convertEnv(Env *job_env) {
	std::list<std::string> envNames;
	job_env->Walk(envToList, (void *)&envNames);
	std::list<std::string>::iterator it;
	for (it = envNames.begin(); it != envNames.end(); it++) {
		std::string name = *it;
		MyString value;
		job_env->GetEnv(name.c_str(), value);
		std::string new_name = "SINGULARITYENV_" + name;
		job_env->SetEnv(new_name.c_str(), value);
	}
	return true;
}
