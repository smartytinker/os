/*
 * ============================================================================
 *
 * Filename:  bankers.c
 *
 * Description:  Banker's Algorithm (Safety Algorithm)
 *
 * --- THE BIG IDEA (THE "SCARED BANKER" ANALOGY) ---
 *
 * 1.  This program checks if a system is in a "SAFE STATE".
 * 2.  A safe state means there is *some order* (a "safe sequence")
 * in which the OS (the Banker) can give all processes (the Customers)
 * the resources (money) they need, so everyone finishes.
 *
 * 3.  HOW IT WORKS (THE SIMULATION):
 * - We have `Available` (Banker's cash in vault, e.g., [10, 5, 7]).
 * - We have `Max` (Each customer's credit limit, e.g., P0 needs [7, 5, 3]).
 * - We have `Allocation` (What each customer *already* has, e.g., P0 has [0, 1, 0]).
 *
 * - Step 1: Calculate `Need` (What each customer *still* needs to finish).
 * Need = Max - Allocation. (P0 needs [7, 4, 3]).
 *
 * - Step 2: Start the simulation. Create a copy of `Available` called `Work`.
 * Create a `Finish` checklist (all `false`).
 *
 * - Step 3: Loop. Find a customer `i` who is *not* finished AND
 * whose `Need[i]` can be paid by your `Work` (Need[i][j] <= Work[j] for all j).
 *
 * - Step 4: If you find one:
 * - "Great! `P_i` can finish."
 * - "Pretend" they do, and they *give back* their `Allocation`.
 * - Update `Work = Work + Allocation[i]` (Banker gets richer!)
 * - Mark `P_i` as `Finish = true`.
 * - Add `P_i` to the `safeSequence`.
 * - Repeat the loop.
 *
 * - Step 5: If you go through a whole loop and can't find *anyone*
 * to help, the state is UNSAFE (a deadlock is possible).
 *
 * - Step 6: If all customers get marked `Finish = true`, the
 * state is SAFE. Print the `safeSequence`.
 *
 * This code handles Chit 6 and Chit 14, as the algorithm is
 * inherently multi-resource (the `R` dimension).
 *
 * ============================================================================
 */

#include <stdio.h>

// Set the number of processes and resources
// P = Processes, R = Resource types
#define P 5
#define R 3

// Function to calculate the 'Need' matrix
void calculateNeed(int need[P][R], int max[P][R], int alloc[P][R]) {
    // Need[i][j] = Max[i][j] - Allocation[i][j]
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }
}

// Function to check if the system is in a safe state
void checkSafety(int avail[], int max[][R], int alloc[][R]) {
    
    // --- Step 1: Calculate the 'Need' matrix ---
    int need[P][R];
    calculateNeed(need, max, alloc);

    // --- Step 2: Initialize 'Work' and 'Finish' ---
    
    // 'work' is the banker's "simulation cash"
    // We copy 'available' so we don't mess up the original.
    int work[R];
    for (int i = 0; i < R; i++) {
        work[i] = avail[i];
    }

    // 'finish' is the banker's checklist of "finished customers"
    // 0 = false (not finished), 1 = true (finished)
    int finish[P];
    for (int i = 0; i < P; i++) {
        finish[i] = 0; // Nobody is finished yet
    }

    // 'safeSeq' will store the final safe order
    int safeSeq[P];
    int count = 0; // Counter for finished processes

    // --- Step 3: The Main Safety Loop ---
    // We loop until we've finished all processes (count == P)
    // or we get stuck and can't find anyone to help.
    
    while (count < P) {
        
        int found = 0; // Flag: "Did we find anyone to help this round?"
        
        // This is the banker "scanning the line" for all processes
        for (int i = 0; i < P; i++) {
            
            // Check 1: Is this process *already* finished?
            // If not... (finish[i] == 0)
            if (finish[i] == 0) {
                
                // Check 2: "Can I afford their 'Need'?"
                // We must be able to afford *all* their resource needs.
                int canGrant = 1; // Assume we can, until proven otherwise
                for (int j = 0; j < R; j++) {
                    if (need[i][j] > work[j]) {
                        canGrant = 0; // Nope, need more than I have
                        break;        // Stop checking this process
                    }
                }

                // --- Step 4: If we found a process we can help ---
                if (canGrant == 1) {
                    
                    // "P[i], you can finish! Here are your resources."
                    // "Now, give me back *all* your allocated resources."
                    
                    // Add this process's *allocation* back to our 'work'
                    for (int k = 0; k < R; k++) {
                        work[k] += alloc[i][k];
                    }
                    
                    // Mark this process as finished
                    finish[i] = 1;
                    
                    // Add it to our safe sequence
                    safeSeq[count] = i;
                    count++;
                    
                    // We found someone! Set flag and restart the scan
                    found = 1;
                }
            } // end if(finish[i] == 0)
        } // end for (scan all processes)

        // --- Step 5: Check if we are stuck ---
        // If we scanned all processes and 'found' is still 0...
        if (found == 0) {
            // We are STUCK. This is an UNSAFE STATE.
            printf("System is in an UNSAFE STATE.\n");
            printf("No safe sequence found. Deadlock is possible.\n");
            return; // Exit the function
        }
    } // end while(count < P)

    // --- Step 6: If we get here, everyone finished! ---
    // The loop completed, so 'count' must be equal to 'P'.
    
    printf("System is in a SAFE STATE.\n");
    printf("Safe Sequence is: < ");
    for (int i = 0; i < P; i++) {
        printf("P%d ", safeSeq[i]);
    }
    printf(">\n");
}


// --- Main driver code ---
int main() {
    
    // This is the classic textbook example data
    // P = 5 processes (P0 to P4)
    // R = 3 resource types (A, B, C)

    // Allocation Matrix (What each process *has*)
    int alloc[P][R] = {
        {0, 1, 0}, // P0
        {2, 0, 0}, // P1
        {3, 0, 2}, // P2
        {2, 1, 1}, // P3
        {0, 0, 2}  // P4
    };

    // Max Matrix (What each process *might need*)
    int max[P][R] = {
        {7, 5, 3}, // P0
        {3, 2, 2}, // P1
        {9, 0, 2}, // P2
        {2, 2, 2}, // P3
        {4, 3, 3}  // P4
    };

    // Available Resources (What the bank *has*)
    int avail[R] = {3, 3, 2}; // (3 of A, 3 of B, 2 of C)

    // Run the safety check
    checkSafety(avail, max, alloc);

    return 0;
}