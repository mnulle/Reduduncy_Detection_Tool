
#include <iostream>
using namespace std;

#include<fstream>
using namespace std;

#include<string>
using namespace std;

#include "Stats.h"
#include "TableStats.h"


TableStats::TableStats ():Stats()
{
	m_nSize = 0;
	m_fTime = 0.0;
	int		j;

	m_pnStats = new double [STAT_FIELDS_SIZE];
	m_pbyType = new char   [STAT_FIELDS_SIZE];
	m_nSize = STAT_FIELDS_SIZE;

	for(j=0; j<STAT_FIELDS_SIZE; j++)
	{
		m_pnStats[j] = 0;
		m_pbyType[j] = FIELD_TYPE_ADDITIVE;
	}

	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void	TableStats::Get_Title	(int nStat, char * szTitle)
{
	if(nStat >= m_nSize || nStat < 0)
	{
		printf("* Warning: Illegal value for stats field in Get_Stat (%d)\n", nStat);
		printf("			Max size = %d w/array at %p\n", m_nSize, m_pnStats);
		return;
	}
	switch(nStat) {
	case UNUSED_GONE:
		sprintf(szTitle, "Unused_Entries");
		break;
	case TOTAL_HITS:
		sprintf(szTitle, "Total_Hits");
		break;
	case BYTES_SAVED:
		sprintf(szTitle, "Bytes_Saved");
		break;
	case BYTES_PROCESSED:
		sprintf(szTitle, "Bytes_Processed");
		break;
	}

}
