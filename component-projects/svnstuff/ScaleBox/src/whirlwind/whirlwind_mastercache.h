/*
 * whirlwind_mastercache.h
 *
 * The Whirlwind Master Cache table is an optimized table that acts as a superset for all cache data
 * that is maintained by the Whirlwind Gateway.
 *
 * Conceptually, one can think of the Master Cache table as a tracker that knows all of the data cached
 * at various downstream UEs. The Master Cache effectively contains only signatures and references to
 * which clients contain the various pieces of information.
 *
 */

#ifndef WHIRLWIND_MASTERCACHE_H_
#define WHIRLWIND_MASTERCACHE_H_

/** An entry in the master cache entry table
 */
class Whirlwind_MasterCacheEntry
{
	public:
		Whirlwind_MasterCacheEntry ();
		~Whirlwind_MasterCacheEntry ();

		/** A sanity check that can be executed against an entry that will try to navigate all pointers
		 * and entries for the purpose of validation
		 *
		 * @return True if correct, false if errors are present
		 */
		bool checkSanity ();

		/** Dump a short summary of the entry to stdout
		 * @param bCheckSanity Add a sanity check to the mix (pre-printout)
		 */
		void dumpInfo_Short (bool bCheckSanity);

		/** Dump a long form of the entry to stdout
		 * @param bCheckSanity Add a sanity check to the mix (pre-printout)
		 */
		void dumpInfo (bool bCheckSanity);


		// Set the signature

		// Get the signature


		// Get the creation time

		// Set the creation time

		// Get the last hit time

		// Set the last hit time



		/** See if there is another entry which occurs when the master cache has multiple signatures
		 * that collide at the same position but yet contain different signatures. The master cache
		 * will allow for chaining of child entries.
		 * @returns True if there is another entry (non-NULL) or false if there is not another one (NULL)
		 */
		bool		hasNextEntry ();

		/** How many clients have this particular signature and byte chunk in their cache?
		 * @returns The count of the clients with this particular piece of data
		 */
		int 		getCountClientsWithCache ();


		/** Retrieve a pointer to the list of UEs that have this particular information
		 *
		 */
		Whirlwind_List_UserElements *	getListUEs ();

	private:
		// The signature for this entry



		// Byte-window used to compute the signature
		bool	m_bHasValidByteWindow;


		// The list of clients
		Whirlwind_List_UserElements		*	m_pListUEs;

		// When was this entry created?
		struct timeval		m_Time_Creation;

		// When was this entry last modified (client list)?
		struct timeval		m_Time_LastClientMod;

		// When was the last hit?
		struct timeval		m_Time_LastHit;

		// How many hits have we seen on this entry?


		// Statistics for this entry


		// Is this a chained hash entry?  This defaults to NULL but may contain a non-NULL value
		//  when a collision occurs in the master cache table.
		Whirlwind_MasterCacheEntry	*	m_pNext;
};


class Whirlwind_MasterCache
{
	public:
		Whirlwind_MasterCache ();
		~Whirlwind_MasterCache ();


		/** Conduct a sanity check on all of the information in the object and the various
		 * children in this master cache
		 * @returns True if all is well, false if there is an error
		 */
		bool 	checkSanity ();

		void 	dumpInfo_Short (bool bCheckSanity);

		void	dumpInfo (bool bCheckSanity);

	private:
		// What is the size of the master cache?  This should not be resized while running
		//  live as a matter of course but during initialization, it may need to be blitzed.

		//


};



#endif /* WHIRLWIND_MASTERCACHE_H_ */
