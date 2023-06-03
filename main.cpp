#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>
#include <bb84_emu/alice/alice.h>
#include <bb84_emu/alice/constants.h>
#include <bb84_emu/common/tui/table.h>
#include <bb84_emu/common/tui/task.h>
#include <bb84_emu/common/tui/task_table.h>

using namespace tsu::bb84_emu;

int main( int argc, char* argv[] )
{
     try
     {
          if( argc != 2 )
          {
               throw std::runtime_error( "Usage: " + std::string( argv[ 0 ] ) + " <host>[:<port>]" );
          }

          size_t addressInputSize = std::strlen( argv[ 1 ] );
          auto addressInputEnd = argv[ 1 ] + addressInputSize;
          uint16_t port = thriftPort;

          // если найдем в переданном адресе порт -- считаем его в переменную порта
          auto separatorIt = std::find( argv[ 1 ], addressInputEnd, ':' );
          if( separatorIt != addressInputEnd )
          {
               std::string portStr( separatorIt + 1, addressInputEnd );
               port = std::stoi( portStr );
          }

          auto& taskTable = tui::GetGlobalTaskTable();
          taskTable.SetName( "BB84 Emu | Alice" );

          taskTable.AddTask( "Generation of qubits" );
          taskTable.AddTask( "Matching the bases" );
          taskTable.AddTask( "Matching the keys" );
          taskTable.SetMessage( "Waiting for\nBob" );

          taskTable.StartDrawing();

          Alice a( { argv[ 1 ], separatorIt }, port );
          a.Start();

          a.Wait();
     }
     catch( std::exception& e )
     {
          std::cerr << e.what() << std::endl;
     }
     return 0;
}
