//Optimal

#include <stdio.h>

int main()
{
    int n = 10;
    int pages[10] = {7,0,1,2,0,3,0,4,2,3};
    int frame_count = 3;
    int frame[3];

    // initialize frames to empty marker -1
    for (int i = 0; i < frame_count; i++) {
        frame[i] = -1;
    }

    int pageFaults = 0;

    for (int i = 0; i < n; i++)
    {
        int currpage = pages[i];
        int found = 0;

        // check if page is already in a frame (hit)
        for (int j = 0; j < frame_count; j++)
        {
            if (frame[j] == currpage)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            pageFaults++;

            // look for empty spot
            int emptyspot = -1;
            for (int j = 0; j < frame_count; j++)
            {
                if (frame[j] == -1)
                {
                    emptyspot = j;
                    break;
                }
            }

            if (emptyspot != -1)
            {
                // place in empty frame
                frame[emptyspot] = currpage;
            }
            else
            {
                // choose page to replace: the one used farthest in future (or not used)
                int farthest = -1;
                int indextoreplace = -1;

                for (int j = 0; j < frame_count; j++)
                {
                    int nextuse = -1;
                    for (int k = i + 1; k < n; k++)
                    {
                        if (frame[j] == pages[k])
                        {
                            nextuse = k;
                            break;
                        }
                    }

                    // if this frame's page is never used again, replace it immediately
                    if (nextuse == -1)
                    {
                        indextoreplace = j;
                        break;
                    }

                    // otherwise pick the page with farthest next use
                    if (nextuse > farthest)
                    {
                        farthest = nextuse;
                        indextoreplace = j;
                    }
                }

                if (indextoreplace == -1)
                    indextoreplace = 0; // fallback (shouldn't normally happen)

                frame[indextoreplace] = currpage;
            }
        }

        // Show frame contents after each step
        printf("\nAfter page %d: ", currpage);
        for (int j = 0; j < frame_count; j++)
        {
            if (frame[j] == -1)
                printf("- ");
            else
                printf("%d ", frame[j]);
        }

        if (found)
            printf(" (No Fault)");
        else
            printf(" (Page Fault)");
    }

    printf("\n\nTotal Page Faults = %d\n", pageFaults);
    return 0;
}

//--------------------------------------------------------

// LRU 


#include <stdio.h>

  

int main()

{

int n = 10;

int pages[10] = {7,0,1,2,0,3,0,4,2,3}; // reference string

int framesCount = 3; // number of frames (>=3)

int frame[3];

int lastUsed[3];

  

// initialize frames and lastUsed to -1

for (int i = 0; i < framesCount; i++) {

frame[i] = -1;

lastUsed[i] = -1;

}

  

int pageFaults = 0;

  

// header

printf("%-12s%-20s%s\n", "Page Ref", "Frames", "Fault");

printf("------------------------------------------------\n");

  

for (int i = 0; i < n; i++)

{

int curr = pages[i];

int hit = 0;

  

// check for hit and update lastUsed

for (int j = 0; j < framesCount; j++)

{

if (frame[j] == curr)

{

hit = 1;

lastUsed[j] = i; // update timestamp

break;

}

}

  

if (!hit)

{

pageFaults++;

  

// find empty frame first

int emptyIndex = -1;

for (int j = 0; j < framesCount; j++)

{

if (frame[j] == -1) { emptyIndex = j; break; }

}

  

if (emptyIndex != -1)

{

// put page into empty frame

frame[emptyIndex] = curr;

lastUsed[emptyIndex] = i;

}

else

{

// find LRU frame (smallest lastUsed)

int lruIndex = 0;

int minTime = lastUsed[0];

for (int j = 1; j < framesCount; j++)

{

if (lastUsed[j] < minTime) {

minTime = lastUsed[j];

lruIndex = j;

}

}

// replace LRU

frame[lruIndex] = curr;

lastUsed[lruIndex] = i;

}

}

  

// print this step

printf("%-12d[", curr);

for (int f = 0; f < framesCount; f++) {

if (frame[f] == -1) printf(" -");

else printf(" %d", frame[f]);

if (f < framesCount - 1) printf(" ");

}

printf(" ] ");

  

if (hit) printf("No\n"); else printf("Yes\n");

}

  

printf("------------------------------------------------\n");

printf("Total Page Faults = %d\n", pageFaults);

return 0;

}


// -----------------------------------------------------
// FCFS

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
    
    // Waiting time for the first process is always 0
    proc[0].wt = 0;

    // Calculate waiting time for the rest of the processes
    for (int i = 1; i < n; i++) {
        // The next process's wait time is the
        // burst time + wait time of the process *before* it.
        // (This is the "finish time" of the previous process)
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

    // First, calculate wait times
    findWaitingTimes_FCFS(proc, n);

    // Second, calculate turnaround times
    findTurnaroundTimes_FCFS(proc, n);

    // Now, print the results and calculate totals
    printf("FCFS Scheduling\n");
    printf("Processes  Burst Time  Waiting Time  Turnaround Time\n");

    for (int i = 0; i < n; i++) {
        total_wt += proc[i].wt;
        total_tat += proc[i].tat;
        printf("   %d\t\t%d\t\t%d\t\t%d\n",
               proc[i].pid, proc[i].bt, proc[i].wt, proc[i].tat);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
}

// --- Main execution ---
int main() {
    // Let's create 3 processes
    // proc[] = { {ID, BurstTime}, ... }
    struct Process proc[] = {{1, 24}, {2, 3}, {3, 3}};
    int n = sizeof(proc) / sizeof(proc[0]); // This just gets the array size (3)

    findAverageTimes_FCFS(proc, n);
    return 0;
}