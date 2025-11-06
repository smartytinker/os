//Server 

/*
 * ============================================================================
 *
 * Filename:  shm_server.c
 *
 * Description:  THE SERVER - Creates and writes to shared memory.
 *
 * --- THE BIG IDEA (THE "PUBLIC WHITEBOARD" ANALOGY) ---
 *
 * 1.  WHAT'S THE PROBLEM?
 * By default, two programs (processes) are like two separate people
 * with their own private notepads (their own memory). One person
 * cannot see what the other is writing.
 *
 * 2.  WHAT'S THE SOLUTION?
 * We ask the "Store Manager" (the Operating System) to set up a
 * giant "Public Whiteboard" in the breakroom.
 *
 * 3.  THIS PROGRAM (THE SERVER):
 * - Asks the Manager (OS) to "please create a new whiteboard".
 * - Gets a key (the `key_t`) and an ID (`shmid`) for the whiteboard.
 * - Gets a "magic pen" (`char *data`) that writes directly onto the board.
 * - Writes "Hello!" on the board.
 * - Waits for the user to press Enter (to give the client time to read).
 * - Cleans up by telling the manager to "please erase and remove
 * the whiteboard" (`shmctl` with `IPC_RMID`).
 *
 * 4.  THE SYSTEM V FUNCTIONS (THE MANAGER'S COMMANDS):
 * - `ftok()`:     "Manager, I need a public name (key) for my whiteboard."
 * - `shmget()`:   "Manager, *get* me a whiteboard. If it doesn't exist,
 * *create* it (`IPC_CREAT`)."
 * - `shmat()`:    "Manager, *attach* this whiteboard to my desk so I can
 * get the 'pen' (a C pointer) to write on it."
 * - `shmdt()`:    "Manager, I'm *detaching* from the board. I'm done."
 * - `shmctl()`:   "Manager, I'm the owner. Please *control* (and remove)
 * this board (`IPC_RMID`)."
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>     // For exit()
#include <string.h>     // For strcpy()
#include <sys/ipc.h>    // For IPC functions (ftok)
#include <sys/shm.h>    // For Shared Memory functions

#define SHM_SIZE 1024   // Make the whiteboard 1KB in size

int main() {
    
    key_t key;
    int shmid;
    char *shm_data;

    // --- 1. Generate a unique key ---
    // We need an *existing* file to generate a unique key.
    // The file "shmfile" is just a dummy file. You must create it first.
    // In your terminal, run: touch shmfile
    // '65' is just an arbitrary project ID.
    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok"); // Print a system error if ftok fails
        exit(1);        // Exit with an error
    }
    printf("Server: Key generated: %d\n", key);

    // --- 2. Create the shared memory segment (the whiteboard) ---
    // shmget() = "get me shared memory"
    // `key`: The public name of the whiteboard.
    // `SHM_SIZE`: How big it should be.
    // `0666`: Read/Write permissions for everyone.
    // `IPC_CREAT`: "If this whiteboard doesn't exist, CREATE it."
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    printf("Server: Shared memory segment created with ID: %d\n", shmid);


    // --- 3. Attach to the shared memory (get the "pen") ---
    // shmat() = "attach to me"
    // `shmid`: The internal ID of the board we just made.
    // `(void*)0`: Let the OS pick the best address in our memory.
    // `0`: Standard flags.
    // This returns a C pointer to the *actual* shared memory.
    shm_data = (char*) shmat(shmid, (void*)0, 0);
    if (shm_data == (char*)(-1)) {
        perror("shmat");
        exit(1);
    }
    printf("Server: Shared memory attached at address: %p\n", shm_data);


    // --- 4. Write data to the shared memory ---
    // This is the "magic" part. We're just using a normal C
    // pointer, but this memory is *also* visible to other processes.
    const char *msg = "Hello from the server! This is shared memory.";
    strcpy(shm_data, msg);
    printf("Server: Wrote data: '%s'\n", shm_data);


    // --- 5. Wait for the client to read the data ---
    // This is a super simple way to "pause" the server.
    // It keeps the shared memory alive so the client has time
    // to run and read the message.
    printf("\nServer is waiting. Run the client in another terminal...\n");
    printf("Press Enter to detach and destroy the shared memory...\n");
    getchar(); // Wait for the user to press Enter


    // --- 6. Detach from shared memory (put the "pen" down) ---
    if (shmdt(shm_data) == -1) {
        perror("shmdt");
        exit(1);
    }
    printf("Server: Shared memory detached.\n");


    // --- 7. Destroy the shared memory (tell the manager to clean up) ---
    // `IPC_RMID`: Mark this segment to be removed.
    // The OS will *actually* remove it once all processes
    // (including the client) have detached from it.
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }
    printf("Server: Shared memory destroyed.\n");

    return 0;
}


// ----------------------------------------------
// Client

/*
 * ============================================================================
 *
 * Filename:  shm_client.c
 *
 * Description:  THE CLIENT - Finds and reads from shared memory.
 *
 * --- THE BIG IDEA (THE "PUBLIC WHITEBOARD" ANALOGY) ---
 *
 * 1.  This program (THE CLIENT) is the *second* person. It
 * wants to read the message from the server.
 *
 * 2.  IT'S ROLE:
 * - Asks the Manager (OS), "Hey, do you have a whiteboard with the
 * public name (key) that the server and I agreed on?"
 * - Gets the *same* ID (`shmid`) for that board.
 * - Gets its *own* "magic pen" (`char *data`) that points to the
 * *exact same* board.
 * - Reads the "Hello!" message that the server already wrote.
 * - Puts its "pen" down and detaches (`shmdt`).
 *
 * 3.  NOTICE: The client *does not* create or destroy the board. It only
 * finds it, reads it, and detaches. The server is in charge
 * of setup and cleanup.
 *
 * 4.  THE SYSTEM V FUNCTIONS (THE MANAGER'S COMMANDS):
 * - `ftok()`:     "Manager, I need the public name (key) for the board."
 * (Must use the *same* file and ID as the server!)
 * - `shmget()`:   "Manager, *get* me the whiteboard with this key."
 * (NOTICE: No `IPC_CREAT`! It fails if the board
 * doesn't already exist. This is how we find it.)
 * - `shmat()`:    "Manager, *attach* this board so I can read it."
 * - `shmdt()`:    "Manager, I'm *detaching*. I'm done reading."
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>     // For exit()
#include <sys/ipc.h>    // For IPC functions (ftok)
#include <sys/shm.h>    // For Shared Memory functions

#define SHM_SIZE 1024   // 1KB (must be same as server)

int main() {
    
    key_t key;
    int shmid;
    char *shm_data;

    // --- 1. Generate the *exact same* key as the server ---
    // This is the critical part. We use the same
    // file ("shmfile") and project ID (65) to find the
    // server's "public name" for the whiteboard.
    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    printf("Client: Key generated: %d\n", key);

    // --- 2. Get the shared memory segment (find the whiteboard) ---
    // Notice: NO IPC_CREAT!
    // We are *finding* an existing segment, not creating one.
    // If the server isn't running, this `shmget` will fail.
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        printf("Client: Error! Is the server running?\n");
        exit(1);
    }
    printf("Client: Shared memory segment found with ID: %d\n", shmid);

    // --- 3. Attach to the shared memory (get the "pen") ---
    // Same as the server. This gives us a pointer to that
    // *same* piece of memory.
    shm_data = (char*) shmat(shmid, (void*)0, 0);
    if (shm_data == (char*)(-1)) {
        perror("shmat");
        exit(1);
    }
    printf("Client: Shared memory attached at address: %p\n", shm_data);


    // --- 4. Read data from the shared memory ---
    // Just like reading a normal C string. Because the server
    // already wrote to this memory, we will see its message.
    printf("\nClient: *** Data read from memory: ***\n");
    printf(">>> '%s' <<<\n\n", shm_data);


    // --- 5. Detach from shared memory (put the "pen" down) ---
    if (shmdt(shm_data) == -1) {
        perror("shmdt");
        exit(1);
    }
    printf("Client: Shared memory detached.\n");

    // Note: The client does NOT destroy the memory segment.
    // That is the server's job.

    return 0;
}

/*

You need two separate terminal windows for this.

Terminal 1:
Bash

# 1. Create the dummy file for ftok (you only need to do this once)
touch shmfile

# 2. Compile the server
gcc shm_server.c -o server

# 3. Run the server
./server

At this point, Terminal 1 will run, write the data, and then stop, saying: Server is waiting. Run the client in another terminal... Press Enter to detach and destroy the shared memory...

Do not press Enter yet!

Terminal 2:
Bash

# 1. Compile the client
gcc shm_client.c -o client

# 2. Run the client
./client

When you run this, Terminal 2 will immediately find the memory and print: Client: *** Data read from memory: *** >>> 'Hello from the server! This is shared memory.' <<< Client: Shared memory detached.

*/