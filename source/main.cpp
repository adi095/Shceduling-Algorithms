#include <iostream>
#include <stdlib.h>
#include <ctime>  // Include to use time() for random seeding
#include "../headers/Clock.h"
#include "../headers/PCBGenerator.h"
#include "../headers/CPU.h"
#include "../headers/StatUpdater.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Initial args validation
    if(argc < 4){
        cout << "Not enough arguments sent to main." << endl;
        cout << "Format should be: ./lab2 inputfile outputfile algorithm timequantum(if algorithm == 2)" << endl;
        return EXIT_FAILURE;
    }
    if(atoi(argv[3]) == 2 && argc == 4){
        cout << "Need to provide time quantum when using Round Robin algorithm" << endl;
        return EXIT_FAILURE;
    }

    // Variables to hold initial arguments
    int algorithm = atoi(argv[3]);
    int timeq = -1;
    if(algorithm == 2 || algorithm == 4) timeq = atoi(argv[4]);
    if(algorithm == 3) timeq = atoi(argv[4]);
    
    // Seed the random number generator for Preemptive Random (PR) scheduling
    srand(time(0));  // This ensures that each run has different random selections

    // Queues to hold PCBs throughout
    auto ready_queue = new DList<PCB>();
    auto finished_queue = new DList<PCB>();

    try {
        // All the objects that will work together to simulate OS process management
        Clock clock;
        PCBGenerator pgen(argv[1], ready_queue, &clock);
        StatUpdater stats(ready_queue, finished_queue, &clock, algorithm, argv[2], timeq);
        CPU cpu(finished_queue, &clock);
        Scheduler scheduler(ready_queue, &cpu, algorithm, timeq);
        Dispatcher dispatcher(&cpu, &scheduler, ready_queue, &clock);
        scheduler.setdispatcher(&dispatcher);

        // Loop will continue until no more processes are going to be generated, no more in ready queue, and CPU is done
        while (!pgen.finished() || ready_queue->size() || !cpu.isidle()) {
            pgen.generate();
            scheduler.execute();
            dispatcher.execute();
            cpu.execute();
            stats.execute();
            clock.step();
        }

        // Final printing of stats
        stats.print();

    } catch(int) {
        delete ready_queue;
        delete finished_queue;
        return EXIT_FAILURE;
    }

    return 0;
}
