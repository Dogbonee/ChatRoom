//
// Created by hunde on 3/30/2024.
//

#include "Program.h"

#include "ResourceLoader.h"


Program::Program() : m_window(sf::VideoMode(800,900), "Chatroom", sf::Style::Close), m_bSocketIsReady(false),
                     m_type(NetworkType::UNASSIGNED), m_mode(Mode::MENU), m_textBox(sf::Vector2f(700, 70), sf::Vector2f(400, 820)),
                     m_nameBox(sf::Vector2f(400, 70), sf::Vector2f(400, 500)), MAX_CLIENTS(8)
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
        if(!m_username.empty() && !m_networkThread.joinable())
        {
            StartClientNetworkThread();
        }
    };
    m_joinRoomButton.SetCallback(clientFunc);

    m_serverSocket = std::make_shared<sf::TcpSocket>();

}

Program::~Program()
{
    m_serverSocket->disconnect();
    for(auto& socket : m_clientSockets)
    {
        socket->disconnect();
    }

    //basically blowing up the thread but since its blocking its necessary
    m_networkThread.detach();

    if(m_updateThread.joinable())
    {
        m_updateThread.join();
    }

}

void Program::Run()
{
    m_window.setFramerateLimit(30);
    m_updateThread = std::thread(UpdateNetwork, this);
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
                SendData(m_serverSocket, m_username + " has left the chat");
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
                if(m_type == NetworkType::SERVER)
                {
                    std::string data = m_username + ": " + m_textBox.GetString();
                    ServerBroadcast(data);
                    m_textContainer.PushText(data);
                }else if(m_type == NetworkType::CLIENT)
                {
                    SendData(m_serverSocket, m_username + ": " + m_textBox.GetString());
                }
                m_textBox.ClearString();
            }
        break;


    }
}

void Program::UpdateNetwork()
{
    while(m_window.isOpen())
    {
        if(!m_bSocketIsReady)continue;

        if(m_type == NetworkType::CLIENT)
        {
            if(m_serverSocket->getLocalPort() == 0)
            {
                m_textContainer.PushText("You have been disconnected from the server");
                m_bSocketIsReady = false;
                continue;
            }
        }

        auto data = ReceiveData();
        if(!data.empty())
        {
            if(m_type == NetworkType::SERVER)
            {
                ServerBroadcast(data);
            }

            m_textContainer.PushText(data);
        }

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
    while(m_window.isOpen())
    {
        if(m_listener.listen(60000) != sf::Socket::Done)
        {
            std::cout<<"Error when listening for connection\n";
            return;
        }

        if(m_listener.accept(*m_clientSockets.emplace_back(std::make_unique<sf::TcpSocket>())) == sf::Socket::Done)
        {
            std::cout << "Connected to client\n";
            m_clientSelector.add(*m_clientSockets[m_clientSockets.size()-1]);
            m_bSocketIsReady = true;
        }
    }

}

void Program::CreateClient()
{

    while(m_serverSocket->connect("127.0.0.1", 60000, sf::seconds(10)) != sf::Socket::Done)
    {
        std::cout<<"Could not connect to server socket... Retrying...\n";
    }
    std::cout << "Connected to server\n";
    m_mode = Mode::CHAT;
    SendData(m_serverSocket, m_username + " has entered the chat");
    m_bSocketIsReady = true;

}

void Program::ServerBroadcast(const std::string& data)
{
    for(auto & socket : m_clientSockets)
    {
        SendData(socket, data);
    }
}

void Program::SendData(const std::shared_ptr<sf::TcpSocket>& socket, const std::string& data)
{
    sf::Packet packet;
    packet << data;

    if(socket->getRemotePort() != 0 && socket->send(packet) != sf::Socket::Done)
    {
        std::cout<<"Error sending packet\n";
    }
}

std::string Program::ReceiveData()
{
    sf::Packet packet;
    bool successfulData = true;
    if(m_type == NetworkType::SERVER)
    {
        if(m_clientSelector.wait())
        {
            for(const auto& socket : m_clientSockets)
            {
                if(socket && m_clientSelector.isReady(*socket))
                {
                    if(socket->receive(packet) == sf::Socket::Disconnected)
                    {
                        m_clientSockets.erase(std::find(m_clientSockets.begin(),m_clientSockets.end(), socket));
                        successfulData = false;
                    }
                }
            }
        }
    }
    else if (m_type == NetworkType::CLIENT)
    {
        sf::Socket::Status status = m_serverSocket->receive(packet);

        if(m_serverSocket->getRemotePort() == 0)
        {
            std::cout<<"Disconnected\n";
            return "";
        }

        if(status != sf::Socket::Done)
        {
            std::cout<<"Error when receiving data\n";
            m_serverSocket->disconnect();
            successfulData = false;
        }
    }
    if(successfulData)
    {
        std::string str;
        packet >> str;

        return str;
    }
    return "";
}
