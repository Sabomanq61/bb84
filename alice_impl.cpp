#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <bb84_emu/common/net/udp_multicast_sender.h>
#include <bb84_emu/common/helpers/helper.h>
#include <bb84_emu/alice/constants.h>
#include <bb84_emu/alice/alice_impl.h>
#include <bb84_emu/common/log/logger.h>
#include <bb84_emu/common/constants.h>
#include <bb84_emu/common/tui/task_table.h>

namespace tsu
{

namespace bb84_emu
{

AliceIfaceImpl::AliceIfaceImpl( const std::string& host )
     : host_( host )
{

}

AliceIfaceImpl::~AliceIfaceImpl()
{

}

void AliceIfaceImpl::SendQuanta( int count )
{
     auto countStr = std::to_string( count );
     auto& taskTable = tui::GetGlobalTaskTable();
     taskTable.StartFrom( 0,
          countStr + " qubits requested\n"
          "Opening connection with Bob"
     );
     std::this_thread::sleep_for( tuiTimeout );

     net::UdpMulticastSender s( udpPort, multicastGroupIp, host_ );
     LOGMSG( "Открытие соединения с Бобом [ Ok ]" );

     std::random_device rd;
     std::mt19937 generator( rd() );
     std::uniform_int_distribution< uint8_t > distribution( 1, 4 );

     taskTable.SetMessage(
          countStr + " qubits requested\n"
          "Generation of qbuts"
     );
     std::this_thread::sleep_for( tuiTimeout );

     states_.resize( count );
     std::generate( states_.begin(), states_.end(), [&]()
     {
          auto state = static_cast< types::QuantumState >( distribution( generator ) );
          LOGMSG( "Генерация квантового состояния (%s) [ Ok ]", helpers::GetDescription( state ).c_str() );
          return state;
     });


     std::vector< uint8_t > msg;
     std::transform( states_.begin(), states_.end(), std::back_inserter( msg ), [ & ]( types::QuantumState state )
     {
          return static_cast< uint8_t >( state );
     });

     taskTable.SetMessage(
          countStr + " qubits requested\n"
          "Sending the qubits to Bob"
     );

     s.Send( msg );

     std::this_thread::sleep_for( tuiTimeout );

     LOGMSG( "Отправка квантового состояния Бобу [ Ok ]" );
     s.CloseSocket();
     LOGMSG( "Закрытие соединения с Бобом [ Ok ]" );

     taskTable.Done( "The qubits are successfully\nsent to Bob" );
}

void AliceIfaceImpl::CheckBases( std::vector< bool >& _return, const std::vector< Basis::type >& bases )
{
     auto& taskTable = tui::GetGlobalTaskTable();
     taskTable.SetMessage( "Sifting the key\nwhile matching bases" );
     std::this_thread::sleep_for( tuiTimeout );

     auto statesIter = states_.begin();

     siftedKeyPart_.resize( 0 );
     for ( auto& basis : bases )
     {
          bool result = basis == Basis::Rectilinear ?
               *statesIter == types::QuantumState::P_0 || *statesIter == types::QuantumState::P_90
               : *statesIter == types::QuantumState::P_45 || *statesIter == types::QuantumState::P_135;

           LOGMSG( "Базис Боба: %s [ Ok ]", helpers::GetDescription( basis ).c_str() );
           LOGMSG( "Базис Алисы под углом %s  [ Ok ]", helpers::ToAngleStr( *statesIter ).c_str() );

          _return.push_back( result );

          if( result )
          {
               LOGMSG( "Базисы совпали [ Ok ]" );
               siftedKeyPart_.push_back( helpers::ToValue( *statesIter ) );
          }
          else
          {
               LOGMSG( "Базисы не совпали [ Ok ]" );
          }
          ++statesIter;
     }

     taskTable.Done( "Sifted successfully" );
     std::this_thread::sleep_for( tuiTimeout );
}

int32_t AliceIfaceImpl::CheckPartitionedKeyChecksum( const std::vector< int32_t >& checksums )
{
     auto& taskTable = tui::GetGlobalTaskTable();
     taskTable.SetMessage( "Checking the key that has\nbeen partitioned into\n" +
          std::to_string( checksums.size() ) + " parts."
     );
     std::this_thread::sleep_for( tuiTimeout );

     int32_t result = 0;
     std::vector< int32_t > aliceChecksums = helpers::GetPartitionedChecksum( siftedKeyPart_, checksums.size() );
     for( size_t i = 0; i < checksums.size(); ++i )
     {
          if( aliceChecksums[ i ] == checksums[ i ] )
          {
               ++result;
          }
     }

     if( result == static_cast< int32_t >( checksums.size() ) )
     {
          taskTable.Done( "Keys match" );
     }
     else
     {
          taskTable.Failed( "Keys don't match" );
     }
     std::this_thread::sleep_for( tuiTimeout );

     return result;
}

void AliceIfaceImpl::ApproveSiftedKey()
{
     key_.insert( key_.end(), siftedKeyPart_.begin(), siftedKeyPart_.end() );
}

void AliceIfaceImpl::FinishGeneration( int keySize )
{
     auto& taskTable = tui::GetGlobalTaskTable();

     if( keySize < 0 )
     {
          LOGMSG( "Канал скомпрометирован. Сброс ключа." );
          std::fill( key_.begin(), key_.end(), 0 );
          key_.resize( 0 );
          taskTable.SetMessage( "Channel compromised. Dropping the key." );
          taskTable.ResetSummary();
          return;
     }

     key_.resize( keySize );

     std::string strQuantumKey { helpers::BytesToHexStr( helpers::GetBytes( key_ ) ) };

     std::string message = "The key of " + std::to_string( keySize ) + " bits\n" +
          "is generated.\n"
          "See it below.";

     taskTable.SetMessage( message );

     const size_t bytesPerLine = 8;
     const size_t byteStrSize = 3;

     size_t lineWidth = byteStrSize * bytesPerLine;

     for( size_t i = lineWidth - 1; i < strQuantumKey.size(); i += lineWidth )
     {
          strQuantumKey.replace( i, 1, 1, '\n' );
     }
     if( strQuantumKey.size() % 2 == 0 )
     {
          strQuantumKey.erase( strQuantumKey.end() - 1 );
     }
     taskTable.SetSummary( strQuantumKey );

     std::this_thread::sleep_for( tuiTimeout );

     LOGMSG( "Ключ сгенерирован [ Ok ]" );
     LOGMSG( "Полученный ключ: [ Ok ]" );
     LOGMSG( "%s [ Ok ]", strQuantumKey.c_str() );
     key_.clear();
}

}

}
