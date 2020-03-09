/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/


#include "condor_common.h"
#include "condor_debug.h"
#include "condor_classad.h"
#include "condor_commands.h"
#include "condor_attributes.h"
#include "daemon.h"
#include "dc_credd.h"
#include "../condor_utils/X509credential.unix.h"
#include "simplelist.h"


#include "../condor_utils/credential.unix.h"


// // // // //
// DCCredd
// // // // //

DCCredd::DCCredd( const char* tName, const char* tPool ) 
	: Daemon( DT_CREDD, tName, tPool )
{
}


DCCredd::~DCCredd( void )
{
}

bool 
DCCredd::storeCredential (Credential * cred,
						  CondorError & condor_error) {

	bool rtnVal = false;
	ReliSock *rsock = NULL;
	void *data = NULL;
	classad::ClassAd * classad = NULL;
	int rc = 0;
	int size=0;
	std::string adbuffer;
	classad::ClassAdUnParser unparser;

	rsock = (ReliSock *)startCommand(
			CREDD_STORE_CRED, Stream::reli_sock, 20, &condor_error);
	if ( ! rsock ) {
		goto EXIT;
	}

		// Force authentication
	if (!forceAuthentication( rsock, &condor_error )) {
		goto EXIT;
	}

		// Prepare to send
	rsock->encode();

		// Serialize metadata
		// We could just send the classad directly over the wire
		// but that has caused problems in the past for some reason
		// so we unparse it into a string
	classad = cred->GetMetadata();
	unparser.Unparse(adbuffer,classad);

		// Retrieve credential data
	cred->GetData (data, size);	

		// Send the metadata and data
	if (! rsock->code (adbuffer) ) {
		condor_error.pushf ("DC_CREDD", 3,
				"Communication error, send credential metadata: %s",
				strerror(errno) );
		goto EXIT;
	}

	if (! rsock->code_bytes (data, size) ) {
		condor_error.pushf ("DC_CREDD", 4,
				"Communication error, send credential data: %s",
				strerror(errno) );
		goto EXIT;
	}

	rsock->end_of_message();

		// Receive the return code
	rsock->decode();
	if (!rsock->code(rc)) {
		condor_error.pushf ("DC_CREDD", 4, "Communication error, recv return cod\n");
		rc = -1;
	}

	rsock->close();
	if (rc) {
		condor_error.pushf ("DC_CREDD", 4, "Invalid CredD return code (%d)", rc);
	}
	rtnVal =  (rc==0) ? true : false;
EXIT:
	if ( rsock ) delete rsock;
	if ( data ) free (data);
	if ( classad ) delete classad;
	return rtnVal;
}

bool 
DCCredd::getCredentialData (const char * cred_name,
							void *& cred_data,
							int & cred_size,
							CondorError & condor_error) {
	locate();
	
	ReliSock rsock;

	rsock.timeout(20);   // years of research... :)

		// Connect
	if( ! rsock.connect(_addr) ) {
		condor_error.pushf ( "DC_CREDD", 1, "Failed to connect to CredD %s", _addr);
		return false;
	}

		// Start command
	if( ! startCommand(CREDD_GET_CRED, (Sock*)&rsock) ) {
		condor_error.push ( "DC_CREDD", 2, "Failed to start command CREDD_GET_CRED");
		return false;
	}

		// Force authentication
	if (!forceAuthentication( &rsock, &condor_error )) {
		return false;
	}

		// Prepare to send request
	rsock.encode();

	rsock.put (cred_name);
	rsock.put ("DOMAIN");
	int mode = 0; // TODO: set the mode to one of STORE_CRED_USER_KRB, STORE_CRED_USER_PWD, or STORE_CRED_USER_OAUTH
	rsock.put(mode);

	rsock.decode();

	if (!(rsock.code(cred_size) && cred_size > 0)) {
		condor_error.push ("DC_CREDD", 3, "ERROR Receiving credential\n");
		return false;
	}

	cred_data = malloc (cred_size);
	if (!rsock.code_bytes (cred_data, cred_size)) {
		free (cred_data);
		cred_data = NULL;
		condor_error.push ("DC_CREDD", 4, "ERROR Receiving credential\n");
		return false;
	}
	
	rsock.close();
	return true;
}

bool
DCCredd::listCredentials (SimpleList <Credential*> & result,  
						  int & size,
						  CondorError & condor_error) {

	bool rtnVal = false;	// default
	ReliSock *rsock = NULL;
	Credential * cred = NULL;
	classad::ClassAdParser parser;
	classad::ClassAd * ad = NULL;
	const char *request = "_";

	rsock = (ReliSock *)startCommand(
			CREDD_QUERY_CRED, Stream::reli_sock, 20, &condor_error);
	if ( ! rsock ) {
		goto EXIT;
	}

		// Force authentication
	if (!forceAuthentication( rsock, &condor_error )) {
		goto EXIT;
	}

	rsock->encode();
	rsock->put (request);
	rsock->end_of_message();

		// Receive response
	rsock->decode();

	if (!rsock->code(size) || (size == 0)) {
		rtnVal = true;
		goto EXIT;
	}

	int i;
	for (i=0; i<size; i++) {
		char * classad_str = NULL;
		if (!(rsock->code (classad_str))) {
			condor_error.push ("DC_CREDD", 3, "Unable to receive credential data");
			goto EXIT;
		}

		ad = parser.ParseClassAd (classad_str);
		if (!ad) {
			condor_error.push ("DC_CREDD", 4, "Unable to parse credential data");
			goto EXIT;
		}

		cred = new X509Credential (*ad);
		result.Append (cred);
	}

	rtnVal =  true;	// success

EXIT:
	if (ad) delete ad;
	if ( rsock ) delete rsock;
	return rtnVal;
}

bool 
DCCredd::removeCredential (const char * cred_name,
						   CondorError & condor_error) {

	bool rtnVal = false;
	ReliSock *rsock = NULL;
	int rc=0;

	rsock = (ReliSock *)startCommand(
			CREDD_REMOVE_CRED, Stream::reli_sock, 20, &condor_error);
	if ( ! rsock ) {
		goto EXIT;
	}

		// Force authentication
	if (!forceAuthentication( rsock, &condor_error )) {
		goto EXIT;
	}

	rsock->encode();

	if ( ! rsock->put (cred_name) ) {
		condor_error.pushf ( "DC_CREDD", 3, "Error sending credential name: %s",
				strerror(errno) );
		goto EXIT;
	}

	if ( ! rsock->end_of_message() ) {
		condor_error.pushf ( "DC_CREDD", 3, "Error sending credential eom: %s",
				strerror(errno) );
		goto EXIT;
	}
	rsock->decode();

	if ( ! rsock->code (rc) ) {
		condor_error.pushf ( "DC_CREDD", 3, "Error rcving credential rc: %s",
				strerror(errno) );
		goto EXIT;
	}

	if (rc) {
		condor_error.push ( "DC_CREDD", 3, "Error removing credential");
		goto EXIT;
	}

	rtnVal = rc;
EXIT:
	if ( rsock ) delete rsock;
	return rtnVal;
}
