//FCFS
 
 #include <stdio.h>

// A structure to hold all process info
struct Process {
    int pid;      // Process ID
    int bt;       // Burst Time
    int wt;       // Waiting Time
    int tat;      // Turnaround Time
};

// Function to calculate waiting times for all processes
void findWaitingTimes_FCFS(struct Process proc[], int n) {
    
    // Waiting time for the *first* process is always 0
    proc[0].wt = 0;

    // Calculate waiting time for the rest of the processes
    for (int i = 1; i < n; i++) {
        // The wait time for this process is the
        // burst time + wait time of the process *before* it.
        // This is just the "finish time" of the previous process.
        proc[i].wt = proc[i - 1].bt + proc[i - 1].wt;
    }
}

// Function to calculate turnaround time for all processes
void findTurnaroundTimes_FCFS(struct Process proc[], int n) {
    // Turnaround Time = Burst Time + Waiting Time
    for (int i = 0; i < n; i++) {
        proc[i].tat = proc[i].bt + proc[i].wt;
    }
}

// Main function to calculate and print everything
void findAverageTimes_FCFS(struct Process proc[], int n) {
    float total_wt = 0;
    float total_tat = 0;

    // --- Step 1: Calculate wait times ---
    findWaitingTimes_FCFS(proc, n);

    // --- Step 2: Calculate turnaround times ---
    findTurnaroundTimes_FCFS(proc, n);

    // --- Step 3: Print details and calculate totals ---
    printf("FCFS Scheduling\n");
    printf("Processes  Burst Time  Waiting Time  Turnaround Time\n");

    for (int i = 0; i < n; i++) {
        total_wt += proc[i].wt;
        total_tat += proc[i].tat;
        printf("   P%d \t\t %d \t\t %d \t\t %d\n",
               proc[i].pid, proc[i].bt, proc[i].wt, proc[i].tat);
    }

    // --- Step 4: Print averages ---
    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
}

// --- Main execution ---
int main() {
    // Let's create 3 processes
    // proc[] = { {ID, BurstTime}, ... }
    struct Process proc[] = {{1, 24}, {2, 3}, {3, 3}};
    
    // Get the number of processes
    int n = sizeof(proc) / sizeof(proc[0]); // This just gets the array size (3)

    findAverageTimes_FCFS(proc, n);
    return 0;
}



// ----------------------------------
//SJF

#include <stdio.h>

// The struct is exactly the same as FCFS
struct Process {
    int pid;      // Process ID
    int bt;       // Burst Time
    int wt;       // Waiting Time
    int tat;      // Turnaround Time
};

// --- These two functions are 100% IDENTICAL to FCFS ---
void findWaitingTimes_SJF(struct Process proc[], int n) {
    proc[0].wt = 0;
    for (int i = 1; i < n; i++) {
        proc[i].wt = proc[i - 1].bt + proc[i - 1].wt;
    }
}
void findTurnaroundTimes_SJF(struct Process proc[], int n) {
    for (int i = 0; i < n; i++) {
        proc[i].tat = proc[i].bt + proc[i].wt;
    }
}
// --------------------------------------------------------

// A simple function to swap two processes (for sorting)
void swap(struct Process *xp, struct Process *yp) {
    struct Process temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A simple Bubble Sort to sort processes by Burst Time
void sort_by_burst_time(struct Process proc[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // If the current process's BT is > the next one's BT, swap them
            if (proc[j].bt > proc[j + 1].bt) {
                swap(&proc[j], &proc[j + 1]);
            }
        }
    }
}

// Main function to calculate and print everything
void findAverageTimes_SJF(struct Process proc[], int n) {
    float total_wt = 0;
    float total_tat = 0;

    // --- THIS IS THE ONLY NEW STEP ---
    // Sort the processes by Burst Time *before* calculating
    sort_by_burst_time(proc, n);
    // ---------------------------------

    printf("SJF Scheduling (after sorting)\n");

    // The rest is identical to FCFS
    findWaitingTimes_SJF(proc, n);
    findTurnaroundTimes_SJF(proc, n);

    printf("Processes  Burst Time  Waiting Time  Turnaround Time\n");
    for (int i = 0; i < n; i++) {
        total_wt += proc[i].wt;
        total_tat += proc[i].tat;
        printf("   P%d \t\t %d \t\t %d \t\t %d\n",
               proc[i].pid, proc[i].bt, proc[i].wt, proc[i].tat);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
}


// --- Main execution ---
int main() {
    // Same processes as before
    // {ID, BurstTime}
    struct Process proc[] = {{1, 24}, {2, 3}, {3, 3}};
    int n = sizeof(proc) / sizeof(proc[0]);

    // The code will sort these to: P2(3), P3(3), P1(24)
    findAverageTimes_SJF(proc, n);
    return 0;
}


// --------------------------------------
// Round Robin

#include <stdio.h>

// We need to add "remaining time" to our struct
struct Process {
    int pid;      // Process ID
    int bt;       // Burst Time (Original)
    int wt;       // Waiting Time
    int tat;      // Turnaround Time
    int rem_bt;   // Remaining Burst Time
};

// Main function to calculate and print everything
void findAverageTimes_RR(struct Process proc[], int n, int quantum) {
    float total_wt = 0;
    float total_tat = 0;
    
    // Make a copy of burst times to track remaining time
    for (int i = 0; i < n; i++) {
        proc[i].rem_bt = proc[i].bt;
        proc[i].wt = 0; // Initialize wait time
    }

    int t = 0; // This is the current time "clock"
    
    // Loop until all processes are done
    while (1) {
        
        int all_done = 1; // Flag to check if we're finished

        // Go through every process in a "round"
        for (int i = 0; i < n; i++) {
            
            // Check if this process still has work to do
            if (proc[i].rem_bt > 0) {
                all_done = 0; // We found a process that's not done

                // --- This is the core RR logic ---
                
                // Case 1: Process has MORE time left than the quantum
                if (proc[i].rem_bt > quantum) {
                    // Run it for the full quantum
                    t += quantum;
                    // Subtract the time it just ran
                    proc[i].rem_bt -= quantum;
                } 
                // Case 2: Process has LESS time left (or equal)
                else {
                    // This is its last turn. Run it for whatever's left.
                    t += proc[i].rem_bt;
                    
                    // It's finished! Calculate its wait time
                    // Wait Time = Finish Time - Original Burst Time
                    proc[i].wt = t - proc[i].bt;
                    
                    // Set remaining time to 0 to mark it as done
                    proc[i].rem_bt = 0; 
                }
                // ----------------------------------
            }
        } // End of for loop (one round)

        // If we went through the whole list and 'all_done' is still 1,
        // it means all processes are finished.
        if (all_done == 1) {
            break; // Exit the while(1) loop
        }
    } // End of while loop

    // Now that waiting times are calculated, find turnaround times
    for (int i = 0; i < n; i++) {
        proc[i].tat = proc[i].bt + proc[i].wt;
    }

    // Print the results
    printf("Round Robin Scheduling (Quantum=%d)\n", quantum);
    printf("Processes  Burst Time  Waiting Time  Turnaround Time\n");

    for (int i = 0; i < n; i++) {
        total_wt += proc[i].wt;
        total_tat += proc[i].tat;
        printf("   P%d \t\t %d \t\t %d \t\t %d\n",
               proc[i].pid, proc[i].bt, proc[i].wt, proc[i].tat);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
}

// --- Main execution ---
int main() {
    // {ID, BurstTime}
    struct Process proc[] = {{1, 24}, {2, 3}, {3, 3}};
    int n = sizeof(proc) / sizeof(proc[0]);
    
    // Set the time slice
    int quantum = 4;

    findAverageTimes_RR(proc, n, quantum);
    return 0;
}