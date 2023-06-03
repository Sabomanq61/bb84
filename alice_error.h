

///@brief Подкласс для всех исключений подсистемы alice
#pragma once
#ifndef BB84_EMU_ALICE_ERROR_H
#define BB84_EMU_ALICE_ERROR_H

#include <bb84_emu/common/error.h>

namespace tsu
{

namespace bb84_emu
{

///@brief Исключение, выбрасываемое в подсистеме alice
struct AliceError : Error
{
     using Error::Error;
};

}

}

#endif //BB84_EMU_ALICE_ERROR_H
