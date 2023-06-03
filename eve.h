
///@brief Заголовочный файл для Евы
#pragma once
#ifndef BB84_EVE_EVE_H
#define BB84_EVE_EVE_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include <bb84_emu/common/net/udp_multicast_receiver.h>
#include <bb84_emu/common/net/udp_multicast_sender.h>
#include <bb84_emu/common/types/types.h>

namespace tsu
{

namespace bb84_emu
{

const size_t evePackageSize = 10'000;

///@brief Класс, описывающий поля и методы Евы
class Eve
{
public:
     ///@brief Конструктор
     ///@param port порт исходящих от Евы пакетов
     ///@param group multicast-группа, в которую нужно посылать пакеты
     ///@param host адрес исходящих от Евы пакетов
     Eve( uint16_t port, const std::string& group, const std::string& host );

     virtual ~Eve();

     ///@brief Начать вмешательство в оптический канал
     void Start();

     ///@brief Прекратить вмешиваться в оптический канал
     void Stop();

     ///@brief Переключить режим работы (идет/не идет вмешательство в канал) на противоположный
     void Toggle();

     ///@brief Происходит вмешательство в данный момент или нет
     bool IsWorking() const;

private:
     ///@brief Метод, шлющий пакеты Евы в сеть
     void SenderLoop();

     net::UdpMulticastSender sender_;
     std::vector< types::QuantumState > states_;
     std::mutex bytesMutex_;
     std::vector< uint8_t > bytes_;
     std::thread senderThread_;

     std::condition_variable notifier_;
     std::mutex cvMutex_;

     std::atomic_bool senderMustPause_;
     std::atomic_bool senderMustDie_;
};

} // bb84_emu

} // infotecs

#endif // BB84_EVE_EVE_H

