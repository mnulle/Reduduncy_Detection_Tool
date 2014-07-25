/* Stats.h : Adapted from the GenMCast module for ns-2
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
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#ifndef TableStats_H_
#define TableStats_H_

#include <string>
using namespace std;

#include <iostream>
#include <fstream>
using namespace std;

#include "Stats.h"


/** @brief The base class for deriving all stat objects
 * 
 *  The stats object allows all of the various multicast entities to share a common stat
 * basis. As a result, one can simply query each node, get the stats, and add/combine all of
 * them together. 
 */
class TableStats: public Stats
{
	public:
		enum stat_fields {
			UNUSED_GONE,
			TOTAL_HITS,
			BYTES_SAVED,
			BYTES_PROCESSED,

			STAT_FIELDS_SIZE
		};
		TableStats  ();
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		void		Get_Title	(int nStat, char * szTitle);
};

#endif /*STATS_H_*/
