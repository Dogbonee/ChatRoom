//
// Created by hunde on 3/30/2024.
//

#include "Program.h"



Program::Program() : m_window(sf::VideoMode(800,900), "Chatroom", sf::Style::Close), m_bSocketIsReady(false),
m_type(NetworkType::UNASSIGNED), m_mode(Mode::MENU), m_textBox(sf::Vector2f(700, 70), sf::Vector2f(400, 820)),
m_nameBox(sf::Vector2f(400, 70), sf::Vector2f(400, 500))
{

    m_nameLabel.setString("Enter Username");
    m_nameLabel.SetPositionWithCenter(sf::Vector2f(400, 400));
}

Program::~Program()
{
    m_socket.disconnect();
    if(m_networkThread.joinable())
    {
        m_networkThread.join();
    }
    if(m_updateThread.joinable())
    {
        m_updateThread.join();
    }
}

void Program::Run()
{
    while(m_window.isOpen())
    {
        Update();
    }
};

void Program::Update()
{
    m_dt = m_clock.restart().asSeconds();
    m_textBox.ManageCursorBlink(m_dt);
    m_nameBox.ManageCursorBlink(m_dt);

    HandleEvents();
    Render();
}


void Program::Render()
{
    m_window.clear(sf::Color(220,220,220));

    if(m_mode == Mode::MENU)
    {
        m_window.draw(m_nameLabel);
        m_window.draw(m_nameBox);
    }

    if(m_mode == Mode::CHAT)
    {
        m_window.draw(m_textBox);
        m_window.draw(m_textContainer);
    }
    m_window.display();
}

void Program::HandleEvents()
{
    for(sf::Event event{}; m_window.pollEvent(event);)
    {
        if(m_mode == Mode::MENU)
        {
            m_nameBox.ManageTextBox(&m_window, event);
        }
        else if(m_mode == Mode::CHAT)
        {
            m_textBox.ManageTextBox(&m_window, event);
        }
        switch(event.type)
        {
            case sf::Event::Closed:
                m_window.close();
            break;
            case sf::Event::KeyPressed:
                HandleKeyboardInput(event.key.code);
            break;

        }
    }
}

void Program::HandleKeyboardInput(sf::Keyboard::Key key)
{
    switch(key)
    {
        case sf::Keyboard::Escape:
            m_window.close();
        break;
        case sf::Keyboard::Enter:
            if(m_mode == Mode::MENU && !m_nameBox.GetString().isEmpty())
            {
                m_username = m_nameBox.GetString();
                m_mode = Mode::CHAT;
            }
            else if(m_mode == Mode::CHAT && !m_textBox.GetString().isEmpty())
            {
                std::thread sendDataThread(&Program::SendData, this, m_textBox.GetString());
                sendDataThread.join();
                m_textContainer.PushText(m_username + ": " + m_textBox.GetString());
                m_textBox.ClearString();
            }
        break;
        case sf::Keyboard::S:
            if(m_type == NetworkType::UNASSIGNED)
            {
                std::cout<<"Server selected\n";
                m_type = NetworkType::SERVER;
                m_networkThread = std::thread(&Program::CreateServer, this);
            }
        break;
            case sf::Keyboard::C:
            if(m_type == NetworkType::UNASSIGNED)
            {
                std::cout<<"Client selected\n";
                m_type = NetworkType::CLIENT;
                m_networkThread = std::thread(&Program::CreateClient, this);
            }
            break;

    }
}

void Program::UpdateNetwork()
{
    while(m_window.isOpen())
    {
        auto data = ReceiveData();
        std::cout<<data<<'\n';
        m_textContainer.PushText(data);
    }
}


void Program::CreateServer()
{
    if(m_listener.listen(60000) != sf::Socket::Done)
    {
        std::cout<<"Error when listening for connection\n";
        return;
    }
    if(m_listener.accept(m_socket) == sf::Socket::Done)
    {
        std::cout << "Connected to client\n";
        m_bSocketIsReady = true;
        UpdateNetwork();
    }


}

void Program::CreateClient()
{
    while(m_socket.connect("127.0.0.1", 60000, sf::seconds(10)) != sf::Socket::Done)
    {
        std::cout<<"Could not connect to server socket... Retrying...\n";
    }
    std::cout << "Connected to server\n";
    m_bSocketIsReady = true;
    UpdateNetwork();


}

void Program::SendData(std::string data)
{
    sf::Packet packet;
    packet << m_username + ": " + data;
    if(m_socket.send(packet) != sf::Socket::Done)
    {
        std::cout<<"Error sending packet\n";
    }
}

std::string Program::ReceiveData()
{
    sf::Packet packet;
    if(m_socket.receive(packet) != sf::Socket::Done)
    {
        std::cout<<"Error when receiving data\n";
        m_socket.disconnect();
    }
    std::string str;
    packet >> str;

    return str;
}
