#include<iostream>
#include<vector>
#include<stdexcept>
#include<iomanip>
#include<map>
#include<memory>
#include<cctype>      // toupper
#include<cstdlib>     // exit
#include<utility>     // move
#include<sstream>     // ostringstream, istringstream
#include<queue>       // priority_queue
#include<ctime>       // time, strftime
#include<fstream>     // ifstream, ofstream
#include<algorithm>   // replace
#include<cmath>       // isfinite
using namespace std;

enum class DroneStatus{AVAILABLE,ON_MISSION,MAINTENANCE};
enum class MissionStatus{PENDING,ASSIGNED,COMPLETED};
enum class Priority{LOW=1,MEDIUM,HIGH};
enum class MissionType{DELIVERY=1,SURVEILLANCE,MAPPING,RESCUE};

string missionStatusToString(MissionStatus s){
    switch(s){
        case MissionStatus::PENDING: return "PENDING";
        case MissionStatus::ASSIGNED: return "ASSIGNED";
        case MissionStatus::COMPLETED: return "COMPLETED";
    }
    return "UNKNOWN";
}

string priorityToString(Priority p){
    switch(p){
        case Priority::LOW: return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH: return "HIGH";
    }
    return "UNKNOWN";
}

string missionTypeToString(MissionType t){
    switch(t){
        case MissionType::DELIVERY:  return "Delivery";
        case MissionType::SURVEILLANCE: return "Surveillance";
        case MissionType::MAPPING:  return "Mapping";
        case MissionType::RESCUE:  return "Rescue";
    }
    return "Unknown";
}

void printMissionHeader(){
    cout<<left<<setw(8)<<"ID"<<setw(14)<<"Type"<<setw(16)<<"Destination"
        <<setw(8)<<"Dist"<<setw(9)<<"Payload"<<setw(10)<<"Priority"
        <<setw(11)<<"Status"<<"Drone"<<endl;
    cout<<string(86,'-')<<endl;
}

class Mission{
    string id;
    MissionType type;
    string destination;
    double distance;
    double payload;
    Priority priority;
    MissionStatus status;
    string assignedDroneId; //empty while PENDING
    int sequence;           //creation order

    public:
        Mission(string id,MissionType type,string destination,double distance,double payload,Priority priority,int sequence)
                :id(id),type(type),destination(destination),distance(distance),payload(payload),
                 priority(priority),status(MissionStatus::PENDING),sequence(sequence){
            if(id.empty())
                throw invalid_argument("Mission ID cannot be empty");
            if(distance<=0)
                throw invalid_argument("Distance must be greater than zero");
            if(payload<0)
                throw invalid_argument("Payload cannot be negative");
        }
        string getId() const {return id;}
        MissionType getType() const { return type;}
        string getDestination() const {return destination;}
        double getDistance() const{ return distance;}
        double getPayload() const{ return payload;}
        Priority getPriority() const { return priority; }
        MissionStatus getStatus() const{return status;}
        string getDroneId() const { return assignedDroneId; }
        int getSequence() const { return sequence; }

        void assignTo(const string& droneId){
            status=MissionStatus::ASSIGNED;
            assignedDroneId=droneId;
        }
        void markCompleted(){status=MissionStatus::COMPLETED; }
        void resetToPending(){
            status=MissionStatus::PENDING;
            assignedDroneId.clear();
        }

        void display() const{
            cout<<left<<setw(8)<<id
                <<setw(14)<<missionTypeToString(type)
                <<setw(16)<<destination
                <<setw(8)<<distance
                <<setw(9)<<payload
                <<setw(10)<<priorityToString(priority)
                <<setw(11)<<missionStatusToString(status)
                <<(assignedDroneId.empty()?"-":assignedDroneId)<<endl;
        }
};

//printDroneHeader() function
void printDroneHeader(){
    cout<<left<<setw(8)<<"ID"<<setw(14)<<"Model"<<setw(14)<<"Type"
            <<setw(8)<<"Batt%"<<setw(8)<<"Range"<<setw(8)<<"Payload"
            <<setw(8)<<"Speed"<<"Status"<<endl;
    cout<<string(86,'-')<<endl;
}

struct InputClosed {};      // thrown when the input stream ends

//Helper functions
string toUpper(string s){
    for(char& c : s) c= toupper(static_cast<unsigned char>(c));
    return s;
}

string readText(const string& prompt){
    string s;
    while(true){
        cout<<prompt;
        if(!getline(cin,s)) throw InputClosed();
        replace(s.begin(), s.end(), '|', '/');   // '|' separates fields in the save files
        if(!s.empty()) return s;
        cout<<"Input cannot be empty.\n";
    }
}

double readNumber(const string& prompt){
    string s;
    while(true){
        cout<<prompt;
        if(!getline(cin,s)) throw InputClosed();
        try{
            size_t pos;
            double v=stod(s,&pos);
            if(pos==s.size() && isfinite(v)) return v; //whole text was a real number
        } catch(const exception&){}
        cout<<"Please enter a valid number.\n";
    }
}

int readInt(const string& prompt,int lo,int hi){
    string s;
    while(true){
        cout<<prompt;
        if(!getline(cin,s)) throw InputClosed();
        try{
            size_t pos;
            int v=stoi(s,&pos);
            if(pos==s.size() && v>=lo && v<=hi) return v;
        }catch(const exception&){}
        cout<<"Please enter a whole number from "<<lo<<" to "<<hi<<".\n";
    }
}

string readOptionalText(const string& prompt) {      // empty answer is allowed
    string s;
    cout << prompt;
    if (!getline(cin, s)) throw InputClosed();
    return s;
}

//formats a number with 1 decimal place
string fmt(double v){
    ostringstream out;
    out<<fixed<<setprecision(1)<<v;
    return out.str();
}

string statustoString(DroneStatus s){
    switch(s){
        case DroneStatus::AVAILABLE: return "AVAILABLE";
        case DroneStatus::ON_MISSION: return "ON_MISSION";
        case DroneStatus::MAINTENANCE: return "MAINTENANCE";
    }
    return "UNKNOWN";
}

bool readYesNo(const string& prompt) {
    while (true) {
        string a = toUpper(readText(prompt + " (y/n): "));
        if (a == "Y" || a == "YES") return true;
        if (a == "N" || a == "NO") return false;
        cout << "Please answer y or n.\n";
    }
}

string currentTime() {
    time_t t = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M", localtime(&t));
    return buffer;
}

// ----- helpers for reading the save files -----
vector<string> split(const string& line, char delimiter) {
    vector<string> parts;
    string field;
    istringstream stream(line);
    while (getline(stream, field, delimiter)) parts.push_back(field);
    if (!line.empty() && line.back() == delimiter) parts.push_back("");
    return parts;
}

double parseNumber(const string& s) {
    size_t pos;
    double v = stod(s, &pos);
    if (pos != s.size() || !isfinite(v)) throw invalid_argument("bad number");
    return v;
}

int parseInt(const string& s) {
    size_t pos;
    int v = stoi(s, &pos);
    if (pos != s.size()) throw invalid_argument("bad number");
    return v;
}

class Drone{
    string ID;
    string model;
    double battery;
    double range;
    double payload;
    double speed;
    DroneStatus status;
    public:
        Drone(string id,string model,double battery,double range,double payload,double speed): ID(id),model(model),battery(battery),
        range(range),payload(payload),speed(speed),status(DroneStatus::AVAILABLE){
            if (battery < 0 || battery > 100)
                throw invalid_argument("Battery must be 0-100");
            if (range <= 0 || speed <= 0)
                throw invalid_argument("Range and speed must be positive");
            if (payload < 0)
                throw invalid_argument("Payload cannot be negative");
        }
        virtual ~Drone(){}   //virtual destructor
        virtual string gettypename() const=0; //pure virtual function
        virtual void performMission(const string&destination)=0; //pure virtual function
        virtual double getEfficiencyFactor() const=0; //pure virtual (battery use)

        string getId()const {return ID;}
        string getModel() const {return model;}
        double getBattery() const{return battery;}
        double Range() const {return range;}
        double Payload() const{return payload;}
        double Speed() const{return speed;}
        DroneStatus getStatus() const{return status;}
        void setStatus(DroneStatus s) { status = s; }

        void charge(double amount) {
            battery += amount;
            if (battery > 100) battery = 100;   // cap at 100
        }

        void useBattery(double amount) {
            battery -= amount;
            if (battery < 0) battery = 0;       // never negative
        }
        static constexpr double RESERVE_BATTERY = 10.0;   // % that must remain after a mission

        // Simplified battery model: share of range used, scaled by drone type and load
        double estimateConsumption(const Mission& m) const {
            double base = (m.getDistance() / range) * 100.0 * getEfficiencyFactor();
            double load = (payload > 0) ? 1.0 + 0.5 * (m.getPayload() / payload) : 1.0;
            return base * load;
        }

        // Returns "" if the drone can take the mission, otherwise the reason it cannot
        string checkSuitability(const Mission& m) const {
            if (status != DroneStatus::AVAILABLE)
                return "not available (" + statustoString(status) + ")";
            if (m.getPayload() > payload)
                return "payload too high (max " + fmt(payload) + " kg)";
            if (m.getDistance() > range)
                return "range too short (max " + fmt(range) + " km)";
            double needed = estimateConsumption(m) + RESERVE_BATTERY;
            if (battery < needed)
                return "battery too low (needs " + fmt(needed) + "%, has " + fmt(battery) + "%)";
            return "";
        }

        void display() const{
            cout<<left<<setw(8)<<ID
            <<setw(14)<<model
            <<setw(14)<<gettypename()
            <<setw(8)<<battery
            <<setw(8)<<range
            <<setw(8)<<payload
            <<setw(8)<<speed
            <<statustoString(status)<<endl;
        }
};

class DeliveryDrone: public Drone{
public:
    DeliveryDrone(string id,string model,double battery,double range,
                        double payload,double speed):Drone(id,model,battery,range,payload,speed){}
    string gettypename() const override{ return "Delivery";}

    void performMission(const string& destination) override{
        cout<<getId()<<"(Delivery): delivering package to "<<destination<<endl;
    }
    double getEfficiencyFactor() const override { return 1.10; }
};

class SurveillanceDrone: public Drone{
public:
    SurveillanceDrone(string id,string model,double battery,double range,
                        double payload,double speed):Drone(id,model,battery,range,payload,speed){}
    string gettypename() const override{ return "Surveillance";}

    void performMission(const string& destination) override{
        cout<<getId()<<"(Surveillance): patrolling "<<destination<<endl;
    }
    double getEfficiencyFactor() const override { return 0.85; }
};

class MappingDrone: public Drone{
public:
    MappingDrone(string id,string model,double battery,double range,
                        double payload,double speed):Drone(id,model,battery,range,payload,speed){}
    string gettypename() const override{ return "Mapping";}

    void performMission(const string& destination) override{
        cout<<getId()<<"(Mapping): Finding Location of "<<destination<<endl;
    }
    double getEfficiencyFactor() const override { return 0.95; }
};

class RescueDrone: public Drone{
public:
    RescueDrone(string id,string model,double battery,double range,
                        double payload,double speed):Drone(id,model,battery,range,payload,speed){}
    string gettypename() const override{ return "Rescue";}

    void performMission(const string& destination) override{
        cout<<getId()<<"(Rescue): Help Arriving "<<destination<<endl;
    }
    double getEfficiencyFactor() const override { return 1.25; }
};

// Builds the right drone class from its type name (used when loading files)
unique_ptr<Drone> createDrone(const string& type,const string& id,const string& model,
                              double battery,double range,double payload,double speed){
    if(type=="Delivery")
        return make_unique<DeliveryDrone>(id,model,battery,range,payload,speed);
    if(type=="Surveillance")
        return make_unique<SurveillanceDrone>(id,model,battery,range,payload,speed);
    if(type=="Mapping")
        return make_unique<MappingDrone>(id,model,battery,range,payload,speed);
    if(type=="Rescue")
        return make_unique<RescueDrone>(id,model,battery,range,payload,speed);
    throw invalid_argument("Unknown drone type: "+type);
}

/*a small struct above FleetManager to carry the outcome back and why each drone was rejected*/
struct AssignmentResult {
    bool assigned = false;
    string droneId;
    vector<string> reasons;    // why each drone was rejected
};

class MaintenanceRecord {
    string droneId;
    string date;
    string description;
public:
    // new record: the date is stamped automatically
    MaintenanceRecord(string droneId, string description)
        : droneId(droneId), date(currentTime()), description(description) {}

    // used when loading a saved record, so the original date is kept
    MaintenanceRecord(string droneId, string description, string date)
        : droneId(droneId), date(date), description(description) {}

    string getDroneId() const { return droneId; }
    string getDate() const { return date; }
    string getDescription() const { return description; }
};

struct CompletionResult {
    string droneId;
    double batteryUsed;
    double batteryLeft;
};

struct LoadSummary {
    int drones = 0, missions = 0, records = 0, skipped = 0;
};

class FleetManager{
    map<string,unique_ptr<Drone>> drones;
    map<string, Mission> missions;       // key = mission ID
    int nextSequence = 1;
    vector<MaintenanceRecord> maintenanceLog;
public:
    bool hasDrone(const string& id) const { return drones.count(id)>0;}
    size_t count() const{ return drones.size();}

    void addDrone(unique_ptr<Drone> drone){
        string id=drone->getId();
        if(hasDrone(id))
            throw invalid_argument("Drone ID "+ id +" already exists");
        drones[id]=move(drone);
    }

    const Drone* findDrone(const string& id) const{
        auto it=drones.find(id);
        if(it==drones.end()) return nullptr;
        return it->second.get();
    }

    void removeDrone(const string& id){
        auto it=drones.find(id);
        if(it==drones.end())
            throw invalid_argument("No drone found with ID "+id);
        if(it->second->getStatus()==DroneStatus::ON_MISSION)
            throw runtime_error("Drone "+id+" is on a mission and cannot be removed");
        drones.erase(it);
    }

    void displayAll() const{
        if(drones.empty()){
            cout<<"No drones registered yet."<<endl;
            return;
        }
        printDroneHeader();
        for(const auto&entry : drones)
            entry.second->display();
    }

    //-----missions-------
    bool hasMission(const string& id)const {return missions.count(id)>0;}
    size_t missionCount() const{return missions.size();}

    void addMission(const string& id,MissionType type,const string& destination,double distance
                    ,double payload,Priority priority){
        if(hasMission(id))
            throw invalid_argument("Mission ID "+id+" already exists");
        missions.emplace(id,Mission(id,type,destination,distance,payload,
                                    priority,nextSequence));
        nextSequence++;      // only reached if the Mission was created successfully
    }

    void displayAllMissions() const{
        if(missions.empty()){
            cout<<"No missions created yet."<<endl;
            return;
        }
        printMissionHeader();
        for(const auto& entry : missions)
            entry.second.display();
    }

    size_t displayPendingMissions() const {
        size_t shown = 0;
        for (const auto& entry : missions) {
            if (entry.second.getStatus() != MissionStatus::PENDING) continue;
            if (shown == 0) printMissionHeader();
            entry.second.display();
            shown++;
        }
        return shown;
    }

    // priority_queue keeps the most urgent PENDING mission on top
    string highestPriorityPendingId() const {
        struct Entry { Priority priority; int sequence; string id; };
        struct Order {
            bool operator()(const Entry& a, const Entry& b) const {
                if (a.priority != b.priority) return a.priority < b.priority;  // higher priority first
                return a.sequence > b.sequence;                                // then older mission first
            }
        };
        priority_queue<Entry, vector<Entry>, Order> queue;
        for (const auto& entry : missions)
            if (entry.second.getStatus() == MissionStatus::PENDING)
                queue.push({entry.second.getPriority(), entry.second.getSequence(), entry.first});
        return queue.empty() ? "" : queue.top().id;
    }

    AssignmentResult assignMission(const string& missionId) {
        auto mit = missions.find(missionId);
        if (mit == missions.end())
            throw invalid_argument("No mission found with ID " + missionId);
        Mission& mission = mit->second;
        if (mission.getStatus() != MissionStatus::PENDING)
            throw runtime_error("Mission " + missionId + " is already " +
                                missionStatusToString(mission.getStatus()));

        AssignmentResult result;
        Drone* best = nullptr;
        for (auto& entry : drones) {
            Drone* candidate = entry.second.get();
            string reason = candidate->checkSuitability(mission);
            if (!reason.empty()) {
                result.reasons.push_back(candidate->getId() + ": " + reason);
                continue;
            }
            if (best == nullptr) { best = candidate; continue; }
            // prefer a drone whose type matches the mission, then the higher battery
            string wanted = missionTypeToString(mission.getType());
            bool candMatch = candidate->gettypename() == wanted;
            bool bestMatch = best->gettypename() == wanted;
            if (candMatch != bestMatch) { if (candMatch) best = candidate; }
            else if (candidate->getBattery() > best->getBattery()) best = candidate;
        }
        if (best == nullptr) return result;      // mission stays PENDING

        best->setStatus(DroneStatus::ON_MISSION);
        mission.assignTo(best->getId());
        result.assigned = true;
        result.droneId = best->getId();
        return result;
    }

    // ===== Stage 7: complete, charge, maintenance =====
    size_t displayAssignedMissions() const {
        size_t shown = 0;
        for (const auto& entry : missions) {
            if (entry.second.getStatus() != MissionStatus::ASSIGNED) continue;
            if (shown == 0) printMissionHeader();
            entry.second.display();
            shown++;
        }
        return shown;
    }

    CompletionResult completeMission(const string& missionId) {
        auto mit = missions.find(missionId);
        if (mit == missions.end())
            throw invalid_argument("No mission found with ID " + missionId);
        Mission& mission = mit->second;
        if (mission.getStatus() != MissionStatus::ASSIGNED)
            throw runtime_error("Mission " + missionId + " is " +
                                missionStatusToString(mission.getStatus()) +
                                "; only ASSIGNED missions can be completed");
        Drone& drone = requireDrone(mission.getDroneId());

        double before = drone.getBattery();
        double used = drone.estimateConsumption(mission);
        drone.performMission(mission.getDestination());     // type-specific message
        drone.useBattery(used);                             // never goes below 0
        mission.markCompleted();
        drone.setStatus(DroneStatus::AVAILABLE);
        return {drone.getId(), before - drone.getBattery(), drone.getBattery()};
    }

    double chargeDrone(const string& id, double amount) {
        Drone& drone = requireDrone(id);
        if (amount <= 0 || amount > 100)
            throw invalid_argument("Charge amount must be above 0 and at most 100");
        if (drone.getStatus() == DroneStatus::ON_MISSION)
            throw runtime_error("Drone " + id + " is on a mission and cannot be charged");
        if (drone.getStatus() == DroneStatus::MAINTENANCE)
            throw runtime_error("Drone " + id + " is under maintenance and cannot be charged");
        if (drone.getBattery() >= 100)
            throw runtime_error("Drone " + id + " is already fully charged");
        drone.charge(amount);                               // capped at 100 inside Drone
        return drone.getBattery();
    }

    bool isUnderMaintenance(const string& id) const {
        const Drone* d = findDrone(id);
        return d != nullptr && d->getStatus() == DroneStatus::MAINTENANCE;
    }

    void startMaintenance(const string& id, const string& description) {
        Drone& drone = requireDrone(id);
        if (drone.getStatus() == DroneStatus::ON_MISSION)
            throw runtime_error("Drone " + id + " is on a mission; complete it first");
        if (drone.getStatus() == DroneStatus::MAINTENANCE)
            throw runtime_error("Drone " + id + " is already under maintenance");
        drone.setStatus(DroneStatus::MAINTENANCE);
        maintenanceLog.push_back(MaintenanceRecord(id, description));
    }

    void finishMaintenance(const string& id) {
        Drone& drone = requireDrone(id);
        if (drone.getStatus() != DroneStatus::MAINTENANCE)
            throw runtime_error("Drone " + id + " is not under maintenance");
        drone.setStatus(DroneStatus::AVAILABLE);
    }

    void displayMaintenanceHistory(const string& id) const {
        bool headerShown = false;
        for (const auto& r : maintenanceLog) {
            if (r.getDroneId() != id) continue;
            if (!headerShown) {
                cout << "\nMaintenance history of " << id << ":" << endl;
                headerShown = true;
            }
            cout << "  " << r.getDate() << "  " << r.getDescription() << endl;
        }
    }

    // ===== Stage 8: statistics and files =====
    void displayStatistics() const {
        if (drones.empty() && missions.empty()) {
            cout << "The fleet is empty. Register drones and create missions first." << endl;
            return;
        }

        cout << "DRONES" << endl;
        cout << "  Total registered      : " << drones.size() << endl;
        const char* types[] = {"Delivery", "Surveillance", "Mapping", "Rescue"};
        for (const char* t : types) {
            int n = 0;
            for (const auto& e : drones)
                if (e.second->gettypename() == t) n++;
            cout << "    " << left << setw(18) << t << ": " << n << endl;
        }
        int available = 0, onMission = 0, maintenance = 0;
        double totalBattery = 0;
        for (const auto& e : drones) {
            DroneStatus st = e.second->getStatus();
            if (st == DroneStatus::AVAILABLE) available++;
            else if (st == DroneStatus::ON_MISSION) onMission++;
            else maintenance++;
            totalBattery += e.second->getBattery();
        }
        cout << "  AVAILABLE             : " << available << endl;
        cout << "  ON_MISSION            : " << onMission << endl;
        cout << "  MAINTENANCE           : " << maintenance << endl;
        if (!drones.empty())
            cout << "  Average battery       : " << fmt(totalBattery / drones.size()) << " %" << endl;

        int pending = 0, assigned = 0, completed = 0;
        double distanceFlown = 0;
        map<string, int> completedBy;                 // drone ID -> missions completed
        for (const auto& e : missions) {
            const Mission& m = e.second;
            if (m.getStatus() == MissionStatus::PENDING) pending++;
            else if (m.getStatus() == MissionStatus::ASSIGNED) assigned++;
            else {
                completed++;
                distanceFlown += m.getDistance();
                completedBy[m.getDroneId()]++;
            }
        }
        cout << "\nMISSIONS" << endl;
        cout << "  Total created         : " << missions.size() << endl;
        cout << "  PENDING               : " << pending << endl;
        cout << "  ASSIGNED              : " << assigned << endl;
        cout << "  COMPLETED             : " << completed << endl;
        cout << "  Distance flown        : " << fmt(distanceFlown) << " km (simulated)" << endl;
        string busiest;
        int most = 0;
        for (const auto& e : completedBy)
            if (e.second > most) { most = e.second; busiest = e.first; }
        if (most > 0)
            cout << "  Most active drone     : " << busiest << " (" << most << " missions)" << endl;

        cout << "\nMAINTENANCE" << endl;
        cout << "  Records logged        : " << maintenanceLog.size() << endl;
    }

    void save() const {
        ofstream dronesFile("drones.txt");
        if (!dronesFile) throw runtime_error("Could not write drones.txt");
        dronesFile << setprecision(10);
        for (const auto& e : drones) {
            const Drone& d = *e.second;
            dronesFile << d.gettypename() << '|' << d.getId() << '|' << d.getModel() << '|'
                       << d.getBattery() << '|' << d.Range() << '|' << d.Payload() << '|'
                       << d.Speed() << '|' << static_cast<int>(d.getStatus()) << '\n';
        }

        ofstream missionsFile("missions.txt");
        if (!missionsFile) throw runtime_error("Could not write missions.txt");
        missionsFile << setprecision(10);
        for (const auto& e : missions) {
            const Mission& m = e.second;
            missionsFile << m.getId() << '|' << static_cast<int>(m.getType()) << '|'
                         << m.getDestination() << '|' << m.getDistance() << '|'
                         << m.getPayload() << '|' << static_cast<int>(m.getPriority()) << '|'
                         << static_cast<int>(m.getStatus()) << '|' << m.getDroneId() << '|'
                         << m.getSequence() << '\n';
        }

        ofstream recordsFile("maintenance.txt");
        if (!recordsFile) throw runtime_error("Could not write maintenance.txt");
        for (const auto& r : maintenanceLog)
            recordsFile << r.getDroneId() << '|' << r.getDate() << '|' << r.getDescription() << '\n';
    }

    LoadSummary load() {
        LoadSummary summary;
        string line;

        ifstream dronesFile("drones.txt");           // a missing file simply reads nothing
        while (getline(dronesFile, line)) {
            if (line.empty()) continue;
            try { loadDroneLine(line); summary.drones++; }
            catch (const exception&) { summary.skipped++; }
        }
        ifstream missionsFile("missions.txt");
        while (getline(missionsFile, line)) {
            if (line.empty()) continue;
            try { loadMissionLine(line); summary.missions++; }
            catch (const exception&) { summary.skipped++; }
        }
        ifstream recordsFile("maintenance.txt");
        while (getline(recordsFile, line)) {
            if (line.empty()) continue;
            vector<string> f = split(line, '|');
            if (f.size() == 3) {
                maintenanceLog.push_back(MaintenanceRecord(f[0], f[2], f[1]));
                summary.records++;
            } else {
                summary.skipped++;
            }
        }
        reconcile();
        return summary;
    }

private:
    Drone& requireDrone(const string& id) {
        auto it = drones.find(id);
        if (it == drones.end())
            throw invalid_argument("No drone found with ID " + id);
        return *it->second;
    }

    void loadDroneLine(const string& line) {
        vector<string> f = split(line, '|');
        if (f.size() != 8) throw invalid_argument("bad drone line");
        int status = parseInt(f[7]);
        if (status < 0 || status > 2) throw invalid_argument("bad status");
        unique_ptr<Drone> d = createDrone(f[0], f[1], f[2], parseNumber(f[3]),
                                          parseNumber(f[4]), parseNumber(f[5]), parseNumber(f[6]));
        d->setStatus(static_cast<DroneStatus>(status));
        addDrone(move(d));                           // throws on a duplicate ID
    }

    void loadMissionLine(const string& line) {
        vector<string> f = split(line, '|');
        if (f.size() != 9) throw invalid_argument("bad mission line");
        int type = parseInt(f[1]), priority = parseInt(f[5]);
        int status = parseInt(f[6]), sequence = parseInt(f[8]);
        if (type < 1 || type > 4 || priority < 1 || priority > 3 || status < 0 || status > 2)
            throw invalid_argument("bad mission value");
        if (hasMission(f[0])) throw invalid_argument("duplicate mission");

        Mission m(f[0], static_cast<MissionType>(type), f[2], parseNumber(f[3]),
                  parseNumber(f[4]), static_cast<Priority>(priority), sequence);
        if (status != 0) m.assignTo(f[7]);           // ASSIGNED or COMPLETED
        if (status == 2) m.markCompleted();
        missions.emplace(m.getId(), m);
        if (sequence >= nextSequence) nextSequence = sequence + 1;
    }

    // Repairs links between drones and missions after loading (e.g. a hand-edited file)
    void reconcile() {
        for (auto& e : missions) {
            Mission& m = e.second;
            if (m.getStatus() != MissionStatus::ASSIGNED) continue;
            auto it = drones.find(m.getDroneId());
            if (it == drones.end())
                m.resetToPending();                              // its drone no longer exists
            else
                it->second->setStatus(DroneStatus::ON_MISSION);
        }
        for (auto& e : drones) {
            if (e.second->getStatus() != DroneStatus::ON_MISSION) continue;
            bool hasMissionAssigned = false;
            for (const auto& me : missions)
                if (me.second.getStatus() == MissionStatus::ASSIGNED &&
                    me.second.getDroneId() == e.first)
                    hasMissionAssigned = true;
            if (!hasMissionAssigned) e.second->setStatus(DroneStatus::AVAILABLE);
        }
    }
};

// ===================== menu actions =====================
void createMission(FleetManager& fleet){
    string id=toUpper(readText("Mission ID          : "));
    if(fleet.hasMission(id))
        throw invalid_argument("Mission ID "+id+" already exists");
    cout<<"  1. Delivery  2. Surveillance  3. Mapping  4. Rescue"<<endl;
    int type=readInt("Mission type (1-4)  : ",1,4);
    string destination=readText("Destination         : ");
    double distance = readNumber("Distance (km)       : ");
    double payload  = readNumber("Payload (kg)        : ");
    cout<<"  1. Low  2. Medium  3. High"<<endl;
    int priority=readInt("Priority (1-3)      : ",1,3);
    fleet.addMission(id,static_cast<MissionType>(type),destination,distance,payload,static_cast<Priority>(priority));
    cout<<"Mission "<<id<<" created with status PENDING."<<endl;
}

//registerDrone(FleetManager& fleet)
void registerDrone(FleetManager& fleet){
    string id=toUpper(readText("Drone ID            : "));
    if(fleet.hasDrone(id))
        throw invalid_argument("Drone ID "+ id+" already exists");
    string model=readText("Name / Model        : ");
    cout<<"  1. Delivery  2. Surveillance  3. Mapping  4. Rescue"<<endl;
    int type=readInt("Type (1-4)          : ",1,4);
    double battery=readNumber("Battery (%)         : ");
    double range =readNumber("Max range (km)      : ");
    double payload = readNumber("Max payload (kg)    : ");
    double speed= readNumber("Max speed (km/h)    : ");

    unique_ptr<Drone> drone;
    switch(type){
        case 1: drone = make_unique<DeliveryDrone>(id,model,battery,range,payload,speed);break;
        case 2: drone = make_unique<SurveillanceDrone>(id,model,battery,range,payload,speed); break;
        case 3: drone=make_unique<MappingDrone>(id,model,battery,range,payload,speed); break;
        case 4: drone=make_unique<RescueDrone>(id,model,battery,range,payload,speed);break;
    }
    fleet.addDrone(move(drone));
    cout<<"Drone "<<id<<" registered successfully."<<endl;
}

void searchDrone(const FleetManager& fleet) {
    string id = toUpper(readText("Enter Drone ID to search: "));
    const Drone* d = fleet.findDrone(id);
    if (d == nullptr) {
        cout << "No drone found with ID " << id << "." << endl;
        return;
    }
    printDroneHeader();
    d->display();
}

void removeDrone(FleetManager& fleet) {
    if (fleet.count() == 0) {
        cout << "There are no drones to remove." << endl;
        return;
    }
    fleet.displayAll();
    string id = toUpper(readText("\nEnter Drone ID to remove: "));
    string answer = toUpper(readText("Remove drone " + id + "? (y/n): "));
    if (answer != "Y") {
        cout << "Removal cancelled." << endl;
        return;
    }
    fleet.removeDrone(id);
    cout << "Drone " << id << " removed." << endl;
}

void assignMission(FleetManager& fleet) {
    if (fleet.displayPendingMissions() == 0) {
        cout << "There are no PENDING missions to assign." << endl;
        return;
    }
    string id = toUpper(readOptionalText("\nMission ID (Enter = highest priority): "));
    if (id.empty()) {
        id = fleet.highestPriorityPendingId();
        cout << "Selected highest-priority mission: " << id << endl;
    }

    AssignmentResult result = fleet.assignMission(id);
    if (result.assigned) {
        cout << "Mission " << id << " is now ASSIGNED to drone " << result.droneId
             << " (drone status: ON_MISSION)." << endl;
    } else {
        cout << "No suitable drone found. Mission " << id << " remains PENDING." << endl;
        for (const string& reason : result.reasons)
            cout << "   - " << reason << endl;
        if (result.reasons.empty())
            cout << "   - no drones are registered" << endl;
    }
}

void completeMission(FleetManager& fleet) {
    if (fleet.displayAssignedMissions() == 0) {
        cout << "There are no ASSIGNED missions to complete." << endl;
        return;
    }
    string id = toUpper(readText("\nEnter Mission ID to complete: "));
    CompletionResult r = fleet.completeMission(id);

    cout << "Mission " << id << " marked COMPLETED." << endl;
    cout << "Drone " << r.droneId << " used " << r.batteryUsed << "% battery; "
         << r.batteryLeft << "% remaining. Status: AVAILABLE." << endl;
    cout << "(Battery use is a simplified simulation, not real flight data.)" << endl;
    if (r.batteryLeft < 30)
        cout << "Warning: battery is low. Consider charging drone " << r.droneId << "." << endl;
}

void chargeDrone(FleetManager& fleet) {
    if (fleet.count() == 0) {
        cout << "There are no drones to charge." << endl;
        return;
    }
    fleet.displayAll();
    string id = toUpper(readText("\nEnter Drone ID to charge: "));
    double amount = readNumber("Charge to add (%)       : ");
    double now = fleet.chargeDrone(id, amount);
    cout << "Drone " << id << " charged. Battery is now " << now << "%." << endl;
}

void recordMaintenance(FleetManager& fleet) {
    if (fleet.count() == 0) {
        cout << "There are no drones registered." << endl;
        return;
    }
    fleet.displayAll();
    string id = toUpper(readText("\nEnter Drone ID: "));
    if (!fleet.hasDrone(id))
        throw invalid_argument("No drone found with ID " + id);
    fleet.displayMaintenanceHistory(id);

    if (fleet.isUnderMaintenance(id)) {
        cout << "Drone " << id << " is currently under maintenance." << endl;
        if (readYesNo("Mark maintenance as finished?")) {
            fleet.finishMaintenance(id);
            cout << "Drone " << id << " is AVAILABLE again." << endl;
        }
        return;
    }
    string work = readText("Maintenance details: ");
    fleet.startMaintenance(id, work);
    cout << "Maintenance recorded. Drone " << id
         << " is now under MAINTENANCE and cannot be assigned missions." << endl;
}

// ----- Stage 8 menu helpers -----
void loadRecords(FleetManager& fleet) {
    LoadSummary s = fleet.load();
    if (s.drones + s.missions + s.records + s.skipped == 0) {
        cout << "No saved records found. Starting with an empty fleet." << endl;
        return;
    }
    cout << "Loaded " << s.drones << " drone(s), " << s.missions << " mission(s) and "
         << s.records << " maintenance record(s)." << endl;
    if (s.skipped > 0)
        cout << "Warning: " << s.skipped << " unreadable line(s) were skipped." << endl;
}

void saveRecords(const FleetManager& fleet) {
    try {
        fleet.save();
        cout << "Records saved to drones.txt, missions.txt and maintenance.txt." << endl;
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void showMenu() {
    cout << "----------------------------------------------------------------" << endl;
    cout << "               DRONE FLEET MANAGEMENT SYSTEM               " << endl;
    cout << "----------------------------------------------------------------" << endl;
    cout << "1. Register Drone" << endl;
    cout << "2. View All Drones" << endl;
    cout << "3. Search Drone" << endl;
    cout << "4. Remove Drone" << endl;
    cout << "5. Create Mission" << endl;
    cout << "6. View All Missions" << endl;
    cout << "7. Assign Mission" << endl;
    cout << "8. Complete Mission" << endl;
    cout << "9. Charge Drone" << endl;
    cout << "10. Record Maintenance" << endl;
    cout << "11. Fleet Statistics" << endl;
    cout << "12. Save Data" << endl;
    cout << "0. Exit" << endl;
    cout << endl;
}

int main(){
    FleetManager fleet;
    loadRecords(fleet);                       // read saved files at start-up
    int n=-1;
    try{
        do{
            showMenu();
            n=readInt("Enter your choice: ",0,12);
            cout<<endl;
            try{
                switch(n){
                    case 1: registerDrone(fleet);break;
                    case 2: fleet.displayAll();break;
                    case 3: searchDrone(fleet);break;
                    case 4: removeDrone(fleet);break;
                    case 5: createMission(fleet);break;
                    case 6: fleet.displayAllMissions();break;
                    case 7: assignMission(fleet);break;
                    case 8: completeMission(fleet);break;
                    case 9: chargeDrone(fleet);break;
                    case 10: recordMaintenance(fleet);break;
                    case 11: fleet.displayStatistics();break;
                    case 12: saveRecords(fleet);break;
                    case 0: cout<<"Saving records and exiting. Goodbye!"<<endl;break;
                }
            }catch(const exception& e){
                cout<<"Error: "<<e.what()<<endl;
            }
            cout<<endl;
        }while(n!=0);
    }catch(const InputClosed&){
        cout<<"\nInput closed. Saving records before exit."<<endl;
    }
    saveRecords(fleet);                       // always save on the way out
    return 0;
}