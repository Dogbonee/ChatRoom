//
// Created by hunde on 3/30/2024.
//

#include "Program.h"

#include "ResourceLoader.h"


Program::Program() : m_window(sf::VideoMode(800,900), "Chatroom", sf::Style::Close), m_bSocketIsReady(false),
                     m_type(NetworkType::UNASSIGNED), m_mode(Mode::MENU), m_textBox(sf::Vector2f(700, 70), sf::Vector2f(400, 820)),
                     m_nameBox(sf::Vector2f(400, 70), sf::Vector2f(400, 500))
{

    m_nameLabel.setString("Enter Username");
    m_nameLabel.SetPositionWithCenter(sf::Vector2f(400, 400));

    m_createRoomButton.SetButtonLabelText("Create Room");
    m_createRoomButton.SetButtonLabelSize(30);
    m_createRoomButton.SetButtonPosition(sf::Vector2f(600, 700));
    std::function serverFunc = [this] {
        m_username = m_nameBox.GetString();
        if(!m_username.empty())
        {
            StartServerNetworkThread();
        }
    };
    m_createRoomButton.SetCallback(serverFunc);

    m_joinRoomButton.SetButtonLabelText("Join Room");
    m_joinRoomButton.SetButtonLabelSize(30);
    m_joinRoomButton.SetButtonPosition(sf::Vector2f(200, 700));
    std::function clientFunc = [this] {
        m_username = m_nameBox.GetString();
        if(!m_username.empty())
        {
            StartClientNetworkThread();
        }
    };
    m_joinRoomButton.SetCallback(clientFunc);

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
    m_window.setFramerateLimit(30);
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
        m_window.draw(m_createRoomButton);
        m_window.draw(m_joinRoomButton);
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
            m_window.setMouseCursor(Resources::NormalCursor);
            m_createRoomButton.ManageButton(&m_window, event);
            m_joinRoomButton.ManageButton(&m_window, event);

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
            if(m_mode == Mode::CHAT && !m_textBox.GetString().isEmpty())
            {
                std::thread sendDataThread(&Program::SendData, this, m_textBox.GetString());
                sendDataThread.join();
                m_textContainer.PushText(m_username + ": " + m_textBox.GetString());
                m_textBox.ClearString();
            }
        break;


    }
}

void Program::UpdateNetwork()
{
    while(m_window.isOpen())
    {
        if(m_socket.getLocalPort() == 0)
        {
            std::cout<<"Socket has disconnected\n";
            return;
        }
        auto data = ReceiveData();
        std::cout<<data<<'\n';
        m_textContainer.PushText(data);

    }
}


void Program::StartServerNetworkThread()
{
    std::cout<<"Server selected\n";
    m_type = NetworkType::SERVER;
    m_networkThread = std::thread(&Program::CreateServer, this);
}

void Program::StartClientNetworkThread()
{
    std::cout<<"Client selected\n";
    m_type = NetworkType::CLIENT;
    m_networkThread = std::thread(&Program::CreateClient, this);
}

void Program::CreateServer()
{
    m_mode = Mode::CHAT;
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
    m_mode = Mode::CHAT;
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
