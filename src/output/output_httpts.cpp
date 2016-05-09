#include "output_httpts.h"
#include <mist/defines.h>
#include <mist/http_parser.h>
#include <mist/stream.h>
#include <unistd.h>

namespace Mist {
  OutHTTPTS::OutHTTPTS(Socket::Connection & conn) : TSOutput(conn){
    if (config->getString("target").size()){
      if (!streamName.size()){
        WARN_MSG("Recording unconnected TS output to file! Cancelled.");
        conn.close();
        return;
      }
      if (config->getString("target") == "-"){
        parseData = true;
        wantRequest = false;
        INFO_MSG("Outputting %s to stdout in TS format", streamName.c_str());
        return;
      }
      if (connectToFile(config->getString("target"))){
        parseData = true;
        wantRequest = false;
        INFO_MSG("Recording %s to %s in TS format", streamName.c_str(), config->getString("target").c_str());
      }else{
        conn.close();
      }
    }
  }
  
  OutHTTPTS::~OutHTTPTS() {}

  void OutHTTPTS::init(Util::Config * cfg){
    HTTPOutput::init(cfg);
    capa["name"] = "HTTPTS";
    capa["desc"] = "Enables HTTP protocol MPEG2/TS pseudostreaming.";
    capa["url_rel"] = "/$.ts";
    capa["url_match"] = "/$.ts";
    capa["socket"] = "http_ts";
    capa["codecs"][0u][0u].append("H264");
    capa["codecs"][0u][0u].append("HEVC");
    capa["codecs"][0u][1u].append("AAC");
    capa["codecs"][0u][1u].append("MP3");
    capa["codecs"][0u][1u].append("AC3");
    capa["methods"][0u]["handler"] = "http";
    capa["methods"][0u]["type"] = "html5/video/mp2t";
    capa["methods"][0u]["priority"] = 1ll;
    capa["push_urls"].append("/*.ts");

    JSON::Value opt;
    opt["arg"] = "string";
    opt["default"] = "";
    opt["arg_num"] = 1ll;
    opt["help"] = "Target filename to store TS file as, or - for stdout.";
    cfg->addOption("target", opt);
  }

  bool OutHTTPTS::isRecording(){
    return config->getString("target").size();
  }
  
  void OutHTTPTS::onHTTP(){
    std::string method = H.method;
    
    initialize();
    H.SetHeader("Content-Type", "video/mp2t");
    H.setCORSHeaders();
    if(method == "OPTIONS" || method == "HEAD"){
      H.SendResponse("200", "OK", myConn);
      H.Clean();
      return;
    }
    H.StartResponse(H, myConn);    
    parseData = true;
    wantRequest = false;
  }

  void OutHTTPTS::sendTS(const char * tsData, unsigned int len){
    if (!isRecording()){
      H.Chunkify(tsData, len, myConn);
    }else{
      myConn.SendNow(tsData, len);
    }
  }
}

