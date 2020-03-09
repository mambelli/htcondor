

#ifndef __CONDOR_SCITOKENS_H_
#define __CONDOR_SCITOKENS_H_

#include <string>
#include <vector>

#include "CondorError.h"

namespace htcondor {

	// Validate a given scitoken and populate the output variables (issuer, subject, expiry,
	// bounding_set) with the corresponding information from the token.
	//
	// Ident should be a unique identifier used as part of the audit trail.
bool
validate_scitoken(const std::string &scitoken_str, std::string &issuer, std::string &subject,
	long long &expiry, std::vector<std::string> &bounding_set, int ident, CondorError &err);

}

#endif // __CONDOR_SCITOKENS_H_
