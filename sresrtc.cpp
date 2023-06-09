// sresrtc.cpp : Defines the exported functions for the DLL application.
//

#include "sres_rtcapi.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sys/stat.h>

#include "tdmsWriter.h"
#include "parser.h"

std::string getStringTime();
bool createFolder(std::string name);

tdms::TDMSWriter *wr;
Parser *parser;
const std::string resultPath = "/usr/local/T15result";
std::vector<std::string> groupOfMcsr;
std::vector<float> dataOfMcsr;
// std::vector<uint16_t> dataOfMcsr;
bool valid = true;

float Coef;

sres_result_t sres_func_init(const void* _InitData, void** _Context)
{
	parser = new Parser;

	valid = parser->Parse(config);
	if (!valid){
		return sres_rc_not_success;
	}
	valid = parser->Parse(properties);
	if (!valid){
		return sres_rc_not_success;
	}

	int size = 0;	
	if(parser->IsMcsr1()){
		groupOfMcsr.push_back("mcsr1");
		size += 64;
	}
	if(parser->IsMcsr2()){
		groupOfMcsr.push_back("mcsr2");
		size += 64;
	}

	dataOfMcsr.resize(size,0);

	std::string name = getStringTime();
	valid = createFolder(name);
	if (!valid){
		return sres_rc_not_success;
	}

	Coef = parser->settings.at("Uref").doubleValue/
		   (pow(2,parser->settings.at("Dig").int16Value)-1);

	wr = new tdms::TDMSWriter(resultPath + "/" + name + "/" + name+".tdms",
							  "mcsr tdms",	
							  "created by rtc");

	for (const auto& mcsr : groupOfMcsr){
		wr->AddGroup(mcsr, "Group of "+ mcsr);
	}

	int cnt = 0;
	for (const auto& mcsr : groupOfMcsr){
		for (int i=0; i<64; i++){
			std::string ch = mcsr + std::to_string(i);
			std::string mcsr = "MCSR";
			wr->AddChannelInGroup(mcsr, ch, tdsTypeSingleFloat, 1);
			// wr->AddChannelInGroup(mcsr, ch, tdsTypeU16, 1);
			wr->CreateChannelProperty(mcsr, ch, "DEVICE", tdsTypeString, std::string(parser->settings.at("DEVICE").stringValue));
			wr->CreateChannelProperty(mcsr, ch, "CNAME", tdsTypeString, std::string(parser->settings.at("CNAME").stringValue + ch));
			wr->CreateChannelProperty(mcsr, ch, "Shot", tdsTypeDoubleFloat, parser->settings.at("Shot").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "Date", tdsTypeTimeStamp, tdms::Timestamp());

			wr->CreateChannelProperty(mcsr, ch, "Start_delay", tdsTypeDoubleFloat, parser->settings.at("Start_delay").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "RATE", tdsTypeDoubleFloat, parser->settings.at("RATE").doubleValue);
			// wr.CreateChannelProperty(mcsr, ch, "FW", tdsTypeDoubleFloat, double(0));
			wr->CreateChannelProperty(mcsr, ch, "T4_delay", tdsTypeDoubleFloat, parser->settings.at("T4_delay").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "Duration", tdsTypeDoubleFloat, parser->settings.at("Duration").doubleValue);

			wr->CreateChannelProperty(mcsr, ch, "Gain", tdsTypeDoubleFloat, parser->settings.at("Gain").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "Offset", tdsTypeDoubleFloat, parser->settings.at("Offset").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "Vcoef", tdsTypeBoolean, parser->settings.at("Vcoef").boolValue);
			wr->CreateChannelProperty(mcsr, ch, "Acoef", tdsTypeDoubleFloat, parser->settings.at("Acoef").doubleValue);
			wr->CreateChannelProperty(mcsr, ch, "Aconst", tdsTypeDoubleFloat, parser->settings.at("Aconst").doubleValue);

			wr->AddRawDataInChannel(mcsr, ch, &dataOfMcsr[cnt]);
			cnt++;
		}
	}
	wr->GenerateFirstFilePart();
	wr->ConfigureToRawData();
	return sres_rc_success;
}

sres_result_t sres_func_data(size_t _SeqNo, sres_stamp_t _Stamp,
	const sres_sample_t* _Samples, void* _Context)
{
	if (!valid){
		return sres_rc_not_success;
	}
	
	for(int i=0; i<dataOfMcsr.size(); i++){
		dataOfMcsr[i] = Coef * _Samples[i]; 
		// dataOfMcsr[i] = _Samples[i]; 
	}

	wr->GenerateRawData();

	return sres_rc_success;
}

void sres_func_done(void* _Context)
{
}

bool createFolder(std::string name){
	std::string tmp = resultPath + "/" + name;
    int status = mkdir(tmp.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0) {
		std::cout << "rtc.so: Cant create folder for tdms data\n";
		return false;
    }
	return true;
}

std::string getStringTime(){
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char buffer [80];
    strftime (buffer,80,"tdms_%Y.%m.%d_%H.%M.%S",now);
    return std::string(buffer);
}

sres_result_t sres_func_notify(int _Code, const void* _Info, size_t _Size,
	void* _Context)
{
	// std::clog << "Invoked: sres_func_notify > ";
	switch (_Code) 
	{
	case sres_nf_prepare:
		// std::clog << "#sres_nf_prepare" << std::endl;
		break;
	case sres_nf_start:
		// std::clog << "#sres_nf_start" << std::endl;
		break;
	case sres_nf_stop:

		delete parser;
		if (valid){
			delete wr;
		}

		// std::clog << "#sres_nf_stop" << std::endl;
		break;
	default:
		// std::clog << "#" << _Code << std::endl;
		break;
	}
	
	return sres_rc_success;
}