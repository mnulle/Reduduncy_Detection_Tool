/* TestSupport.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 * $Revision: 1.1 $  $Date: 2008/03/05 22:14:37 $
 **********************************************************
 */
 
 
#ifndef TESTSUPPORT_H_
#define TESTSUPPORT_H_

// Comment out to remove test module from inclusion in main build
#define TEST_MODULE_ENABLED

/** Attempt to resolve an I/O module name from the specified string. The
 * resulting module will then be returned for future processing. The name
 * field refers to the unique class-level type of name (InputLog, InputPurge),
 * not the actual name that might be used to idenify one particular instance
 * of a given type (IL0, IL1, IL-WANSide).  
 * @param	sName		The name of the module (class-wise name)
 * @returns A pointer to a valid IOModule object if successfully resolved,
 *          NULL otherwise 
 */
IOModule * 		mapModuleFromName_Test (string sName);

#endif /*TESTSUPPORT_H_*/
