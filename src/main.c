#include <appimage/appimage.h>
#include <string.h>
#include "file.h"
#include "version.h"

int help() {
    puts("Neptune " VERSION_NUMBER "\n");
    puts("Usage: nep [options] <appimage name>\n");
    puts("Options\n");
    puts("install - integrates an AppImage into your desktop and installs the app to your preferred directory, ~/Applications by default");
    puts("remove - de-integrates an AppImage from your desktop, and deletes app.");
    puts("list - lists all AppImages installed");

    return 0;
}

int main(int argc, char * argv[]) {

    if (argc < 2)
        return help();
    else if (strcmp(argv[1], "list") == 0)
        return nep_list();
    else if (argc < 3)
        return help();
    else if (strcmp(argv[1], "install") == 0)
        return nep_install(argv[2]);
    else if (strcmp(argv[1], "remove") == 0)
        return nep_remove(argv[2]);
    
    return help();
}