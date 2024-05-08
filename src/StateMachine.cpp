//
// Created by hunde on 3/30/2024.
//

#include "StateMachine.h"

#include "Program.h"


StateMachine::StateMachine() : m_window(sf::VideoMode(800, 900), "Default Name")
{
    m_states.emplace_back(new Program(this, &m_window));
}

StateMachine::~StateMachine()
{

}

void StateMachine::UpdateCurrentState()
{
    p_currentState->Update();
}

void StateMachine::ChangeState(STATES state)
{
    try
    {
        p_currentState = m_states.at(state);
    }catch (std::exception& e)
    {
        std::cout << "State does not exist\n";
        exit(1);
    }
}

void StateMachine::StartStateMachine()
{
    if(p_currentState == nullptr)
    {
        std::cout << "A state must be set before starting the state machine\n";
        exit(1);
    }
    while(m_window.isOpen())
    {
        UpdateCurrentState();
    }
}


