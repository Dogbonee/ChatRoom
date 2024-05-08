
#include "ResourceLoader.h"
#include "StateMachine.h"

int main(int argc, char* argv[])
{
    StateMachine sm;
    Resources::LoadResources();
    sm.ChangeState(DEFAULT_STATE);
    sm.StartStateMachine();
    return 0;
}
