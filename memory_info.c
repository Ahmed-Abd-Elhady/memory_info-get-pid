#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <psapi.h>
#include <stdint.h> // Include for uintptr_t

void get_memory_addresses_by_pid(DWORD pid) {
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T address = 0;

    // Open the process with required access rights
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        printf("Could not open process with PID %lu. Error: %lu\n", pid, GetLastError());
        return;
    }

    // Get the memory information of the process
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        printf("Process ID: %lu\n", pid);
        printf("Working Set Size: %zu bytes\n", pmc.WorkingSetSize);
        printf("Pagefile Usage: %zu bytes\n", pmc.PagefileUsage);
    } else {
        printf("Could not retrieve memory information for PID %lu. Error: %lu\n", pid, GetLastError());
    }

    // Print the memory address range
    printf("Memory address range:\n");
    while (VirtualQueryEx(hProcess, (LPCVOID)address, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT) {
            printf("0x%016llx - 0x%016llx\n", (unsigned long long)(uintptr_t)mbi.BaseAddress, 
                   (unsigned long long)(uintptr_t)mbi.BaseAddress + mbi.RegionSize);
        }
        address += mbi.RegionSize; // Move to the next region
    }

    // Close the process handle
    CloseHandle(hProcess);
}

int main() {
    DWORD pid;  
    char choice;

    do {
        printf("Enter the process ID (PID): ");
        if (scanf("%lu", &pid) != 1) {
            printf("Invalid input. Please enter a valid PID.\n");
            continue;
        }

        get_memory_addresses_by_pid(pid); // Retrieve and print memory information

        // Ask user if they want to retry
        printf("\nDo you want to check another PID? (y/n): ");
        scanf(" %c", &choice); // Read user's choice to continue or not
    } while (choice == 'y' || choice == 'Y'); // Repeat if the user chooses 'y'

    return 0;
}
