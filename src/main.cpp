#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>

int main() {
    // Gathering User Input
    int numOfChildren = 0;
    int fileIndex = 0;
    while(numOfChildren == 0 || fileIndex == 0) {
        // Get the user input for children
        if(numOfChildren == 0) {
            std::cout << "Number of child processes (1, 2, or 4): ";
            std::cin >> numOfChildren;
        }
        // Validate input
        if(numOfChildren != 1 && numOfChildren != 2 && numOfChildren != 4) {
            numOfChildren = 0;
        }

        // Get the user input for file
        if(fileIndex == 0) {
            std::cout << "Index of file (1, 2, or 3): ";
            std::cin >> fileIndex;
        }
        // Validate input
        if(fileIndex != 1 && fileIndex != 2 && fileIndex != 3) {
            fileIndex = 0;
        }

        // Error case message
        if(numOfChildren == 0 || fileIndex == 0) {
            std::cout << "Invalid input(s). Try again.";
        } 
    }
    int fds[numOfChildren][2];

    // Creating Children Processes
    for(int i = 0; i < numOfChildren; ++i) {
        // Intantiate fork
        pid_t pid = fork();

        // Validate fork
        if(pid == -1) {
            std::cerr << "Fork Failed." << std::endl;
            return 1; // Error Exit
        } else if(pid == 0) {
            // Open file
            std::string fileName = "file" + std::to_string(fileIndex) + ".dat";
            FILE *file = fopen(fileName.c_str(), "r");
            // Validate file
            if(file == nullptr) {
                std::cerr << "Error with file." << std::endl;
                return 1; // Error Exit
            }

            // Get total lines of file
            char line[256];
            int lineCount = 0;
            while (fgets(line, sizeof(line), file) != nullptr) {
                lineCount++;
            }
            // Reset file pointer
            fseek(file, 0, SEEK_SET);

            // Calculate lines to process
            int startLine = (lineCount / numOfChildren) * i + 1;
            int endLine = (i == numOfChildren -1 ) ? lineCount : (lineCount / numOfChildren) * (i + 1);

            // Process assigned lines
            long long sum = 0;
            while (fgets(line, sizeof(line), file) != nullptr) {
                // Add line to sum and increment lineCount
                if(endLine >= startLine) {
                    sum += std::stoi(line);
                }
                startLine++;
            }
            // Close file
            fclose(file);

            // Using pipe, write sum to parent
            int writeData = write(fds[i][1], &sum, sizeof(sum));
            // Validate write
            if(writeData == -1) {
                std::cerr << "Error with write." << std::endl;
                return 1; // Error exit
            }

            // Close fds
            close(fds[i][0]);
            close(fds[i][1]);

            return 0; // Valid Exit
        }
    }
    // Wait
    wait(NULL);

    int total = 0;
    // Parent process sums
    for(int i = 0; i < numOfChildren; i++) {
        read(fds[i][0], &total, sizeof(total));
    }

    // Final output
    std::cout << "Total: " << total << std::endl;
    return 0;
}