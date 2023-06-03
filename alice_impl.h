#ifndef BB84_EMU_ALICE_IMPL_H
#define BB84_EMU_ALICE_IMPL_H

#include <vector>
#include <string>

#include <gen/AliceInterface.h>
#include <bb84_emu/common/types/types.h>
#include <bb84_emu/common/helpers/statistics.h>
#include <bb84_emu/common/constants.h>

namespace tsu
{

namespace bb84_emu
{

class AliceIfaceImpl : virtual public AliceInterfaceIf
{
public:
     ///@brief Конструктор
     ///@param host - IP-адрес хоста
     explicit AliceIfaceImpl( const std::string& host );

     ///@brief Деструктор
     virtual ~AliceIfaceImpl();

     ///@brief SendQuanta - Отправка квантового состояния Бобу
     void SendQuanta( int32_t count ) override;

     ///@brief CheckBases - Проверка состояние базисов
     ///@param _return - вектор, показывающий совпадают соответствующие базисы или нет
     ///@param basis - Базис выбранный Бобом
     void CheckBases( std::vector< bool >& _return, const std::vector< Basis::type >& bases ) override;

     ///@brief CheckPartitionedKeyChecksum - Проверка контрольных сумм разбитого на части ключа
     ///@param checksums контрольные суммы частей ключа
     ///@returns количество правильных сумм
     int32_t CheckPartitionedKeyChecksum( const std::vector< int32_t >& checksums ) override;

     ///@brief ApproveSiftedKey - Подтвердить совпадение просеянного ключа
     void ApproveSiftedKey() override;

     ///@brief FinishGeneration - Закончить процесс генерации
     ///@param keySize размер результирующего ключа
     void FinishGeneration( int keySize ) override;

private:
     std::vector< types::QuantumState > states_;
     std::string host_;
     std::vector< uint8_t > siftedKeyPart_;
     std::vector< uint8_t > key_;
};

}

}

#endif //BB84_EMU_ALICE_IMPL_H
