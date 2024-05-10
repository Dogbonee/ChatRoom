//
// Created by hunde on 3/30/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include <thread>

#include "Button.h"
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

    sf::IpAddress m_address;
    int m_port;

    //Menu members
    Label m_nameLabel;
    Label m_addressLabel;
    Label m_portLabel;
    Textbox m_nameBox;
    Textbox m_addressBox;
    Textbox m_portBox;
    Button m_createRoomButton;
    Button m_joinRoomButton;


    //Chat members
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


    std::vector<std::shared_ptr<sf::TcpSocket>> m_clientSockets;
    sf::SocketSelector m_clientSelector;
    std::shared_ptr<sf::TcpSocket> m_serverSocket;
    sf::TcpListener m_listener;



    void StartServerNetworkThread();
    void StartClientNetworkThread();

    void CreateServer();
    void CreateClient();
    void ServerBroadcast(const std::string& data);
    void SendData(const std::shared_ptr<sf::TcpSocket>&, const std::string& data);
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
