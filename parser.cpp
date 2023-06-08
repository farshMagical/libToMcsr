#include "parser.h"

#include <fstream> 
#include <iostream> 
#include <cstring> 

bool Parser::Parse(const std::string& path){
    std::ifstream file;
    if (!openFile(file,path)){
        return false;
    }
    if(path == config){
        parseConfig(file);
    }
    else if(path == properties){
        parseProperties(file);
    }
    file.close();
    return true;
}

void Parser::parseConfig(std::ifstream& file){
    std::string tmp;
    while (file >> tmp){

        if(tmp[0] == ';'){ // u can comment useless strings
            getline(file,tmp);
            continue;
        }
        else if (tmp == "work_mode"){
            file >> tmp;
            int mcsr;
            file >> mcsr;
            if(mcsr == 1){
               Mcsr1Is = true; 
            }
            else if(mcsr == 2){
               Mcsr2Is = true; 
            }
            else{
               Mcsr1Is = true; 
               Mcsr2Is = true; 
            }
        }
    }
}

void Parser::parseProperties(std::ifstream& file){
    setDefaultProperties();
    std::string tmp;
    while (file >> tmp){
        if(tmp[0] == '/'){ // u can comment useless strings
            getline(file,tmp);
            continue;
        }
        if (tmp == "DEVICE" || tmp == "CNAME"){
            std::string cp;
            file >> cp;
            settings.at(tmp).stringValue = new char[tmp.size()];
            strcpy(settings.at(tmp).stringValue, cp.c_str());
            // file >> settings.at(tmp).stringValue;
        }
        else if (tmp == "Vcoef"){
            file >> settings.at(tmp).boolValue;
        }
        else if (tmp == "Dig"){
            file >> settings.at(tmp).int16Value;
        }
        else{
            file >> settings.at(tmp).doubleValue;
        }
    }
}

void Parser::setDefaultProperties(){
    settings["DEVICE"].stringValue = std::string("T15").data();
    settings["CNAME"].stringValue = std::string("Ch.").data();
    settings["Shot"].doubleValue = double(123);
    // settings["Date"].
    settings["Start_delay"].doubleValue = double(0);
    settings["RATE"].doubleValue = double(2000000);
    settings["FW"].doubleValue = double(0);
    settings["T4_delay"].doubleValue = double(0);
    settings["Duration"].doubleValue = double(0);
    settings["Gain"].doubleValue = double(1);
    settings["Offset"].doubleValue = double(0);
    settings["Vcoef"].boolValue = bool(true);
    settings["Acoef"].doubleValue = double(1);
    settings["Aconst"].doubleValue = double(0);

    settings["Uref"].doubleValue = double(4.096);
    settings["Dig"].int16Value = 14;
}

bool Parser::openFile(std::ifstream& file, const std::string& path){
    file.open(path);
    if(!file.is_open()){
        std::cout << "rtc.so: File (" << path << ") is not open\n";
        std::cout << "tdms file will not be written" << std::endl;
        return false;
    }
    return true;
}

bool Parser::IsMcsr1(){
    return Mcsr1Is;
}
bool Parser::IsMcsr2(){
    return Mcsr2Is;
}