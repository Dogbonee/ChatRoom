//
// Created by hunde on 3/30/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

#include "Textbox.h"
#include "TextContainer.h"


enum NetworkType
{
    UNASSIGNED = 0,
    SERVER,
    CLIENT
};


class Program{

    NetworkType m_type;

    Textbox m_textBox;
    TextContainer m_textContainer;

    sf::Clock m_clock;
    float m_dt{};

    sf::RenderWindow m_window;

    void Update();
    void Render();
    void HandleEvents();
    void HandleKeyboardInput(sf::Keyboard::Key key);
    void UpdateNetwork();

    sf::TcpSocket m_socket;
    sf::TcpListener m_listener;

    void CreateServer();
    void CreateClient();
    void SendData(std::string data);
    std::string ReceiveData();
    bool m_bSocketIsReady;
    std::thread m_networkThread;
    std::thread m_updateThread;

public:
    Program();
    ~Program();
    void Run();

};



#endif //PROGRAM_H
