
#include "Program.h"
#include "ResourceLoader.h"


int main(int argc, char* argv[])
{

    Resources::LoadResources();
    Program program;
    program.Run();
    return 0;
}
