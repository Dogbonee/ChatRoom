//
// Created by hunde on 3/30/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

#include "Label.h"
#include "Textbox.h"
#include "TextContainer.h"


enum class NetworkType
{
    UNASSIGNED = 0,
    SERVER,
    CLIENT
};

enum class Mode
{
    MENU = 0,
    CHAT
};

class Program{

    Mode m_mode;
    NetworkType m_type;
    std::string m_username;

    Label m_nameLabel;
    Textbox m_nameBox;

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
