///@brief Заголовочный файл для Алисы
#pragma once
#ifndef BB84_ALICE_H
#define BB84_ALICE_H

#include <thread>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TSimpleServer.h>
#include <bb84_emu/alice/alice_impl.h>
#include <future>

namespace tsu
{

namespace bb84_emu
{

///@brief Класс, описывающий поля и методы Алисы
class Alice
{

public:
     ///@brief Конструктор
     ///@param host - IP-адрес хоста
     ///@param port - порт хоста
     Alice( const std::string &host, uint16_t port );

     ///@brief Деструктор
     virtual ~Alice();

     ///@brief Start - Запуск Thrift сервера
     void Start();

     ///@brief Wait - Ожидание завершения работы сервера
     void Wait();

     ///@brief Wait - Остановка сервера
     void Stop();

private:
     std::shared_ptr< AliceIfaceImpl > handler_;
     std::shared_ptr< apache::thrift::TProcessor > processor_;
     std::shared_ptr< apache::thrift::server::TServerTransport > serverTransport_;
     std::shared_ptr< apache::thrift::transport::TTransportFactory > transportFactory_;
     std::shared_ptr< apache::thrift::protocol::TProtocolFactory > protocolFactory_;
     std::unique_ptr< apache::thrift::server::TSimpleServer > server_;
     std::future< void > t_;
};

}

}


#endif
