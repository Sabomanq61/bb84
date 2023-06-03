#include <bb84_emu/eve/eve.h>

#include <algorithm>
#include <iostream>

namespace tsu
{

namespace bb84_emu
{

Eve::Eve( uint16_t port, const std::string& group, const std::string& host )
     : sender_( port, group, host )
     , states_( evePackageSize, types::QuantumState::P_0 )
     , bytes_( evePackageSize )
     , senderMustPause_( true )
     , senderMustDie_( false )
{
     std::transform( states_.begin(), states_.end(), bytes_.begin(), []( auto state )
     {
          return static_cast< uint8_t >( state );
     });

     senderThread_ = std::thread( &Eve::SenderLoop, this );
}

Eve::~Eve()
{
     Stop();
     senderMustDie_ = true;
     notifier_.notify_one();

     if( senderThread_.joinable() )
     {
          senderThread_.join();
     }
}

void Eve::Start()
{
     if( IsWorking() )
     {
          return;
     }
     
     senderMustPause_ = false;
     notifier_.notify_one();
}

void Eve::Stop()
{
     if( !IsWorking() )
     {
          return;
     }

     senderMustPause_ = true;
}

void Eve::Toggle()
{
     if( IsWorking() )
     {
          Stop();
     }
     else
     {
          Start();
     }
}

bool Eve::IsWorking() const
{
     return !senderMustPause_;
}

void Eve::SenderLoop()
{
     while( !senderMustDie_ )
     {
          std::unique_lock< std::mutex > lock( cvMutex_ );
          notifier_.wait( lock );
          lock.unlock();

          while( !senderMustPause_ )
          {
               //@todo: раскомментировать, когда bytes_ будет изменяться в другом потоке
               // std::lock_guard< std::mutex > bytesLock( bytesMutex_ );
               sender_.Send( bytes_ );
          }
     }
}

}

}
