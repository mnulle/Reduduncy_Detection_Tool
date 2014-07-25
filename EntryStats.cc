
#include <iostream>
using namespace std;

#include<fstream>
using namespace std;

#include<string>
using namespace std;

#include "Stats.h"
#include "EntryStats.h"


EntryStats::EntryStats ()
{
	m_nSize = 0;
	m_fTime = 0.0;
	int		j;

	m_pnStats = new int [STAT_FIELDS_SIZE];
	m_pbyType = new char   [STAT_FIELDS_SIZE];
	m_nSize = STAT_FIELDS_SIZE;

	for(j=0; j<STAT_FIELDS_SIZE; j++)
	{
		m_pnStats[j] = 0;
		m_pbyType[j] = FIELD_TYPE_ADDITIVE;
	}

	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void	EntryStats::Get_Title	(int nStat, char * szTitle)
{
	if(nStat >= m_nSize || nStat < 0)
	{
		printf("* Warning: Illegal value for stats field in Get_Stat (%d)\n", nStat);
		printf("			Max size = %d w/array at %p\n", m_nSize, m_pnStats);
		return;
	}
	switch(nStat) {
	case ACCESS:
		sprintf(szTitle, "Access_Count");
		break;
	case MISS:
		sprintf(szTitle, "Miss_Count");
		break;
	case HIT:
		sprintf(szTitle, "Hit_Count");
		break;
	case BYTES_SAVED:
		sprintf(szTitle, "Bytes_Saved");
		break;
	}

}
