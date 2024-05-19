//
// Created by hunde on 3/30/2024.
//

#include "Program.h"

#include "ResourceLoader.h"


Program::Program() : m_window(sf::VideoMode(800,900), "Chatroom", sf::Style::Close), m_bSocketIsReady(false),
                     m_type(NetworkType::UNASSIGNED), m_mode(Mode::MENU), m_textBox(sf::Vector2f(700, 70), sf::Vector2f(400, 820)),
                     m_nameBox(sf::Vector2f(400, 70), sf::Vector2f(500, 300)),  m_addressBox(sf::Vector2f(400, 70), sf::Vector2f(500, 400)),
                     m_portBox(sf::Vector2f(400, 70), sf::Vector2f(500, 500))
{

    m_nameLabel.setString("Username");
    m_nameLabel.SetPositionWithCenter(sf::Vector2f(150, 290));
    m_addressLabel.setString("Address");
    m_addressLabel.SetPositionWithCenter(sf::Vector2f(150, 390));
    m_portLabel.setString("Port");
    m_portLabel.SetPositionWithCenter(sf::Vector2f(150, 490));

    m_createRoomButton.SetButtonLabelText("Create Room");
    m_createRoomButton.SetButtonLabelSize(30);
    m_createRoomButton.SetButtonPosition(sf::Vector2f(600, 700));
    std::function serverFunc = [this] {
        m_username = m_nameBox.GetString();
        if(!m_username.empty() && !m_portBox.GetString().isEmpty())
        {
            m_address = m_addressBox.GetString().toAnsiString();
            std::string portStr = m_portBox.GetString().toAnsiString();
            try
            {
                m_port = std::stoi(portStr);
            }catch(std::exception& e)
            {
                return;
            }
            StartServerNetworkThread();
        }
    };
    m_createRoomButton.SetCallback(serverFunc);

    m_joinRoomButton.SetButtonLabelText("Join Room");
    m_joinRoomButton.SetButtonLabelSize(30);
    m_joinRoomButton.SetButtonPosition(sf::Vector2f(200, 700));
    std::function clientFunc = [this] {
        m_username = m_nameBox.GetString();
        if(!m_username.empty() && !m_addressBox.GetString().isEmpty() && !m_portBox.GetString().isEmpty() && !m_networkThread.joinable())
        {
            m_address = m_addressBox.GetString().toAnsiString();
            std::string portStr = m_portBox.GetString().toAnsiString();
            try
            {
                m_port = std::stoi(portStr);
            }catch(std::exception& e)
            {
                return;
            }
            StartClientNetworkThread();
        }
    };
    m_joinRoomButton.SetCallback(clientFunc);

    m_serverSocket = std::make_shared<sf::TcpSocket>();


    //Encryption
    m_encryptionParams.GenerateRandomWithKeySize(m_rng, 2048);
    m_privateKey = CryptoPP::RSA::PrivateKey(m_encryptionParams);
    m_publicKey = CryptoPP::RSA::PublicKey(m_encryptionParams);

}

Program::~Program()
{

    m_serverSocket->disconnect();
    for(auto& client : m_clients)
    {
        client.Socket->disconnect();
    }

    //basically blowing up the thread but since its blocking its necessary
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
    m_updateThread = std::thread(&Program::UpdateNetwork, this);
    while(m_window.isOpen())
    {
        Update();
    }
}

void Program::Update()
{
    m_dt = m_clock.restart().asSeconds();
    m_textBox.ManageCursorBlink(m_dt);
    m_nameBox.ManageCursorBlink(m_dt);
    m_addressBox.ManageCursorBlink(m_dt);
    m_portBox.ManageCursorBlink(m_dt);
    HandleEvents();
    Render();
}


void Program::Render()
{
    m_window.clear(sf::Color(220,220,220));

    if(m_mode == Mode::MENU)
    {
        m_window.draw(m_nameLabel);
        m_window.draw(m_addressLabel);
        m_window.draw(m_portLabel);
        m_window.draw(m_nameBox);
        m_window.draw(m_addressBox);
        m_window.draw(m_portBox);
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
            m_addressBox.ManageTextBox(&m_window, event);
            m_portBox.ManageTextBox(&m_window, event);
            m_window.setMouseCursor(Resources::NormalCursor);
            m_createRoomButton.ManageButton(&m_window, event);
            m_joinRoomButton.ManageButton(&m_window, event);

        }
        else if(m_mode == Mode::CHAT)
        {
            m_textBox.ManageTextBox(&m_window, event);
            m_textContainer.ManageTextContainer(&m_window, event);
        }
        switch(event.type)
        {
            case sf::Event::Closed:
                if(m_type == NetworkType::CLIENT)
                {
                    SendData(m_serverSocket, EncryptData(m_username + " has left the chat", m_serverPublicKey));
                }
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
                    auto encryptedData = EncryptData(m_username + ": " + m_textBox.GetString(), m_serverPublicKey);
                    SendData(m_serverSocket, encryptedData);
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
        sf::sleep(sf::milliseconds(50));
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
        HandleQueuedMessages();
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
    std::cout << "Hosting server on port " << m_port << '\n';
    m_mode = Mode::CHAT;
    while(m_window.isOpen())
    {
        if(m_listener.listen(m_port) != sf::Socket::Done)
        {
            std::cout<<"Error when listening for connection\n";
            return;
        }


        if(m_listener.accept(*m_clients.emplace_back().Socket) == sf::Socket::Done)
        {
            Client& client = m_clients[m_clients.size()-1];
            sf::Packet keyPacket;
            client.Socket->receive(keyPacket);
            std::string key;
            keyPacket >> key;
            client.EncryptionKey = ConvertStringToKey(key);
            client.Ready = true;
            SendData(client.Socket, ConvertKeyToString(m_publicKey));

            m_clientSelector.add(*client.Socket);
            m_bSocketIsReady = true;

        }
    }

}

void Program::CreateClient()
{
    std::cout<<"Attempting to connect to host at address " << m_address << " on port " << m_port << '\n';
    while(m_serverSocket->connect(m_address, m_port, sf::seconds(10)) != sf::Socket::Done)
    {
        std::cout<<"Could not connect to server socket... Retrying...\n";
    }
    std::cout << "Connected to server\n";
    m_mode = Mode::CHAT;
    SendData(m_serverSocket, ConvertKeyToString(m_publicKey));
    sf::Packet keyPacket;
    std::string key;
    m_serverSocket->receive(keyPacket);
    keyPacket >> key;
    m_serverPublicKey = ConvertStringToKey(key);
    SendData(m_serverSocket, EncryptData(m_username + " has entered the chat", m_serverPublicKey));
    m_bSocketIsReady = true;

}

void Program::ServerBroadcast(const std::string& data)
{
    for(auto & client : m_clients)
    {
        if(client.Ready)
        {
            std::string encryptedData = EncryptData(data, client.EncryptionKey);
            SendData(client.Socket, encryptedData);
        }else
        {
            m_messageQueue.emplace_back(client, data);
        }
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

void Program::HandleQueuedMessages()
{
    int i = 0;
    for(auto& queuedMessage : m_messageQueue)
    {
        if(queuedMessage.client.Ready)
        {
            SendData(queuedMessage.client.Socket, EncryptData(queuedMessage.message, queuedMessage.client.EncryptionKey));
            m_messageQueue.erase(m_messageQueue.begin()+i);
        }
        i++;
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
            int i = 0;
            for(const auto& client : m_clients)
            {
                if(client.Socket && m_clientSelector.isReady(*client.Socket))
                {
                    if(client.Socket->receive(packet) == sf::Socket::Disconnected)
                    {
                        m_clients.erase(m_clients.begin() + i);
                        successfulData = false;
                    }
                }
                i++;
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

        return str.empty() ? "" : DecryptData(str, m_privateKey);
    }
    return "";
}



std::string Program::ConvertKeyToString(const CryptoPP::RSA::PublicKey &publicKey)
{
    std::string keyStr;
    CryptoPP::StringSink keySink(keyStr);
    publicKey.Save(keySink);
    return keyStr;
}

CryptoPP::RSA::PublicKey Program::ConvertStringToKey(const std::string &keyStr)
{
    CryptoPP::RSA::PublicKey key;
    CryptoPP::StringSource strSource(keyStr, true);
    key.Load(strSource);
    return key;
}

std::string Program::EncryptData(std::string plainText, CryptoPP::RSA::PublicKey publicKey)
{
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
    std::string cipherText;
    CryptoPP::StringSource ss(plainText, true, new CryptoPP::PK_EncryptorFilter(m_rng, encryptor, new CryptoPP::StringSink(cipherText)));
    return cipherText;
}

std::string Program::DecryptData(std::string cipherText, CryptoPP::RSA::PrivateKey privateKey)
{
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
    std::string recoveredText;
    CryptoPP::StringSource ss(cipherText, true, new CryptoPP::PK_DecryptorFilter(m_rng, decryptor, new CryptoPP::StringSink(recoveredText)));
    return recoveredText;
}
