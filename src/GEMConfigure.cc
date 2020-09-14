#include "GEMConfigure.h"

using namespace std;

GEMConfigure::GEMConfigure()
{
    nFile = 0;
    cout<<"XXXX"<<endl;
    configure_file = "config/gem.cfg";
    //LoadConfigure();
}

GEMConfigure::GEMConfigure(const char * file)
{
    configure_file = file;
    //LoadConfigure();
}

GEMConfigure::~GEMConfigure()
{
}

void GEMConfigure::LoadConfigure()
{
    ifstream filestream;
    filestream.open( configure_file.c_str(), ifstream::in);
    if(!filestream)
    {
        cout<<"GEMConfigure: configure file open error..."
	    <<endl;
    }
    string line;
    int kk_file = 0;
    while( getline(filestream, line) )
    {
	line.erase( remove_if(line.begin(), line.end(), ::isspace), line.end() ); // strip spaces
	if(line.find("#") == 0) continue; //skip comments

	char* tokens = strtok( (char*)line.data(), " :,");
	while( tokens != NULL)
	{
	    string s = tokens;
	    if(s == "RUNTYPE")
	    {
		tokens = strtok(NULL, " :,");
		runType = tokens;
	    }
	    else if(s == "MAPPING")
	    {
		tokens = strtok(NULL, " :,");
		mapping = tokens;
	    }
	    else if(s=="SAVEPED")
	    {
		tokens = strtok(NULL, " :,");
		save_pedestal = tokens;
	    }
	    else if(s=="NEVENT")
	    {
		tokens = strtok(NULL, " :,");
		nEvent = atoi(tokens);
	    }
	    else if(s=="LOADPED")
	    {
		tokens = strtok(NULL, " :,");
		load_pedestal = tokens;
	    }
	    else if(s=="NFILE")
	    {
		tokens = strtok(NULL, " :,");
		nFile = atoi(tokens);
	    }
	    else if(s=="INPUTFILE")
	    {
		tokens = strtok(NULL, " :,");
		fileHeader = tokens;
		tokens = strtok(NULL, " :,");
		evioStart = atoi(tokens);
		tokens = strtok(NULL, " :,");
		evioEnd = atoi(tokens);

		for(int i=evioStart; i<= evioEnd;i++)
		{
		    fileList[kk_file++] = fileHeader+"."+to_string(i);
		}
	    }
	    else if(s == "HitRootFile")
	    {
		tokens = strtok(NULL, " :,");
		hitrootfile_path = tokens;
	    }

	    else
	    {
		tokens = strtok(NULL, " :,");
	    }
	}
    }

    assert(kk_file == nFile);
}

string GEMConfigure::GetRunType()
{
    return runType;
}

string GEMConfigure::GetMapping()
{
    return mapping;
}

string GEMConfigure::GetSavePedPath()
{
    return save_pedestal;
}

string GEMConfigure::GetLoadPedPath()
{
    return load_pedestal;
}

int GEMConfigure::GetNumEvt()
{
    return nEvent;
}
