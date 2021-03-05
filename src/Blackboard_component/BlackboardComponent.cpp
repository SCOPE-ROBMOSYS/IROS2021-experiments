/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
#include <iostream>

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Property.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include <Blackboard.h>

#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
using namespace yarp::os;

class BlackboardComponent : public Blackboard
{
public:
    BlackboardComponent() = default;

    bool init(std::string filename)
    {
        // The file has entry of the type key : value1, value2, value3,...
        //if a value is equal to "null", corresponding value will be initialized as an empty string
        yInfo() << "Initializing Blackboard";

        std::ifstream cFile (filename);
        if (cFile.is_open())
        {
            std::string line;
            while(getline(cFile, line)){//parse each single line
                line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                     line.end());
                if(line[0] == '#' || line.empty()) // skypping comments and empylined
                    continue;
                auto delimiterPos = line.find(":"); // delimiter is :
                auto key = line.substr(0, delimiterPos);
                auto allvalues = line.substr(delimiterPos + 1);
                std::string word = "";
                std::stringstream s(allvalues);
                Bottle b_values;
                while (getline(s, word, ',')) {// values are Comma Separated Values
                    if(is_digits(word)){
                        b_values.addInt32(std::stoi(word));
                    }
                    else if(is_float(word)){
                        b_values.addFloat64(std::stod(word));
                    }
                    else if(word=="null"){
                        b_values.addString(std::string());
                    }
                    else{
                        b_values.addString(word);
                    }
                }
                yInfo() << "key = "<< key << " with values "<< b_values.toString();
                m_initialization_values[key] = b_values;
            }
            resetData();
            return true;

        }
        else {
            std::cerr << "Couldn't open config file for reading.\n";
            return false;
        }

    }

    bool open()
    {
        this->yarp().attachAsServer(m_server_port);
        if (!m_server_port.open("/BlackboardComponent")) {
            yError("Could not open /BlackboardComponent");
            return false;
        }
        return true;
    }

    void close()
    {
        m_server_port.close();
    }


    std::vector<std::string> listAll() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::vector<std::string> ret;
        for(auto entry : m_storage)
        {
            yInfo() <<  entry.first  << " : " << entry.second.toString();
            ret.emplace_back(entry.first + " : " + entry.second.toString());
        }
        return ret;
    }

    // erase a single entry
    void clearData(const std::string &target)  override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.erase (m_storage.find(target));
    }

    // erase all the memory
    void clearAll() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.clear();
    }

    void resetData() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.clear();
        m_storage = m_initialization_values;
    }

    //If target matches the key of an element in the blackboard, the function returns a reference to its mapped value
    //Read function is possible only for the skill skillID or if the key target is not locked
    //Readfunction fails when return -1
    DataInt32 readDataInt32(const std::string& skillID, const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataInt32 response;
        if (m_storage.count(target) == 0){
            yInfo() << "readData error: key " << target << " INVALID_KEY" ;
            response.dataValue = 0;
            response.reqEval = INVALID_KEY;
            return response; 
	    }

        Bottle b = m_storage[target];
        if((b.get(0)).toString().empty() || (b.get(0)).toString() == skillID ){
            yInfo() << "readData from " << skillID << " with key " << target;
            response.dataValue = (b.get(1)).asInt32();
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "readData from " << skillID << " UNAUTHORIZED_CLIENT " ;
            response.dataValue = 0;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    DataString readDataString(const std::string& skillID, const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataString response;
        if (m_storage.count(target) == 0){
            yInfo() << "readData error: key " << target << " INVALID_KEY" ;
            response.dataValue = "";
            response.reqEval = INVALID_KEY;
            return response; 
	    }

        Bottle b = m_storage[target];
        if((b.get(0)).toString().empty() || (b.get(0)).toString() == skillID ){
            yInfo() << "readData from " << skillID << " with key " << target;
            response.dataValue = (b.get(1)).asString();
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "readData from " << skillID << " UNAUTHORIZED_CLIENT " ;
            response.dataValue = "";
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
        
    }

    DataFloat64 readDataFloat64(const std::string& skillID, const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataFloat64 response;
        if (m_storage.count(target) == 0){
            yInfo() << "readData error: key " << target << " INVALID_KEY" ;
            response.dataValue = 0;
            response.reqEval = INVALID_KEY;
            return response; 
	    }

        Bottle b = m_storage[target];
        if((b.get(0)).toString().empty() || (b.get(0)).toString() == skillID ){
            yInfo() << "readData from " << skillID << " with key " << target;
            response.dataValue = (b.get(1)).asFloat64();
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "readData from " << skillID << " UNAUTHORIZED_CLIENT " ;
            response.dataValue = 0;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    //write a data to the blackboard
    //Write function is possible only for the skill skillID or if the key target is not locked
    DataBool writeDataInt32(const std::string& skillID, const std::string& target, const std::int32_t datum) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataBool response;
        if (m_storage.count(target) == 0){
            yInfo() << "writeData error: key " << target << " INVALID_KEY" ;
            response.dataValue = false;
            response.reqEval = INVALID_KEY;
            return response;  
	    }
        Bottle b = m_storage[target];
        
        if((b.get(0)).toString() == skillID ){
            yInfo() << "writeData from " << skillID << " : key " << target << " and new value " << datum;
            m_storage.at(target).get(1) = yarp::os::Value(datum);
            response.dataValue = true;
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "writeData from " << skillID << " UNAUTHORIZED_CLIENT " ;
            response.dataValue = false;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    DataBool writeDataString(const std::string& skillID, const std::string& target, const std::string& datum) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataBool response;
        if (m_storage.count(target) == 0){
            yInfo() << "writeData error: key " << target << " INVALID_KEY" ;
            response.dataValue = false;
            response.reqEval = INVALID_KEY;
            return response;
	    }
        Bottle b = m_storage[target];
        
        if((b.get(0)).toString() == skillID ){
            yInfo() << "writeData from " << skillID << " : key " << target << " and new value " << datum;
            m_storage.at(target).get(1) = yarp::os::Value(datum);
            response.dataValue = true;
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "writeData from " << skillID << " UNAUTHORIZED_CLIENT" ;
            response.dataValue = false;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    DataBool writeDataFloat64(const std::string& skillID, const std::string& target, const double datum) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataBool response;
        if (m_storage.count(target) == 0){
            yInfo() << "writeData error: key " << target << " INVALID_KEY" ;
            response.dataValue = false;
            response.reqEval = INVALID_KEY;
            return response;  
	    }
        Bottle b = m_storage[target];
        
        if((b.get(0)).toString() == skillID ){
            yInfo() << "writeData from " << skillID << " : key " << target << " and new value " << datum;
            m_storage.at(target).get(1) = yarp::os::Value(datum);
            response.dataValue = true;
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "writeData from " << skillID << " UNAUTHORIZED_CLIENT " ;
            response.dataValue = false;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    //lock function: lock request from a skill skillID to a key target 
    //lock function is possible only if the key is not already locked or for the skill skillID
    DataBool lock(const std::string& skillID, const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataBool response;
        if (m_storage.count(target) == 0){
            yInfo() << "lock error: key " << target << " INVALID_KEY" ;
            response.dataValue = false;
            response.reqEval = INVALID_KEY;
            return response; 
	    }
        Bottle b = m_storage[target];

        if((b.get(0)).toString().empty() || (b.get(0)).toString() == skillID ){
            yInfo() << "lock request from " << skillID << " for key " << target;
            m_storage.at(target).get(0) = yarp::os::Value(skillID);
            response.dataValue = true;
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "lock request from " << skillID << " for key " << target << " UNAUTHORIZED_CLIENT";
            response.dataValue = false;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    //unlock function: unlock request from a skill skillID to a key target 
    //unlock function is possible only for the skill skillID
    DataBool unlock(const std::string& skillID, const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        DataBool response;
        if (m_storage.count(target) == 0){
            yInfo() << "unlock error: key " << target << " INVALID_KEY" ;
            response.dataValue = false;
            response.reqEval = INVALID_KEY;
            return response; 
	    }
        Bottle b = m_storage[target];

        if((b.get(0)).toString() == skillID ){
            yInfo() << "unlock request from " << skillID << " for key " << target;
            m_storage.at(target).get(0) = yarp::os::Value("");
            response.dataValue = true;
            response.reqEval = VALID;
            return response;
        }
        else{
            yInfo() << "unlock request from " << skillID << " for key " << target << " UNAUTHORIZED_CLIENT";
            response.dataValue = false;
            response.reqEval = UNAUTHORIZED_CLIENT;
            return response;
        }
    }

    ///check if the string is an integer
    bool is_digits(const std::string &str)
    {
        return std::all_of(str.begin(), str.end(), ::isdigit);  
    }
    ///check if the string is a float
    bool is_float(const std::string &str ) {
        std::istringstream iss(str);
        float f;
        iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
        // Check the entire string was consumed and if either failbit or badbit is set
        return iss.eof() && !iss.fail(); 
    }

private:
    yarp::os::RpcServer             m_server_port;
    std::map<std::string, Bottle>   m_storage;
    std::map<std::string, Bottle>   m_initialization_values;
    std::mutex                      m_mutex;

};

int main(int argc, char *argv[])
{
    Network yarp;

    BlackboardComponent blackboardComponent;
     
    if (!blackboardComponent.open()) {
        return 1;
    }

    ResourceFinder rf;
    rf.setVerbose();
    std::string default_cong_filename = "entries.ini";
    bool file_ok = rf.setDefaultConfigFile(default_cong_filename);
    if (!file_ok)
    {
        yWarning() << " Default config file " << default_cong_filename << " not found";
    }
    rf.configure(argc, argv);


    if(!blackboardComponent.init(default_cong_filename))
    {
        return 1;
    }

    while (true) {
        yInfo("Server running happily");
        yarp::os::Time::delay(10);
    }

    blackboardComponent.close();

    return 0;
}

