#include <stdio.h>
#include <string.h>
#include <fstream>
#include <set>
#include <mist/config.h>
#include <mist/procs.h>
#include "controller_capabilities.h"

///\brief Holds everything unique to the controller.
namespace Controller {
  
  JSON::Value capabilities;
  //Converter::Converter * myConverter = 0;
  
  ///Generate list of available triggers, storing in global 'capabilities' JSON::Value.
  void checkAvailTriggers(){
    JSON::Value & trgs = capabilities["triggers"];
    trgs["SYSTEM_START"]["when"] = "After MistServer boot";
    trgs["SYSTEM_START"]["stream_specific"] = false;
    trgs["SYSTEM_START"]["payload"] = "";
    trgs["SYSTEM_START"]["response"] = "always";
    trgs["SYSTEM_START"]["response_action"] = "If false, shuts down the server.";

    trgs["SYSTEM_STOP"]["when"] = "Before MistServer shuts down";
    trgs["SYSTEM_STOP"]["stream_specific"] = false;
    trgs["SYSTEM_STOP"]["payload"] = "shutdown reason (string)";
    trgs["SYSTEM_STOP"]["response"] = "always";
    trgs["SYSTEM_STOP"]["response_action"] = "If false, aborts shutdown.";

    trgs["OUTPUT_START"]["when"] = "Before a connector starts listening for connections";
    trgs["OUTPUT_START"]["stream_specific"] = false;
    trgs["OUTPUT_START"]["payload"] = "connector configuration (JSON)";
    trgs["OUTPUT_START"]["response"] = "ignored";
    trgs["OUTPUT_START"]["response_action"] = "None.";

    trgs["OUTPUT_STOP"]["when"] = "Before a connector stops listening for connections";
    trgs["OUTPUT_STOP"]["stream_specific"] = false;
    trgs["OUTPUT_STOP"]["payload"] = "connector configuration (JSON)";
    trgs["OUTPUT_STOP"]["response"] = "ignored";
    trgs["OUTPUT_STOP"]["response_action"] = "None.";

    trgs["STREAM_ADD"]["when"] = "Before a new stream is configured";
    trgs["STREAM_ADD"]["stream_specific"] = true;
    trgs["STREAM_ADD"]["payload"] = "stream name (string)\nstream configuration (JSON)";
    trgs["STREAM_ADD"]["response"] = "always";
    trgs["STREAM_ADD"]["response_action"] = "If false, does not accept the new stream.";

    trgs["STREAM_CONFIG"]["when"] = "Every time a stream's configuration changes";
    trgs["STREAM_CONFIG"]["stream_specific"] = true;
    trgs["STREAM_CONFIG"]["payload"] = "stream name (string)\nnew stream configuration (JSON)";
    trgs["STREAM_CONFIG"]["response"] = "always";
    trgs["STREAM_CONFIG"]["response_action"] = "If false, rejects new configuration and reverts to current configuration.";

    trgs["STREAM_REMOVE"]["when"] = "Before an existing stream is removed";
    trgs["STREAM_REMOVE"]["stream_specific"] = true;
    trgs["STREAM_REMOVE"]["payload"] = "stream name (string)";
    trgs["STREAM_REMOVE"]["response"] = "always";
    trgs["STREAM_REMOVE"]["response_action"] = "If false, prevents removal and reverts to current configuration.";

    trgs["STREAM_SOURCE"]["when"] = "When a stream's source setting is loaded";
    trgs["STREAM_SOURCE"]["stream_specific"] = true;
    trgs["STREAM_SOURCE"]["payload"] = "stream name (string)";
    trgs["STREAM_SOURCE"]["response"] = "when-blocking";
    trgs["STREAM_SOURCE"]["response_action"] = "A non-empty response will set the stream source to the response value. An empty response will cause the stream source to not be changed from the normally configured stream source.";

    trgs["STREAM_LOAD"]["when"] = "Before a stream input is loaded";
    trgs["STREAM_LOAD"]["stream_specific"] = true;
    trgs["STREAM_LOAD"]["payload"] = "stream name (string)";
    trgs["STREAM_LOAD"]["response"] = "always";
    trgs["STREAM_LOAD"]["response_action"] = "If false, prevents loading of stream input.";

    trgs["STREAM_READY"]["when"] = "When a stream finished loading and is ready for playback";
    trgs["STREAM_READY"]["stream_specific"] = true;
    trgs["STREAM_READY"]["payload"] = "stream name (string)\ninput type (string)";
    trgs["STREAM_READY"]["response"] = "always";
    trgs["STREAM_READY"]["response_action"] = "If false, shuts down the stream input.";

    trgs["STREAM_UNLOAD"]["when"] = "Before a stream input is unloaded";
    trgs["STREAM_UNLOAD"]["stream_specific"] = true;
    trgs["STREAM_UNLOAD"]["payload"] = "stream name (string)\ninput type (string)";
    trgs["STREAM_UNLOAD"]["response"] = "always";
    trgs["STREAM_UNLOAD"]["response_action"] = "If false, aborts the unload and keeps the stream loaded.";

    trgs["STREAM_PUSH"]["when"] = "Before an incoming push is accepted";
    trgs["STREAM_PUSH"]["stream_specific"] = true;
    trgs["STREAM_PUSH"]["payload"] = "stream name (string)\nconnection address (string)\nconnector (string)\nrequest url (string)";
    trgs["STREAM_PUSH"]["response"] = "always";
    trgs["STREAM_PUSH"]["response_action"] = "If false, rejects the incoming push.";

    trgs["STREAM_TRACK_ADD"]["when"] = "Before a new track is accepted by a live stream buffer";
    trgs["STREAM_TRACK_ADD"]["stream_specific"] = true;
    trgs["STREAM_TRACK_ADD"]["payload"] = "stream name (string)\ntrack ID (integer)\n";
    trgs["STREAM_TRACK_ADD"]["response"] = "ignored";
    trgs["STREAM_TRACK_ADD"]["response_action"] = "None.";

    trgs["STREAM_TRACK_REMOVE"]["when"] = "Before a track is removed by a live stream buffer";
    trgs["STREAM_TRACK_REMOVE"]["stream_specific"] = true;
    trgs["STREAM_TRACK_REMOVE"]["payload"] = "stream name (string)\ntrack ID (integer)\n";
    trgs["STREAM_TRACK_REMOVE"]["response"] = "ignored";
    trgs["STREAM_TRACK_REMOVE"]["response_action"] = "None.";

    trgs["STREAM_BUFFER"]["when"] = "Every time a live stream buffer changes state";
    trgs["STREAM_BUFFER"]["stream_specific"] = true;
    trgs["STREAM_BUFFER"]["payload"] = "stream name (string)\nbuffer state: EMPTY, FULL, DRY or RECOVER (string)\nbuffer health information (only if not EMPTY) (JSON)";
    trgs["STREAM_BUFFER"]["response"] = "ignored";
    trgs["STREAM_BUFFER"]["response_action"] = "None.";

    trgs["RTMP_PUSH_REWRITE"]["when"] = "On incoming RTMP pushes, allows rewriting the RTMP URL to/from custom formatting";
    trgs["RTMP_PUSH_REWRITE"]["stream_specific"] = true;
    trgs["RTMP_PUSH_REWRITE"]["payload"] = "full current RTMP url (string)\nconnection hostname (string)";
    trgs["RTMP_PUSH_REWRITE"]["response"] = "when-blocking";
    trgs["RTMP_PUSH_REWRITE"]["response_action"] = "If non-empty, overrides the full RTMP url to the response value. If empty, denies the incoming RTMP push.";

    trgs["PUSH_OUT_START"]["when"] = "Before a push out (to file or other target type) is started";
    trgs["PUSH_OUT_START"]["stream_specific"] = true;
    trgs["PUSH_OUT_START"]["payload"] = "stream name (string)\npush target (string)";
    trgs["PUSH_OUT_START"]["response"] = "when-blocking";
    trgs["PUSH_OUT_START"]["response_action"] = "A non-empty response will set the push target to the response value. An empty response will abort the push. Variable substitution will still take place.";

    trgs["RECORDING_END"]["when"] = "When a push to file finishes";
    trgs["RECORDING_END"]["stream_specific"] = true;
    trgs["RECORDING_END"]["payload"] = "stream name (string)\npush target (string)\nconnector / filetype (string)\nbytes recorded (integer)\nseconds spent recording (integer)\nunix time recording started (integer)\nunix time recording stopped (integer)\ntotal milliseconds of media data recorded (integer)\nmillisecond timestamp of first media packet (integer)\nmillisecond timestamp of last media packet (integer)\n";
    trgs["RECORDING_END"]["response"] = "ignored";
    trgs["RECORDING_END"]["response_action"] = "None.";

    trgs["CONN_OPEN"]["when"] = "After a new connection is accepted";
    trgs["CONN_OPEN"]["stream_specific"] = true;
    trgs["CONN_OPEN"]["payload"] = "stream name (string)\nconnection address (string)\nconnector (string)\nrequest url (string)";
    trgs["CONN_OPEN"]["response"] = "always";
    trgs["CONN_OPEN"]["response_action"] = "If false, rejects the connection.";

    trgs["CONN_CLOSE"]["when"] = "After a new connection is closed";
    trgs["CONN_CLOSE"]["stream_specific"] = true;
    trgs["CONN_CLOSE"]["payload"] = "stream name (string)\nconnection address (string)\nconnector (string)\nrequest url (string)";
    trgs["CONN_CLOSE"]["response"] = "ignored";
    trgs["CONN_CLOSE"]["response_action"] = "None.";

    trgs["CONN_PLAY"]["when"] = "Before a connection first starts playback";
    trgs["CONN_PLAY"]["stream_specific"] = true;
    trgs["CONN_PLAY"]["payload"] = "stream name (string)\nconnection address (string)\nconnector (string)\nrequest url (string)";
    trgs["CONN_PLAY"]["response"] = "always";
    trgs["CONN_PLAY"]["response_action"] = "If false, rejects the playback attempt.";

    trgs["USER_NEW"]["when"] = "Every time a new session is added to the session cache";
    trgs["USER_NEW"]["stream_specific"] = true;
    trgs["USER_NEW"]["payload"] = "stream name (string)\nconnection address (string)\nconnection identifier (integer)\nconnector (string)\nrequest url (string)\nsession identifier (integer)";
    trgs["USER_NEW"]["response"] = "always";
    trgs["USER_NEW"]["response_action"] = "If false, denies the session while it remains in the cache. If true, accepts the session while it remains in the cache.";

    trgs["USER_END"]["when"] = "Every time a session ends (same time it is written to the access log)";
    trgs["USER_END"]["stream_specific"] = true;
    trgs["USER_END"]["payload"] = "session identifier (hexadecimal string)\nstream name (string)\nconnector (string)\nconnection address (string)\nduration in seconds (integer)\nuploaded bytes total (integer)\ndownloaded bytes total (integer)\ntags (string)";
    trgs["USER_END"]["response"] = "ignored";
    trgs["USER_END"]["response_action"] = "None.";

    trgs["LIVE_BANDWIDTH"]["when"] = "Every time a new live stream key frame is received";
    trgs["LIVE_BANDWIDTH"]["stream_specific"] = true;
    trgs["LIVE_BANDWIDTH"]["payload"] = "stream name (string)\ncurrent bytes per second (integer)";
    trgs["LIVE_BANDWIDTH"]["response"] = "always";
    trgs["LIVE_BANDWIDTH"]["response_action"] = "If false, shuts down the stream buffer.";
    trgs["LIVE_BANDWIDTH"]["argument"] = "Triggers only if current bytes per second exceeds this amount (integer)";
  }
  
  ///Aquire list of available protocols, storing in global 'capabilities' JSON::Value.
  void checkAvailProtocols(){
    std::deque<std::string> execs;
    Util::getMyExec(execs);
    std::string arg_one;
    char const * conn_args[] = {0, "-j", 0};
    for (std::deque<std::string>::iterator it = execs.begin(); it != execs.end(); it++){
      if ((*it).substr(0, 8) == "MistConn"){
        //skip if an MistOut already existed - MistOut takes precedence!
        if (capabilities["connectors"].isMember((*it).substr(8))){
          continue;
        }
        arg_one = Util::getMyPath() + (*it);
        conn_args[0] = arg_one.c_str();
        capabilities["connectors"][(*it).substr(8)] = JSON::fromString(Util::Procs::getOutputOf((char**)conn_args));
        if (capabilities["connectors"][(*it).substr(8)].size() < 1){
          capabilities["connectors"].removeMember((*it).substr(8));
        }
      }
      if ((*it).substr(0, 7) == "MistOut"){
        arg_one = Util::getMyPath() + (*it);
        conn_args[0] = arg_one.c_str();
        capabilities["connectors"][(*it).substr(7)] = JSON::fromString(Util::Procs::getOutputOf((char**)conn_args));
        if (capabilities["connectors"][(*it).substr(7)].size() < 1){
          capabilities["connectors"].removeMember((*it).substr(7));
        }
      }
      if ((*it).substr(0, 6) == "MistIn" && (*it) != "MistInfo"){
        arg_one = Util::getMyPath() + (*it);
        conn_args[0] = arg_one.c_str();
        capabilities["inputs"][(*it).substr(6)] = JSON::fromString(Util::Procs::getOutputOf((char**)conn_args));
        if (capabilities["inputs"][(*it).substr(6)].size() < 1){
          capabilities["inputs"].removeMember((*it).substr(6));
        }
      }
    }
  }
  
  ///\brief A class storing information about the cpu the server is running on.
  class cpudata{
    public:
      std::string model;///<A string describing the model of the cpu.
      int cores;///<The amount of cores in the cpu.
      int threads;///<The amount of threads this cpu can run.
      int mhz;///<The speed of the cpu in mhz.
      int id;///<The id of the cpu in the system.
      
      ///\brief The default constructor
      cpudata(){
        model = "Unknown";
        cores = 1;
        threads = 1;
        mhz = 0;
        id = 0;
      }
      ;
      
      ///\brief Fills the structure by parsing a given description.
      ///\param data A description of the cpu.
      void fill(char * data){
        int i;
        i = 0;
        if (sscanf(data, "model name : %n", &i) != EOF && i > 0){
          model = (data + i);
        }
        if (sscanf(data, "cpu cores : %d", &i) == 1){
          cores = i;
        }
        if (sscanf(data, "siblings : %d", &i) == 1){
          threads = i;
        }
        if (sscanf(data, "physical id : %d", &i) == 1){
          id = i;
        }
        if (sscanf(data, "cpu MHz : %d", &i) == 1){
          mhz = i;
        }
      }
      ;
  };

  ///\brief Checks the capabilities of the system.
  ///\param capa The location to store the capabilities.
  ///
  /// \api
  /// `"capabilities"` requests are always empty:
  /// ~~~~~~~~~~~~~~~{.js}
  /// {}
  /// ~~~~~~~~~~~~~~~
  /// and are responded to as:
  /// ~~~~~~~~~~~~~~~{.js}
  /// {
  ///   "connectors": { // a list of installed connectors
  ///     "FLV": { //name of the connector. This is based on the executable filename, with the "MistIn" / "MistConn" prefix stripped.
  ///       "codecs": [ //supported combinations of codecs.
  ///         [["H264","H263","VP6"],["AAC","MP3"]] //one such combination, listing simultaneously available channels and the codec options for those channels
  ///       ],
  ///       "deps": "HTTP", //dependencies on other connectors, if any.
  ///       "desc": "Enables HTTP protocol progressive streaming.", //human-friendly description of this connector
  ///       "methods": [ //list of supported request methods
  ///         {
  ///           "handler": "http", //what handler to use for this request method. The "http://" part of a URL, without the "://".
  ///           "priority": 5, // priority of this request method, higher is better.
  ///           "type": "flash/7" //type of request method - usually name of plugin followed by the minimal plugin version, or 'HTML5' for pluginless.
  ///         }
  ///       ],
  ///       "name": "HTTP_Progressive_FLV", //Full name of this connector.
  ///       "optional": { //optional parameters
  ///         "username": { //name of the parameter
  ///           "help": "Username to drop privileges to - default if unprovided means do not drop privileges", //human-readable help text
  ///           "name": "Username", //human-readable name of parameter
  ///           "option": "--username", //command-line option to use
  ///           "type": "str" //type of option - "str" or "num"
  ///         }
  ///         //above structure repeated for all (optional) parameters
  ///       },
  ///       //above structure repeated, as "required" for required parameters, if any.
  ///       "socket": "http_progressive_flv", //unix socket this connector listens on, if any
  ///       "url_match": "/$.flv", //URL pattern to match, if any. The $ substitutes the stream name and may not be the first or last character.
  ///       "url_prefix": "/progressive/$/", //URL prefix to match, if any. The $ substitutes the stream name and may not be the first or last character.
  ///       "url_rel": "/$.flv" //relative URL where to access a stream through this connector.
  ///     }
  ///     //... above structure repeated for all installed connectors.
  ///   },
  ///   "cpu": [ //a list of installed CPUs
  ///     {
  ///       "cores": 4, //amount of cores for this CPU
  ///       "mhz": 1645, //speed in MHz for this CPU
  ///       "model": "Intel(R) Core(TM) i7-2630QM CPU @ 2.00GHz", //model identifier, for humans
  ///       "threads": 8 //amount of simultaneously executing threads that are supported on this CPU
  ///     }
  ///     //above structure repeated for all installed CPUs
  ///   ],
  ///   "load": {
  ///     "fifteen": 72,
  ///     "five": 81,
  ///     "memory": 42,
  ///     "one": 124
  ///   },
  ///   "mem": {
  ///     "cached": 1989, //current memory usage of system caches, in MiB
  ///     "free": 2539, //free memory, in MiB
  ///     "swapfree": 0, //free swap space, in MiB
  ///     "swaptotal": 0, //total swap space, in MiB
  ///     "total": 7898, //total memory, in MiB
  ///     "used": 3370 //used memory, in MiB (excluding system caches, listed separately)
  ///   },
  ///   "speed": 6580, //total speed in MHz of all CPUs cores summed together
  ///   "threads": 8 //total count of all threads of all CPUs summed together
  ///   "cpu_use": 105 //Tenths of percent CPU usage - i.e. 105 = 10.5%
  /// }
  /// ~~~~~~~~~~~~~~~
  void checkCapable(JSON::Value & capa){
    //capa.null();
    capa.removeMember("cpu");
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo){
      std::map<int, cpudata> cpus;
      char line[300];
      int proccount = -1;
      while (cpuinfo.good()){
        cpuinfo.getline(line, 300);
        if (cpuinfo.fail()){
          //empty lines? ignore them, clear flags, continue
          if ( !cpuinfo.eof()){
            cpuinfo.ignore();
            cpuinfo.clear();
          }
          continue;
        }
        if (memcmp(line, "processor", 9) == 0){
          proccount++;
        }
        cpus[proccount].fill(line);
      }
      //fix wrong core counts
      std::map<int, int> corecounts;
      for (int i = 0; i <= proccount; ++i){
        corecounts[cpus[i].id]++;
      }
      //remove double physical IDs - we only want real CPUs.
      std::set<int> used_physids;
      int total_speed = 0;
      int total_threads = 0;
      for (int i = 0; i <= proccount; ++i){
        if ( !used_physids.count(cpus[i].id)){
          used_physids.insert(cpus[i].id);
          JSON::Value thiscpu;
          thiscpu["model"] = cpus[i].model;
          thiscpu["cores"] = cpus[i].cores;
          if (cpus[i].cores < 2 && corecounts[cpus[i].id] > cpus[i].cores){
            thiscpu["cores"] = corecounts[cpus[i].id];
          }
          thiscpu["threads"] = cpus[i].threads;
          if (thiscpu["cores"].asInt() > thiscpu["threads"].asInt()){
            thiscpu["threads"] = thiscpu["cores"];
          }
          thiscpu["mhz"] = cpus[i].mhz;
          capa["cpu"].append(thiscpu);
          total_speed += cpus[i].cores * cpus[i].mhz;
          total_threads += cpus[i].threads;
        }
      }
      capa["speed"] = total_speed;
      capa["threads"] = total_threads;
    }
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo){
      char line[300];
      int bufcache = 0;
      while (meminfo.good()){
        meminfo.getline(line, 300);
        if (meminfo.fail()){
          //empty lines? ignore them, clear flags, continue
          if ( !meminfo.eof()){
            meminfo.ignore();
            meminfo.clear();
          }
          continue;
        }
        long long int i;
        if (sscanf(line, "MemTotal : %lli kB", &i) == 1){
          capa["mem"]["total"] = i / 1024;
        }
        if (sscanf(line, "MemFree : %lli kB", &i) == 1){
          capa["mem"]["free"] = i / 1024;
        }
        if (sscanf(line, "SwapTotal : %lli kB", &i) == 1){
          capa["mem"]["swaptotal"] = i / 1024;
        }
        if (sscanf(line, "SwapFree : %lli kB", &i) == 1){
          capa["mem"]["swapfree"] = i / 1024;
        }
        if (sscanf(line, "Buffers : %lli kB", &i) == 1){
          bufcache += i / 1024;
        }
        if (sscanf(line, "Cached : %lli kB", &i) == 1){
          bufcache += i / 1024;
        }
      }
      capa["mem"]["used"] = capa["mem"]["total"].asInt() - capa["mem"]["free"].asInt() - bufcache;
      capa["mem"]["cached"] = bufcache;
      capa["load"]["memory"] = ((capa["mem"]["used"].asInt() + (capa["mem"]["swaptotal"].asInt() - capa["mem"]["swapfree"].asInt())) * 100)
          / capa["mem"]["total"].asInt();
    }
    std::ifstream loadavg("/proc/loadavg");
    if (loadavg){
      char line[300];
      loadavg.getline(line, 300);
      //parse lines here
      float onemin, fivemin, fifteenmin;
      if (sscanf(line, "%f %f %f", &onemin, &fivemin, &fifteenmin) == 3){
        capa["load"]["one"] = (long long int)(onemin * 100);
        capa["load"]["five"] = (long long int)(fivemin * 100);
        capa["load"]["fifteen"] = (long long int)(fifteenmin * 100);
      }
    }
    std::ifstream cpustat("/proc/stat");
    if (cpustat){
      char line[300];
      while (cpustat.getline(line, 300)){
        static unsigned long long cl_total = 0, cl_idle = 0;
        unsigned long long c_user, c_nice, c_syst, c_idle, c_total;
        if (sscanf(line, "cpu %Lu %Lu %Lu %Lu", &c_user, &c_nice, &c_syst, &c_idle) == 4){
          c_total = c_user + c_nice + c_syst + c_idle;
          if (c_total - cl_total > 0){
            capa["cpu_use"] = (long long int)(1000 - ((c_idle - cl_idle) * 1000) / (c_total - cl_total));
          }else{
            capa["cpu_use"] = 0ll;
          }
          cl_total = c_total;
          cl_idle = c_idle;
          break;
        }
      }
    }

  }

}

