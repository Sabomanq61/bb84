# Файл-описание интерфейса Алисы
#
# Формат файла Thrift IDL описан здесь: https://thrift.apache.org/docs/idl

namespace cpp tsu.bb84_emu

///@brief Тип базиса
enum Basis
{
     Rectilinear = 0, ///< Прямолинейный
     Diagonal = 1 ///< Диагональный
}

///@brief Сервис Алисы 
service AliceInterface
{
     ///@brief Отправка квантового состояния
     ///@param count количество квантов в пакете
     void SendQuanta(1: i32 count);
     ///@brief Проверка состояния базисов
     ///@param bases базисы, с которыми сверяется Алиса
     ///@returns совпадают соответствующие базисы или нет
     list<bool> CheckBases(1: list<Basis> bases);
     ///@brief Проверить совпадение контрольные суммы ключа, разбитого на части
     ///@param checksums контрольные суммы частей ключа
     ///@returns количество правильных сумм
     i32 CheckPartitionedKeyChecksum(1: list<i32> checksums);
     ///@brief Подтвердить совпадение просеянного ключа
     void ApproveSiftedKey();
     ///@brief Закончить процесс генерации
     ///@param keySize количество бит ключа, которые надо оставить. Значение меньше нуля сигнализирует о
     /// скомпрометированном канале
     void FinishGeneration(1: i32 keySize);
}
