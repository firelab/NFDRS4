// FireWxConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <deque>
#include "fw21.h"
#include "csv_readrow.h"
#include "utctime.h"
#include <vector>
#include <numeric>

using namespace std;
using namespace utctime;

const string errStrings[] =
{
	"Blank station id",
	"Invalid date",
	"Unrecognized Region ID",
	"Unrecognized Unit ID",
	"Unrecognized Unit ID (District)",
	"Invalid Hour in RAWS record",
	"Invalid Minutes in RAWS record.",
	"-9999 found in record",
	"Change in Precipitation Measurement code mid stream",
	"Invalid or missing Precipitation Measurement Code",
	"Solar radiation out of range",
	"Missing solar radiation",
	"Bad Gust Speed or Gust Azimuth"
};

void Usage()
{
    cout << "FireWxConverter converts FW13 fire weather data files to FW21 fire weather data files\n";
    cout << "FireWxConverter expects three parameters:\n";
    cout << "FireWxConverter FW13file UTCoffset FW21file\n";
    cout << "\twhere\n\tFW13file is the complete path to the input FW13 file to be converted\n";
    cout << "\tUTCoffest is the integer offset from UTC time for the location the FW13 file represents\n";
    cout << "\tFW21file is the complete path to the input FW21 file to be produced\n";
}

//RH conversion utility routines
double satvap(double t)

{//return saturationvapor pressure, baset on temperature t(degrees K)
	if (t != 35.86)
		return exp(1.81 + (t * 17.27 - 4717.31) / (t - 35.86));
	else
		return 0.0;
}

double fTok(double f)
{
	//convert fahrenheit to kelvin
	return (f - 32.0) / 1.8 + 273.16;
}


int rhFromWb(double td, double tw, double pp)
{
	double corr = (0.00066 * (1.0 + (0.00115 * (tw - 273.16))) * pp * (td - tw));
	double val = max(1.0, min(100.0, ((satvap(tw) - corr) / satvap(td)) * 100.0));
	double rem = val - floor(val);
	int ret = (int)floor(val);
	return ret + ((rem >= 0.5) ? 1 : 0);
}

int rhFromDp(double dry, double dew)
{
	double val = 100.0 * (exp(-7482.6 / (dew + 398.36) + 15.674)
		/ exp(-7482.6 / (dry + 398.36) + 15.674));
	double rem = val - floor(val);
	int ret = (int)floor(val);
	return ret + ((rem >= 0.5) ? 1 : 0);
}

int RH(int RHType, int in, int db)
{
	int ret;
	switch (RHType)
	{
	case 1://wet bulb
		ret = rhFromWb(fTok(db), fTok((double)in), 900.0);//station press not corrected for elevation
		break;
	case 3:
		ret = rhFromDp((double)db, (double)in);
		break;
	default:
		ret = in;
	}
	return ret;
}


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        Usage();
        exit(1);
    }
    string inFileName, outFileName;
    inFileName = argv[1];
    outFileName = argv[3];
    int tzOffset = atoi(argv[2]);
	//for hourly precip tracking...
	UTCTime lastTime;
	time_t hoursDiff = utctime::get_hour_diff();
	std::deque<double> prev23;
	for (int i = 0; i < 23; i++)
		prev23.push_back(0.0);

    int encType = 0;
	int prevPcpCode = -1, pcpCode = -1, errors = 0 , m, d, y;
    char buf[161], temp[16];
    wchar_t bufw[161];
    string sta, oldStationID, seekStr;
	vector<string> staList;
    ifstream in;
    in.open(inFileName);
    if (!in.is_open())
    {
        cout << "Error opening " << inFileName << " as input FW13 file\n";
        return -1;
    }
    // first check to see if we've got ASCII

    in.read((char*)(&bufw), 152);

    if (bufw[0] > 256)
        encType = 0;   // ASCII
    else
        encType = 1;   // UTF-16
    in.close();

    in.open(inFileName, ios_base::in | ios_base::binary);   // re-open

	vector<FW21Record> vRecs;
        //need to know precipitation code
    while (!in.eof() && pcpCode < 0)
    {
        if (encType == 0)
        {   // Ascii / UTF-8
            in.getline(buf, 1024);
        }
        else
        {   // UTF-16
            in.read((char*)(&bufw), 152);
            for (int i = 0; i < 75; i++)
                buf[i] = (char)bufw[i + 2];
        }
        if (strlen(buf) > 63)
        {
            if (buf[0] != 'W')
            {
                continue;
            }
            strncpy(temp, &buf[62], 1);
            temp[1] = 0;
            //1 = running 24 inches, 2 = running 24 mm, 3 = hourly inches, 4 = hourly mm
            pcpCode = atoi(temp);
            if (pcpCode < 1 || pcpCode > 4)
                pcpCode = -1;
        }
    }
    if (pcpCode > 0)
        prevPcpCode = pcpCode;
    else
    {
       cout << "\tError: Can not determine precipitation code (column 63)\n";
        errors++;
    }
    in.seekg(0, in.beg);
    long recNo = 0, lineNo = 0, nonwx = 0, raws = 0, fcast = 0, daily = 0, other = 0, rejects = 0;
	bool isRaws, isNFDRS;
	while (!in.eof())
	{
		recNo++;

		if (encType == 0)
		{   // Ascii / UTF-8
			in.getline(buf, 1024);
		}
		else
		{   // UTF-16
			in.read((char*)(&bufw), 152);
			for (int i = 0; i < 75; i++)
				buf[i] = (char)bufw[i + 2];
		}

		if (in.eof())
			break;
		int rtype = 0;
		lineNo++;
		isRaws = isNFDRS = false;
		//check buffer for NODATA since IBM can't understand fixed width fields....
		//NODATA is -9999 slammed anywhere into the record
		if (strstr(buf, "-9999") != NULL)
		{
			cout << "\tError: Line Number " << lineNo <<", " << errStrings[7] << "\n";
			errors++;
			continue;
		}
		//check record type
		if (buf[0] != 'W')
		{
			nonwx++;
			continue;
		}

		if (buf[1] == '1' && buf[2] == '3')
			rtype = 13;
		// FW13
		else if (buf[1] == '9' && buf[2] == '8')
			rtype = 9;
		// FW9
		else
		{
			nonwx++;
			continue;
		}
		switch (buf[21])
		{
		case 'R':
			isRaws = true;
			raws++;
			break;
		case 'F':
			fcast++;
			break;
		case 'O':
			isNFDRS = true;
			daily++;
			break;
		default:
			other++;
		}
		if (!isNFDRS && !isRaws)
			continue;
		strncpy(temp, &buf[3], 6);
		temp[6] = 0;
		string sta = temp;
		sta = trim(sta);
		if (sta.length() == 0)//stationID can not be blank!!!!!!
		{
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[0] << "\n";
			errors++;
			continue;
		}
		string tempSta = sta;// .TrimLeft();
		if (tempSta.length() == 5)
		{
			sta = "0" + tempSta;
		}
		strncpy(temp, &buf[9], 4);
		temp[4] = 0;
		y = atoi(temp);
		strncpy(temp, &buf[13], 2);
		temp[2] = 0;
		m = atoi(temp);
		strncpy(temp, &buf[15], 2);
		temp[2] = 0;
		d = atoi(temp);
		UTCTime y2kCheck(y, m, d, 0, 0, 0);
		if (y2kCheck.timestamp() == 0 || y < 1900)
		{
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[1] << ": "<< m << "/" << d << "/" << y << "\n";
			rejects++;
			errors++;
			continue;
		}
		if (staList.empty())
			staList.push_back(sta);
		else
		{
			if (std::find(staList.begin(), staList.end(), sta) == staList.end())
			{
				cout << "\tError: Multpile stations encountered, line number " << lineNo << ", ignoring record for " << sta << "\n";
				continue;
			}
		}

			strncpy(temp, &buf[17], 2);
		temp[2] = 0;
		int hr = atoi(temp);
		if (hr >= 24 || hr < 0)
		{
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[5] << " : " << hr << "\n";
			errors++;
			continue;
		}

		strncpy(temp, &buf[19], 2);
		temp[2] = 0;
		int mn = atoi(temp);
		if (mn > 60 || mn < 0)
		{
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[6] << " : " << mn << "\n";
			errors++;
			continue;
		}

		UTCTime thisTime(y, m, d, hr, mn, 0);
		FW21Record rec;
		rec.SetDateTime(thisTime.get_tm());
		strncpy(temp, &buf[23], 3);
		temp[3] = 0;
		int db;
		if (strncmp(temp, "   ", 3) != 0)
		{
			db = atoi(temp);
			rec.SetTemp(db);
		}
		else
		{
			cout << "\tError: Line Number " << lineNo << ", temperature is blank\n";
			errors++;
			continue;
		}
		char t[2];
		t[0] = buf[61];
		t[1] = 0;
		strncpy(temp, &buf[26], 3);
		temp[3] = 0;
		if (strncmp(temp, "   ", 3) != 0)
		{
			rec.SetRH(max(1, RH(atoi(t), atoi(temp), db)));
		}
		else
		{
			cout << "\tError: Line Number " << lineNo << ", RH is invalid\n";
			errors++;
			continue;
		}

		//****************WIND SPEED AND DIRECTION *****************
		strncpy(temp, &buf[29], 3);
		temp[3] = 0;
		int tdir = -1, tws = -1;
		if (strncmp(temp, "   ", 3) != 0)
			tdir = atoi(temp);
		strncpy(temp, &buf[32], 3);
		temp[3] = 0;
		if (strncmp(temp, "   ", 3) != 0)
			tws = atoi(temp);
		if (tws >= 0 && tdir >= 0 && tdir <= 360)
		{
			rec.SetWindAzimuth(tdir);
			rec.SetWindSpeed(tws);
		}
		//NEED TO CHECK PRECIP MEASUREMENT TYPE CODE
		strncpy(temp, &buf[62], 1);
		temp[1] = 0;
		//1 = running 24 inches, 2 = running 24 mm, 3 = hourly inches, 4 = hourly mm
		pcpCode = atoi(temp);
		if (prevPcpCode <= 0)
			prevPcpCode = pcpCode;
		if (prevPcpCode != pcpCode)
		{
			//this is an error that never should happen
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[8] << " : " << pcpCode <<"\n";
			errors++;
			continue;
		}
		strncpy(temp, &buf[51], 5);
		temp[5] = 0;
		double thisPcp24 = -1.0, thisPcp = -1.0;
		switch (pcpCode)
		{
		case 1:
			thisPcp24 = atof(temp) / 1000.0;//inches have implied decimal point
			break;
		case 2:
			thisPcp24 = atof(temp) * 0.03937007874; //millimeter to inch
			break;
		case 3:
			thisPcp = atof(temp) / 1000.0;//inches have implied decimal point
			break;
		case 4:
			thisPcp = atof(temp) * 0.03937007874; //millimeter to inch
			break;
		default:
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[9] << " : " << pcpCode << "\n";
			errors++;
			continue;

		}
		//now deal with precip
		double hours = (thisTime - lastTime) / hoursDiff, sum23;
		for (int t = 1; t < hours && prev23.size() > 0; t++)
			prev23.pop_front();
		if (vRecs.size() > 0)
		{
			if (thisPcp < 0.0)
			{
				sum23 = std::accumulate(prev23.begin(), prev23.end(), 0.0);
				thisPcp = thisPcp24 - sum23;
				if (thisPcp < 0.0)
					thisPcp = 0.0;
			}
		}
		else
		{
			if (thisPcp < 0.0)
			{
				thisPcp = thisPcp24;
			}
		}
		rec.SetPrecip(thisPcp);
		prev23.push_back(thisPcp);
		while(prev23.size() > 23)
			prev23.pop_front();
		strncpy(temp, &buf[64], 4);
		temp[4] = 0;
		string tSlr = temp;
		trim(tSlr);
		//tSlr.Trim();
		//if(strncmp(temp, "    ", 4) != 0)
		if (tSlr.length() > 0)
		{
			int tSolRad = stoi(tSlr);
			if (tSolRad < 1400 && tSolRad >= 0)
				rec.SetSolarRadiation(tSolRad);
				//twxObs.m_SolarRadiation = tSolRad;// atoi(tSlr);
			else
			{
				cout << "\tError: Line Number " << lineNo << ", " << errStrings[10] << " : " << tSolRad << "\n";
				continue;
			}
		}
		else
		{
			cout << "\tError: Line Number " << lineNo << ", " << errStrings[11] << "\n";
			continue;
		}
		// end solar radiation addition

		//****************GUST SPEED AND DIRECTION *****************
		// 11/2012 added for FFP4.1
		strncpy(temp, &buf[68], 3);
		temp[3] = 0;
		int tgdir = -1, tgws = -1;
		if (strncmp(temp, "   ", 3) != 0)
			tgdir = atoi(temp);

		strncpy(temp, &buf[71], 3);
		temp[3] = 0;
		if (strncmp(temp, "   ", 3) != 0)
			tgws = atoi(temp);
		if (tgws >= 0 && tgdir >= 0 && tgdir <= 360)
		{
			if (tgdir == 360)
				tgdir = 0;

			//twxObs.m_GustDir = tgdir;
			//twxObs.m_GustSpd = tgws;
			rec.SetGustAzimuth(tgdir);
			rec.SetGustSpeed(tgws);
		}
		else
		{
			cout << "\tWarning: Line Number " << lineNo << ", " << errStrings[12] << "\n";
		}

		//***************end gusts ********************************

		// added code to get snow flag
		// 11/2012 added for FFP4.1

		strncpy(temp, &buf[74], 1);
		temp[1] = 0;
		if (temp[0] == 'Y' || temp[0] == 'y')
			rec.SetSnowFlag(1);
			//twxObs.m_SnowFlag = 1;
		else //if (temp[0] == 'N' || temp[0] == 'n')
			rec.SetSnowFlag(0);
			//twxObs.m_SnowFlag = 0;
		//else
			//twxObs.SetFieldNull(&twxObs.m_SnowFlag);
		// end snow flag addition

		vRecs.push_back(rec);		//twxObs.Update();
		lastTime = thisTime;
	}
	in.close();

                                                            
    //int errors = 0;

	//data.

	CFW21Data data;
	for (vector<FW21Record>::iterator it = vRecs.begin(); it != vRecs.end(); ++it)
	{
		if (data.AddRecord(*it) != 1)
		{
			cout << "Error adding record to FW21Data, " << (*it).GetYear() << "/" << (*it).GetMonth() << "/" << (*it).GetDay() << " : " << (*it).GetHour() << "\n";
		}
	}
	if (data.WriteFile(outFileName.c_str(), tzOffset) == 1)
	{
		cout << "Successfully wrote " << outFileName << "\n";
		return 0;
	}
	cout << "Error writing " << outFileName << "\n";
	return -1;
}

