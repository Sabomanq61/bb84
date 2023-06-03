///@brief Сервис Алисы

#include <memory>
#include <mutex>
#include <bb84_emu/alice/alice.h>
#include <bb84_emu/common/log/logger.h>

using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

namespace tsu
{

namespace bb84_emu
{

Alice::Alice( const std::string &host, uint16_t port )
{
     handler_ = std::shared_ptr< AliceIfaceImpl >( new AliceIfaceImpl( host ) );
     processor_ = std::shared_ptr< TProcessor >( new AliceInterfaceProcessor( handler_ ) );
     serverTransport_ = std::shared_ptr< TServerSocket >( new TServerSocket( port ) );
     transportFactory_ = std::shared_ptr< TTransportFactory >( new TBufferedTransportFactory() );
     protocolFactory_ = std::shared_ptr< TProtocolFactory >( new TBinaryProtocolFactory() );
     server_ = std::unique_ptr<TSimpleServer>( new TSimpleServer( processor_, serverTransport_, transportFactory_, protocolFactory_ ) );
}

Alice::~Alice()
{
     Stop();
}

void Alice::Start()
{
     std::promise<void> barrier;
     std::future<void> barrierFuture = barrier.get_future();

     t_ = std::async(std::launch::async,[this,b = std::move(barrier)]() mutable
     {
         try
         {
              b.set_value();
              LOGMSG( "Запуск сервера [ Ok ]%s" );
              server_->serve();
              LOGMSG( "Остановка сервера [ Ok ]%s" );
         }
         catch (apache::thrift::TException &ex)
         {
               throw;
         }
         catch (std::exception& e)
         {
               throw;
         }
     });

     if(!t_.valid())
     {
          throw std::future_error(std::future_errc::no_state);
     }

     barrierFuture.wait();

     if(std::future_status::ready == t_.wait_for(std::chrono::milliseconds(200)))
     {
          t_.wait();
          throw std::logic_error("Must never been thrown");
     }
}

void Alice::Wait()
{
     if(t_.valid())
     {
          t_.wait();
     }
}

void Alice::Stop()
{
     server_->stop();
     Wait();
}

}

}
