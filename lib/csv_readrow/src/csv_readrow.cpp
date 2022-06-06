
#include "csv_readrow.h"
#include <string.h>
#ifndef _MSC_VER
//#include <unistd.h>
#define _stricmp strcasecmp
#define strnicmp strncasecmp
//#define _access access
#endif


std::vector<std::string> csv_read_row(std::string &line, char delimiter)
{
    std::stringstream ss(line);
    return csv_read_row(ss, delimiter);
}
 
std::vector<std::string> csv_read_row(std::istream &in, char delimiter)
{
    std::stringstream ss;
    bool inquotes = false;
    std::vector<std::string> row;//relying on RVO
    while(in.good())
    {
        char c = in.get();
        if (!inquotes && c=='"') //beginquotechar
        {
            inquotes=true;
        }
        else if (inquotes && c=='"') //quotechar
        {
            if ( in.peek() == '"')//2 consecutive quotes resolve to 1
            {
                ss << (char)in.get();
            }
            else //endquotechar
            {
                inquotes=false;
            }
        }
        else if (!inquotes && c==delimiter) //end of field
        {
            row.push_back( ss.str() );
            ss.str("");
        }
        else if (!inquotes && (c=='\r' || c=='\n' || c == -1) )
        {
            if(in.peek()=='\n') { in.get(); }
            row.push_back( ss.str() );
            return row;
        }
        else
        {
            ss << c;
        }
    }
    return row;
}

int getColIndex(std::string colName, std::vector<std::string> colNames)
{
	//int colNum = -1;
	char trgName[64], trgCols[64];
	strcpy(trgName,trim(colName).data());
	char *p = strchr(trgName, ' ');
	for(int i=0, leng=colNames.size(); i<leng; i++)
	{
		strcpy(trgCols, trim(colNames[i]).data());
		if(_stricmp(trgName,trgCols) == 0)
		//if(colNames[i].compare(colName) == 0)
		{
			return i;
		}
	}

	return -1;
}
