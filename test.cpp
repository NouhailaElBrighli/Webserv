#include <iostream>
#include <dirent.h>
	#include <iostream>
#include <dirent.h>

int main() {
    const char* dirname = "/Users/nel-brig/Desktop/webserv";
    DIR* dirp = opendir(dirname);

    if (dirp) {
        struct dirent* entry;
        while ((entry = readdir(dirp)) != NULL) {
            std::cout << entry->d_name << std::endl;
        }

        closedir(dirp);
    } else {
        std::cout << "Failed to open directory." << std::endl;
    }
    return 0;
}