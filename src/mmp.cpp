#include <log.h>
#include <mmp.h>
#include <algorithm>

void MMPFile::XML::print(int level){
  if (!(level&1) && type==Type::EndTag) return;
  if (!(level&2) && type==Type::Data) return;
  for (int i=0;i<2*depth;++i) std::cout << " ";
  if (type==Type::Root) std::cout << "ROOT";
  else if (type==Type::Unset) std::cout << "Unset";
  else if (type==Type::Data) std::cout << "Data";
  else if (type==Type::Comment) std::cout << "Comment";
  else if (type==Type::EndTag) std::cout << "ENDTAG " << name << ":";
  else {
    if (type==Type::Prologue)  std::cout << "Prologue: ";
    else if (type==Type::DOCTYPE) std::cout << "Doctype: ";
    else if (type==Type::Tag) std::cout << "TAG " << name << ":";
    for (int i=0; i<(int)params.size();++i) std::cout << params[i].first << "->" << params[i].second << "  ";
  }
  std::cout << std::endl;
  for (int i=0;i<(int)contents.size();++i) contents[i].print(level);
}


  
int MMPFile::XML::parse(std::string& str, int pos){
  if (type==Type::Root){
    while (pos<(int)str.size()){
      contents.push_back(XML());
      contents.back().depth=depth+1;
      pos=contents.back().parse(str, pos);
    }
  }else if (type==Type::Unset){
    if (str[pos]=='<' && pos+2<(int)str.size()){
      if (str[pos+1]=='?') type=Type::Prologue, ++pos;
      else if (str[pos+1]=='!'){
        if (str[pos+2]=='-' && pos+3<(int)str.size() && str[pos+3]=='-') type=Type::Comment, pos+=2;
        else   type=Type::DOCTYPE, ++pos;
      }else if (str[pos+1]=='/') type=Type::EndTag, ++pos;
      else type=Type::Tag;
      int s=0;
      bool end=(type==Type::Prologue || type==Type::Comment || type==Type::DOCTYPE || type==Type::EndTag);
      int v=0;
      std::string p1="";
      std::string last="";
      bool has_last=0;
      for(++pos;pos<(int)str.size(); ++pos){
        if (str[pos]=='>' && !s){
          if (type==Type::Prologue && str[pos-1]=='?')break;
          if (type==Type::Comment && pos-2>=0 && str[pos-1]=='-' && str[pos-2]=='-') break;
          if (type==Type::DOCTYPE) break;
          if (type==Type::EndTag) break;
          if (type==Type::Tag){
            if (last=="/") last="", end=1, has_last=0;
            break;
          }
        }
        if (!s){
          if (str[pos]==' ' || str[pos]=='\n' || str[pos]=='\t' || str[pos]=='/'){
            if (has_last){
              if (v==0){ // name of tag
                name=last;
                last="";
                has_last=0;
                v=1;
              }else if (v==1){ // no = sign 
                if (p1.size()) params.push_back({p1, "true"}); // parameter without =value
                p1=last;
                last="";
                has_last=0;
              }else if (v==2){ // has = sign
                params.push_back({p1, last});
                last="";
                has_last=0;
                p1="";
                v=1;
              }
            }
            if (str[pos]=='/') last="/", has_last=1;
          }else if (str[pos]=='=' && v==1){ // state from no = sign to has = sign
            if (has_last && p1.size()){
              params.push_back({p1, "true"}); // lonely tag
            }else if (p1.size() && !has_last) last=p1, has_last=1;
            p1=last;
            last="";
            has_last=0;
            v=2;
          }else if (str[pos]=='\''){
            s=1;
          }else if (str[pos]=='"'){
            s=2;
          }else{
            last.push_back(str[pos]);
            has_last=1;
          }
        }else if (s==1){
          if (str[pos]=='\'') s=0;
          else last.push_back(str[pos]);
          has_last=1;
        }else if (s==2){
          if (str[pos]=='"') s=0;
          else last.push_back(str[pos]);
          has_last=1;
        }
      }
      if (v==0 && has_last) name=last;
      else if (v==1 && has_last) params.push_back({last, ""});
      else if (v==1 && p1.size()) params.push_back({p1, ""});
      else if (v==2) params.push_back({p1, last});
      if (!end){
        while (pos<(int)str.size()){
          contents.push_back(XML());
          contents.back().depth=depth+1;
          pos=contents.back().parse(str, pos);
          if (contents.back().type==Type::EndTag && contents.back().name==name){
            break;
          }
        }
      }
    }else{
      type=Type::Data;
      name.push_back(str[pos]);
      for (++pos; pos<(int) str.size() && str[pos]!='<';++pos){
        name.push_back(str[pos]);
      }
    }
  }
  return pos;
}

void MMPFile::XML::find(std::string tag, std::vector<MMPFile::XML*>& res){
  for (int i=0;i<(int)contents.size();++i){
    if (contents[i].type==Type::Tag && contents[i].name==tag) res.push_back(&contents[i]);
  }
}

MMPFile::XML* MMPFile::XML::findOne(std::string tag){
  for (int i=0;i<(int)contents.size();++i){
    if (contents[i].type==Type::Tag && contents[i].name==tag) return &contents[i];
  }
  return NULL;
}
std::pair<bool, std::string> MMPFile::XML::findParam(std::string paramName){
  for (int i=0;i<(int)params.size();++i){
    if (params[i].first==paramName) return {1, params[i].second};
  }
  return {0,""};
}




void MMPFile::Track::setNote(int pos, int len, int key, int vol){
  len=std::max(len, 1); // -192 -> 1
  notes.emplace_back(pos, pos+len, key, vol);
  events.emplace_back(pos, notes.back());
  events.emplace_back(pos+len, notes.back());
  if (vol) lastNoteEndTime=std::max(lastNoteEndTime, (double)(pos+len));
}

void MMPFile::Track::setNote(Note& note, int deltapos) {
  notes.emplace_back(note);
  notes.back().timeBegins+=deltapos;
  notes.back().timeEnds+=deltapos;
  events.emplace_back(notes.back().timeBegins, notes.back());
  events.emplace_back(notes.back().timeEnds, notes.back());
  if (notes.back().volume) lastNoteEndTime=std::max(lastNoteEndTime, (double)(notes.back().timeEnds));
}

void MMPFile::Track::initSimulation(){
  std::sort(events.begin(), events.end());
  currentTime=0;
  currentEventIndex=1;
  for (int i=0;i<108;++i)keyBuffer[i]=0;
}
void MMPFile::Track::getTime(int time, std::vector<double>& rv){
  rv.resize(108);
  if (time>currentTime){
    while (events[currentEventIndex].first<time){
      if (events[currentEventIndex].second.timeBegins==events[currentEventIndex].first) keyBuffer[events[currentEventIndex].second.key]+=events[currentEventIndex].second.volume;
      if (events[currentEventIndex].second.timeEnds==events[currentEventIndex].first) keyBuffer[events[currentEventIndex].second.key]-=events[currentEventIndex].second.volume;
      ++currentEventIndex;
    }
  }else{
    while (events[currentEventIndex-1].first>=time){
      if (events[currentEventIndex-1].second.timeBegins==events[currentEventIndex-1].first) keyBuffer[events[currentEventIndex-1].second.key]-=events[currentEventIndex-1].second.volume;
      if (events[currentEventIndex-1].second.timeEnds==events[currentEventIndex-1].first) keyBuffer[events[currentEventIndex-1].second.key]+=events[currentEventIndex-1].second.volume;
      --currentEventIndex;
    }
  }

  for (int i=0;i<108;++i)rv[i]=(double)keyBuffer[i]/100.0;
}


void MMPFile::parseFromXML(){
  std::vector<XML*> res;
  if ((lmmsProjectNode=root.findOne("lmms-project"))){
    if (XML* head=lmmsProjectNode->findOne("head")){
      std::pair<bool, std::string> v=head->findParam("bpm");
      if (!v.first){
        XML* bpm=head->findOne("bpm");
        if (bpm!=NULL){
          v=bpm->findParam("value");
          if (v.first){
            BPM=std::stoi(v.second);
            if (globalSettings::verboseLevel>1) lout << "[I] MMPFile::parseFromXML " << this << ", BPM (2):" << BPM << LEND;
          }else lout << "[E] MMPFile::parseFromXML " << this << ", NO BPM (2)" << LEND;
        }else  lout << "[E] MMPFile::parseFromXML " << this << ", NO BPM (1)" << LEND;
      }else{
        BPM=std::stoi(v.second);
        if (globalSettings::verboseLevel>1) lout << "[I] MMPFile::parseFromXML " << this << ", BPM:" << BPM << LEND;
      }
    }
    if ((songNode=lmmsProjectNode->findOne("song"))){
      if ((trackContainerNode=songNode->findOne("trackcontainer"))){
        trackContainerNode->find("track", trackNodes);
      }
    }
  }
  std::vector<XML*> patterns;
  std::vector<XML*> bbtcos;
  std::vector<XML*> notes;
  int bbtrackindex=0;
  for (int i=0;i<(int)trackNodes.size();++i){
    if (globalSettings::verboseLevel>1 ) lout << "[I] MMPFile::parseFromXML " << this << ", MPP FILE Track #" << i << ": " << trackNodes[i]->findParam("name").second << LEND;
    XML* bbtrack=trackNodes[i]->findOne("bbtrack");
    tracks.push_back(Track());
    if (bbtrack){
      XML* tc=bbtrack->findOne("trackcontainer");
      if (tc){
        std::vector<XML*> bbtcNodes;
        tc->find("track", bbtcNodes);
        for (int ii=0;ii<(int)bbtcNodes.size();++ii){ // instrument
          if (globalSettings::verboseLevel>1 ) lout << "[I] MMPFile::parseFromXML " << this << ", MPP FILE Track #B" << ii << ": " << bbtcNodes[ii]->findParam("name").second << LEND;
          patterns.clear();
          bbinstruments.emplace_back();
          bbitracks.emplace_back();
          bbtcNodes[ii]->find("pattern", patterns);
          for (int j=0;j<(int)patterns.size();++j){
            int steps=std::stoi(patterns[j]->findParam("steps").second);
            bbinstruments.back().bbtracks.push_back({steps, Track()});
            patterns[j]->find("note", notes);
            for (int k=0;k<(int)notes.size();++k){
              int key=std::stoi(notes[k]->findParam("key").second);
              int vol=std::stoi(notes[k]->findParam("vol").second);
              int pos=std::stoi(notes[k]->findParam("pos").second);
              int len=std::stoi(notes[k]->findParam("len").second);
              bbinstruments.back().bbtracks.back().second.setNote(pos, len, key, vol);
            }
          }
        }
      }
      bbtcos.clear();
      trackNodes[i]->find("bbtco", bbtcos);
      for (int ii=0;ii<(int)bbtcos.size();++ii){
        int pos=std::stoi(bbtcos[ii]->findParam("pos").second);
        int len=std::stoi(bbtcos[ii]->findParam("len").second);
        for (int k=0;k<(int)bbinstruments.size();++k){ // loop through instruments
          if ((int)bbinstruments[k].bbtracks.size()<=bbtrackindex) continue; // this instrument didn't contain patterns enough for this bbtrack
          int lent=bbinstruments[k].bbtracks[bbtrackindex].first*12;
          for (int ppos=pos;ppos<pos+len;ppos+=lent){ // loop through starting positions of beat/bassline
            std::vector<Track::Note>& bnotes=bbinstruments[k].bbtracks[bbtrackindex].second.getNotes();
            for (int ni=0;ni<(int)bnotes.size();++ni){ // add each note
              bbitracks[k].setNote(bnotes[ni], ppos);
            }
          }
        }
      }
      ++bbtrackindex;
    }
    
    
    patterns.clear();
    trackNodes[i]->find("pattern", patterns);
    for (int j=0;j<(int)patterns.size();++j){
      notes.clear();
      patterns[j]->find("note", notes);
      int patternPos=std::stoi(patterns[j]->findParam("pos").second);
      for (int k=0;k<(int)notes.size();++k){
        int key=std::stoi(notes[k]->findParam("key").second);
        int vol=std::stoi(notes[k]->findParam("vol").second);
        int pos=std::stoi(notes[k]->findParam("pos").second);
        int len=std::stoi(notes[k]->findParam("len").second);
        tracks.back().setNote(pos+patternPos, len, key, vol);
      }
    }
    
    tracks.back().initSimulation();
  }
  for (int i=0;i<(int)bbitracks.size();++i){
    bbitracks[i].initSimulation();
  }
}

int MMPFile::read(const char* filename){
  std::ifstream is(filename);
  if (!is.is_open() || !is.good()) return MMP_ERROR_OPENING_ERROR;
  std::string str;
  is.seekg(0, std::ios::end);   
  str.reserve(is.tellg());
  is.seekg(0, std::ios::beg);
  str.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  root.type=XML::Type::Root;
  root.parse(str, 0);
//   root.print(0);
  parseFromXML();
  return 0;
}

void MMPFile::spectrums(int track, std::vector<double>& times, std::vector<std::vector<double> >& ans, bool usebb){
  for (double time : times){
    ans.push_back(std::vector<double>());
    int timeT=timeFromSecs(time);
    if (usebb) bbitracks[track].getTime(timeT, ans.back());
    else       tracks[track].getTime(timeT, ans.back());
  }
}