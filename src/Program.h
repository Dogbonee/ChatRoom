//
// Created by hunde on 3/30/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

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

struct Client
{
    CryptoPP::RSA::PublicKey EncryptionKey;
    std::shared_ptr<sf::TcpSocket> Socket;
    bool Ready = false;
    Client()
    {
        Socket = std::make_shared<sf::TcpSocket>();
    }
};

struct QueuedMessage
{
    Client client;
    std::string message;
    QueuedMessage(Client client, std::string message)
    {
        this->client = std::move(client);
        this->message = std::move(message);
    }
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


    std::vector<Client> m_clients;
    sf::SocketSelector m_clientSelector;
    std::shared_ptr<sf::TcpSocket> m_serverSocket;
    sf::TcpListener m_listener;
    std::vector<QueuedMessage> m_messageQueue;



    //Encryption
    CryptoPP::InvertibleRSAFunction m_encryptionParams;
    CryptoPP::AutoSeededRandomPool m_rng;
    CryptoPP::RSA::PrivateKey m_privateKey;
    CryptoPP::RSA::PublicKey m_publicKey;
    CryptoPP::RSA::PublicKey m_serverPublicKey;


    CryptoPP::RSA::PublicKey m_remotePublicKey;

    std::string ConvertKeyToString(const CryptoPP::RSA::PublicKey& publicKey);
    CryptoPP::RSA::PublicKey ConvertStringToKey(const std::string& keyStr);
    std::string EncryptData(std::string plainText, CryptoPP::RSA::PublicKey publicKey);
    std::string DecryptData(std::string cipherText, CryptoPP::RSA::PrivateKey privateKey);


    void StartServerNetworkThread();
    void StartClientNetworkThread();

    void CreateServer();
    void CreateClient();
    void ServerBroadcast(const std::string& data);
    void SendData(const std::shared_ptr<sf::TcpSocket>& socket, const std::string& data);
    void HandleQueuedMessages();
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
