#include "../headers/PCBGenerator.h"

PCBGenerator::PCBGenerator(std::string filename, DList<PCB> *lst, Clock *c) {
    clock = c;
    ready_queue = lst;
    _finished = false;
    last_arr = 0;
    arr_size = 25;
    arrivals = new bool[arr_size];
    pids = new bool[arr_size];
    for(int i = 0; i < arr_size; ++i) {
        arrivals[i] = false;
        pids[i] = false;
    }
    infile.open(filename);
    readnext();
}

PCBGenerator::~PCBGenerator(){
    delete arrivals;
    delete pids;
}

void PCBGenerator::generate(){
    if(!_finished && clock->gettime() >= nextPCB.arrival){
        ready_queue->add_end(nextPCB);
        readnext();
    }
}

void PCBGenerator::readnext(){
    bool error = false;
    std::stringstream ss;
    std::string line;
    float vals[5];

    while (!infile.fail()) {
        getline(infile, line);
        
        if (line.length() <= 2) continue;  // Skip empty lines or lines with too few characters
        ss.clear();                        // Clear the stringstream state for reuse
        ss.str(line);                      // Set the stringstream to the current line

        int count = 0;
        while (count < 4 && ss >> vals[count]) {  // Read up to 4 values
            count++;
        }

        if (count < 4 || ss.fail()) {
            _finished = true;
            return;  // Ensure we handle incomplete lines
        }

        if (vals[0] >= arr_size || vals[1] >= arr_size) doublearrays();  // Resize arrays if needed

        // Error checking logic
        if (vals[1] < 0) {
            std::cerr << "Arrival time can't be less than zero. Exiting now." << std::endl;
            throw 1;
        }
        if (vals[2] <= 0) {
            std::cerr << "CPU Burst time must be greater than 0. Exiting now." << std::endl;
            throw 1;
        }
        if (vals[1] < last_arr) {
            std::cerr << "File needs to be sorted by arrival time. Exiting now." << std::endl;
            throw 1;
        }
        if (pids[int(vals[0])]) {
            std::cerr << "Can't have duplicate PIDs. Exiting now." << std::endl;
            throw 1;
        }
        if (arrivals[int(vals[1])]) {
            std::cerr << "Can't have duplicate arrival times. Exiting now." << std::endl;
            throw 1;
        }

        // No errors, process data
        arrivals[int(vals[1])] = true;
        pids[int(vals[0])] = true;
        nextPCB = PCB(vals[0], vals[1], vals[2], vals[3]);

        return;  // Exit the function after successfully reading one valid line
    }

    // If we reach here, the file has ended
    _finished = true;
}
    

bool PCBGenerator::finished(){
    return _finished;
}

void PCBGenerator::doublearrays(){
    arr_size *= 2;
    auto temp_arrs = new bool[arr_size];
    auto temp_pids = new bool[arr_size];
    for(int i = 0; i < arr_size; ++i) {
        if(i < arr_size/2){
            temp_arrs[i] = arrivals[i];
            temp_pids[i] = pids[i];
        }
        else {
            temp_arrs[i] = false;
            temp_pids[i] = false;
        }
    }
    delete arrivals;
    delete pids;
    arrivals = temp_arrs;
    pids = temp_pids;
}