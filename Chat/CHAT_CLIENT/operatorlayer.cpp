#include "operatorlayer.h"
#include <chatmodel.h>
#include <client.h>

#include <QQmlContext>

OperatorLayer::OperatorLayer(QQmlApplicationEngine &engine, boost::asio::io_service &service)
  : QObject()
  , chatModel_(nullptr)
  , client_(nullptr)
{

  boost::asio::ip::tcp::resolver resolver(service);
  boost::asio::ip::tcp::endpoint epoint(boost::asio::ip::address::from_string("192.168.2.233"), 1234);
  auto epointI = resolver.resolve(epoint);

  client_ = new Client(service, epointI);
  chatModel_ = new ChatModel(this);

  engine.rootContext()->setContextProperty("cl", client_);
  engine.rootContext()->setContextProperty("chmod", chatModel_);

  client_->setReadHandle([this](ChatMessage &chatMessage) {


    if(chatMessage.getFlagToMessage() == ChatMessage::Flags::MESSAGE)
    {
      chatModel_->add(QString::fromUtf8(chatMessage.getBody(), chatMessage.getBodySize()));
    }
    else      
    {
      QByteArray str = QByteArray(chatMessage.getBody(), chatMessage.getBodySize());
      auto filename = str.mid(0, str.indexOf('|'));
      std::string str1 = str.mid(str.indexOf('|') + 1).toStdString();

      std::ofstream out(filename.toStdString(), std::ios::binary | std::ofstream::app);
      out.write(str1.c_str(), str1.length());
      out.close();
    }
  });

}

OperatorLayer::~OperatorLayer()
{
  if (client_)
  {
    delete client_;
    delete chatModel_;
    client_ = nullptr;
    chatModel_ = nullptr;
  }
}
