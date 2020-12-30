#pragma once

#ifdef ARCH
    #include <json/json.h>
#else
    #include <jsoncpp/json/json.h>
    #include <memory>
#endif
#include <fstream>
#include <string>
#include <iostream>

inline Json::Value* make_json(std::string str)
{
    std::ifstream file;
    file.open(str);
    std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));




    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader = (std::unique_ptr<Json::CharReader>)builder.newCharReader();

    Json::Value* root = new Json::Value;
    std::string errors;


    bool success = reader->parse(
        content.c_str(),
        content.c_str() + content.size(),
        root,
        &errors
    );



    reader.reset();
    if(!success)
    {
        std::cout << "error\n"; 
        std::cout << errors << "\n";
        return NULL;
    }

    return root;
   
}


inline void write_file(Json::Value root,const char* name)
{
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";

    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(name);
    writer -> write(root, &outputFileStream);
}



inline Json::Value* new_json()
{
    Json::Value *event = new Json::Value;
    return event;
}

inline Json::Value& add_Sub(Json::Value& event,const char* name)
{
    event[name];
    return event[name];
}

template <typename T>
void add_Field(Json::Value& event, const char* name,T val)
{
    event[name] = val;
}

template <typename T>
void add_val(Json::Value& event,T val)
{
    event.append(val);
}

