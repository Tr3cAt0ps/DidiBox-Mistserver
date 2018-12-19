mistplayers.dashjs = {
  name: "Dash.js player",
  mimes: ["dash/video/mp4"/*,"html5/application/vnd.ms-ss"*/],
  priority: MistUtil.object.keys(mistplayers).length + 1,
  isMimeSupported: function (mimetype) {
    return (MistUtil.array.indexOf(this.mimes,mimetype) == -1 ? false : true);
  },
  isBrowserSupported: function (mimetype,source,MistVideo) {
    
    //check for http/https mismatch
    if (location.protocol != MistUtil.http.url.split(source.url).protocol) {
      MistVideo.log("HTTP/HTTPS mismatch for this source");
      return false;
    }
    
    //don't use dashjs if this location is loaded over file://
    if (location.protocol == "file:") {
      MistVideo.log("This source ("+mimetype+") won't load if the page is run via file://");
      return false;
    }
    
    return ("MediaSource" in window);
  },
  player: function(){this.onreadylist = [];},
  scriptsrc: function(host) { return host+"/dashjs.js"; }
};
var p = mistplayers.dashjs.player;
p.prototype = new MistPlayer();
p.prototype.build = function (MistVideo,callback) {
  var me = this;
  
  this.onDashLoad = function() {
    if (MistVideo.destroyed) { return; }
    
    MistVideo.log("Building DashJS player..");
    
    var ele = document.createElement("video");
    
    if ("Proxy" in window) {
      var overrides = {
        get: {},
        set: {}
      };
      
      MistVideo.player.api = new Proxy(ele,{
        get: function(target, key, receiver){
          if (key in overrides.get) {
            return overrides.get[key].apply(target, arguments);
          }
          var method = target[key];
          if (typeof method === "function"){
            return function () {
              return method.apply(target, arguments);
            }
          }
          return method;
        },
        set: function(target, key, value) {
          if (key in overrides.set) {
            return overrides.set[key].call(target,value);
          }
          return target[key] = value;
        }
      });
      
      if (MistVideo.info.type == "live") {
        overrides.get.duration = function(){
          //this should indicate the end of Mist's buffer
          var buffer_end = 0;
          if (this.buffered.length) {
            buffer_end = this.buffered.end(this.buffered.length-1)
          }
          var time_since_buffer = (new Date().getTime() - MistVideo.player.api.lastProgress.getTime())*1e-3;
          return buffer_end + time_since_buffer + -1*MistVideo.player.api.liveOffset + 45;
        };
        overrides.set.currentTime = function(value){
          var offset = value - MistVideo.player.api.duration;
          //MistVideo.player.api.liveOffset = offset;
          
          MistVideo.log("Seeking to "+MistUtil.format.time(value)+" ("+Math.round(offset*-10)/10+"s from live)");
          
          MistVideo.video.currentTime = value;
          //.player.api.setSource(MistUtil.http.url.addParam(MistVideo.source.url,{startunix:offset}));
        }
        MistUtil.event.addListener(ele,"progress",function(){
          MistVideo.player.api.lastProgress = new Date();
        });
        MistVideo.player.api.lastProgress = new Date();
        MistVideo.player.api.liveOffset = 0;
      }
      
    }
    else {
      me.api = ele;
    }
    
    if (MistVideo.options.autoplay) {
      ele.setAttribute("autoplay","");
    }
    if ((MistVideo.options.loop) && (MistVideo.info.type != "live")) {
      ele.setAttribute("loop","");
    }
    if (MistVideo.options.poster) {
      ele.setAttribute("poster",MistVideo.options.poster);
    }
    if (MistVideo.options.controls == "stock") {
      ele.setAttribute("controls","");
    }
    
    var player = dashjs.MediaPlayer().create();
    //player.getDebug().setLogToBrowserConsole(false);
    player.initialize(ele,MistVideo.source.url,MistVideo.options.autoplay);
    
    
    me.dash = player;
    
    //add listeners for events that we can log
    var skipEvents = ["METRIC_ADDED","METRIC_CHANGED","METRICS_CHANGED","FRAGMENT_LOADING_STARTED","FRAGMENT_LOADING_COMPLETED","LOG","PLAYBACK_TIME_UPDATED","PLAYBACK_PROGRESS"];
    for (var i in dashjs.MediaPlayer.events) {
      if (skipEvents.indexOf(i) < 0) {
        me.dash.on(dashjs.MediaPlayer.events[i],function(e){
          MistVideo.log("Player event fired: "+e.type);
        });
      }
    }
    
    MistVideo.player.setSize = function(size){
      this.api.style.width = size.width+"px";
      this.api.style.height = size.height+"px";
    };
    MistVideo.player.api.setSource = function(url) {
      MistVideo.player.dash.attachSource(url);
    };
    
    //trackswitching
    MistVideo.player.api.setTrack = function(type,id){
      var meta = MistUtil.tracks.parse(MistVideo.info.meta.tracks);
      if ((!(type in meta)) || ((!(id in meta[type]) && (id != 0)))) {
        MistVideo.log("Skipping trackselection of "+type+" track "+id+" because it does not exist");
        return;
      }
      
      //figure out what the track number is
      //whyyyy did it have to be reverse order
      var n = me.dash.getBitrateInfoListFor("video").length - 1;
      for (var i in MistVideo.info.meta.tracks) {
        var t = MistVideo.info.meta.tracks[i];
        if (t.type == type) {
          if (t.trackid == id) { break; }
          n--;
        }
      }
      
      me.dash.setAutoSwitchQualityFor(type,false); //turn off ABR rules //TODO do we want this by default?
      me.dash.setFastSwitchEnabled(true); //show the new track asap
      me.dash.setQualityFor(type,n);
      //dash does change the track, but is appended to the buffer, so it seems to take a while..
    }
    
    //react to automatic trackswitching
    me.dash.on("qualityChangeRendered",function(e){
      //the newQuality-th track of type mediaType is being selected
      
      //figure out the track id
      //whyyyy did it have to be reverse order
      var n = me.dash.getBitrateInfoListFor("video").length - 1;
      var id;
      for (var i in MistVideo.info.meta.tracks) {
        var t = MistVideo.info.meta.tracks[i];
        if (t.type == e.mediaType) {
          if (e.newQuality == n) {
            id = t.trackid;
            break;
          }
          n--;
        }
      }
      
      //create an event to pass this to the skin
      MistUtil.event.send("playerUpdate_trackChanged",{
        type: e.mediaType,
        trackid: id
      },MistVideo.video);
      
    });
    
    //dashjs keeps on spamming the stalled icon >_>
    MistUtil.event.addListener(ele,"progress",function(e){
      if (MistVideo.container.getAttribute("data-loading") == "stalled") {
        MistVideo.container.removeAttribute("data-loading");
      }
    });
    
    me.api.unload = function(){
      me.dash.reset();
    };
    
    MistVideo.log("Built html");
    callback(ele);
  }
  
  if ("dashjs" in window) {
    this.onDashLoad();
  }
  else {
    
    var scripttag = MistUtil.scripts.insert(MistVideo.urlappend(mistplayers.dashjs.scriptsrc(MistVideo.options.host)),{
      onerror: function(e){
        var msg = "Failed to load dashjs.js";
        if (e.message) { msg += ": "+e.message; }
        MistVideo.showError(msg);
      },
      onload: me.onDashLoad
    },MistVideo);
  }
}
