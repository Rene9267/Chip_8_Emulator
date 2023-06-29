#include "CHIP8Emulator.h"
#define SDL_MAIN_HANDLED
#include "SDL.h"
#define CLOVE_SUITE_NAME CHIP_Suite
#include "clove-unit.h"

CLOVE_SUITE_SETUP_ONCE()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		{
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		}
	}

	std::cout << std::hex;
}

CLOVE_TEST(INIT_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_IS_TRUE(emulator_init.IsValid());
	CLOVE_INT_EQ((int)0x200, (int)emulator_init.GetPcCounter());
}

#pragma region OPCode 0

CLOVE_TEST(OPCode0_CLS_Test)
{
	chipotto::Emulator emulator_init;
	uint8_t* pixels = nullptr;
	int pitch;

	emulator_init.OpcodeD(0xD005);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode0(0x00E0));

	SDL_LockTexture(emulator_init.GetTexture(), nullptr, reinterpret_cast<void**>(&pixels), &pitch);
	uint8_t* returnal = static_cast<uint8_t*>(calloc(emulator_init.GetTextureHeight() * pitch, sizeof(uint8_t)));
	CLOVE_NOT_NULL(returnal);

	CLOVE_INT_EQ(0, memcmp(returnal, pixels, emulator_init.GetTextureHeight() * pitch * sizeof(uint8_t)));
	free(returnal);
}

CLOVE_TEST(OPCode0_RET_STACKOVERFLOW_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator_init.Opcode0(0x00EE));
}

CLOVE_TEST(OPCode0_RET_TEST)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode1(0x1240);
	emulator_init.Opcode2(0x2300);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode0(0x00EE));
	CLOVE_INT_EQ(0x240, (int)emulator_init.GetPcCounter());
}

#pragma endregion

#pragma region OPCode 1

CLOVE_TEST(OPCode1_JP_Test)
{
	chipotto::Emulator emulator_init;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotIncrementPC, (int)emulator_init.Opcode1(0x1300));
	CLOVE_INT_EQ(0x0300, (int)emulator_init.GetPcCounter());
}

#pragma endregion

#pragma region OPCode 2

CLOVE_TEST(OPCode2_CALL_STACKOVERFLOW_Test)
{
	chipotto::Emulator emulator_init;
	for (size_t i = 0; i < 16; i++)
	{
		emulator_init.Opcode2(0x2000);
	}
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator_init.Opcode2(0x2000));
}

CLOVE_TEST(OPCode2_CALL_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotIncrementPC, (int)emulator_init.Opcode2(0x2100));
	CLOVE_INT_EQ(0x100, (int)emulator_init.GetPcCounter());
	CLOVE_INT_EQ(0x200, (int)emulator_init.GetFromStack(0xFFF));
}

#pragma endregion

#pragma region OPCode 3

CLOVE_TEST(OPCode3_SEVx_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6323);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode3(0x3323));
	CLOVE_INT_EQ(0x202, (int)emulator_init.GetPcCounter());

	emulator_init.Opcode6(0x6423);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode3(0x3425));
	CLOVE_INT_NE(0x204, (int)emulator_init.GetPcCounter());
}
#pragma endregion

#pragma region OPCode 4

CLOVE_TEST(OPCode4_SNEVx_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6325);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode4(0x3323));
	CLOVE_INT_EQ(0x202, (int)emulator_init.GetPcCounter());

	emulator_init.Opcode6(0x6423);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode4(0x3423));
	CLOVE_INT_NE(0x204, (int)emulator_init.GetPcCounter());
}
#pragma endregion

#pragma region OPCode 6

CLOVE_TEST(OPCode6_LDVx_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode6(0x63AC));
	CLOVE_UINT_EQ(0x63AC & 0xFF, emulator_init.GetFromRegister((0x63AC >> 8) & 0xF));
}
#pragma endregion

#pragma region OPCode 7

CLOVE_TEST(OPCode7_ADDVx_Test)
{
	chipotto::Emulator emulator_init;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode7(0x7013));
	CLOVE_INT_EQ(0x13, (int)emulator_init.GetFromRegister(0))
}
#pragma endregion

#pragma region OPCode 8

CLOVE_TEST(OPCode8_LDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6423);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8040));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetFromRegister(4));
}

CLOVE_TEST(OPCode8_ORVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(OPCode8_ANDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6426);
	emulator_init.Opcode6(0x6034);

	CLOVE_INT_NE(0x24, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8042));
	CLOVE_INT_EQ(0x24, (int)emulator_init.GetFromRegister(0));
}

CLOVE_TEST(OPCode8_XORVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6426);
	emulator_init.Opcode6(0x6034);

	CLOVE_INT_NE(0x12, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8043));
	CLOVE_INT_EQ(0x12, (int)emulator_init.GetFromRegister(0));
}

CLOVE_TEST(OPCode8_ADDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x64FA);
	emulator_init.Opcode6(0x6003);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8044));
	CLOVE_INT_EQ(0xFD, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ((int)0, (int)emulator_init.GetFromRegister(0xF));

	emulator_init.Opcode6(0x6406);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8044));
	CLOVE_INT_EQ(0x3, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ(1, (int)emulator_init.GetFromRegister(0xF));

}

CLOVE_TEST(OPCode8_SUBVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(OPCode8_SHRVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(OPCode8_SUBNVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(OPCode8_SHLVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6034);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x800E));
	CLOVE_INT_EQ(0x00, (int)emulator_init.GetFromRegister(0xF));
	CLOVE_INT_EQ(0x68, (int)emulator_init.GetFromRegister(0));

	emulator_init.CleanRegisters();
	emulator_init.Opcode6(0x60F2);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x800E));
	CLOVE_INT_EQ(0x1, (int)emulator_init.GetFromRegister(0xF));
	CLOVE_INT_EQ(0xE4, (int)emulator_init.GetFromRegister(0));
}

#pragma endregion

#pragma region OPCode A

CLOVE_TEST(OPCodeA_LDI_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeA(0xA540));
	CLOVE_INT_EQ(0x540, (int)emulator_init.GetI());
}
#pragma endregion

#pragma region OPCode C
CLOVE_TEST(OPCodeC_RNDVx_Test)
{
	chipotto::Emulator emulator_init;

	std::srand(1);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeC(0xC0FF));
	CLOVE_INT_EQ(41, (int)emulator_init.GetFromRegister(0));
}
#pragma endregion

#pragma region OPCodeF

CLOVE_TEST(OPCodeF_LDVxDT_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF007));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetDelayTimer());
}

CLOVE_TEST(OPCodeF_LDVxK_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::WaitForKeyboard, (int)emulator_init.OpcodeF(0xF00A));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetDelayTimer());
}

CLOVE_TEST(OPCodeF_LDDTVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF015));
	CLOVE_INT_EQ((int)emulator_init.GetDelayTimer(), 0x25);
	CLOVE_INT_EQ(17 + SDL_GetTicks64(), (int)emulator_init.GetDelaTimerTicks());
}

CLOVE_TEST(OPCodeF_LDSTVx_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF018));
}

CLOVE_TEST(OPCodeF_ADDIVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);
	emulator_init.OpcodeA(0xA239);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF01E));
	CLOVE_INT_EQ(0x25E, (int)emulator_init.GetI());
}

CLOVE_TEST(OPCodeF_LDFVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF029));
	CLOVE_INT_EQ(0xB9, (int)emulator_init.GetI());
}

CLOVE_TEST(OPCodeF_LDBVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x60F5);
	emulator_init.OpcodeA(0xA239);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF033));

	CLOVE_INT_EQ((int)(0xF5 / 100), (int)emulator_init.GetFromMemoryMap(emulator_init.GetI()));

	CLOVE_INT_EQ((int)(0xF5 - ((int)emulator_init.GetFromMemoryMap(emulator_init.GetI()) * 100) / 10),
		(int)emulator_init.GetFromMemoryMap(emulator_init.GetI() + 1));

	CLOVE_INT_EQ((int)(0xF5 % 10), (int)emulator_init.GetFromMemoryMap(emulator_init.GetI() + 2));
}

CLOVE_TEST(OPCodeF_LDIVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x60F5);
	emulator_init.Opcode6(0x61BA);
	emulator_init.Opcode6(0x6263);
	emulator_init.OpcodeA(0xA239);
	uint16_t temp_i = emulator_init.GetI();
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF255));

	CLOVE_INT_EQ(0xF5, emulator_init.GetFromMemoryMap(temp_i));
	CLOVE_INT_EQ(0xBA, emulator_init.GetFromMemoryMap(temp_i + 1));
	CLOVE_INT_EQ(0x63, emulator_init.GetFromMemoryMap(temp_i + 2));
}

CLOVE_TEST(OPCodeF_LDVxI_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.OpcodeA(0xA239);
	emulator_init.Opcode6(0x60F5);
	emulator_init.Opcode6(0x61BA);
	emulator_init.Opcode6(0x6263);
	emulator_init.OpcodeF(0xF255);

	emulator_init.CleanRegisters();
	CLOVE_INT_EQ(0, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ(0, (int)emulator_init.GetFromRegister(1));
	CLOVE_INT_EQ(0, (int)emulator_init.GetFromRegister(2));

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF265));

	CLOVE_INT_EQ(0xF5, (int)emulator_init.GetFromRegister(0));
	CLOVE_INT_EQ(0xBA, (int)emulator_init.GetFromRegister(1));
	CLOVE_INT_EQ(0x63, (int)emulator_init.GetFromRegister(2));
}
#pragma endregion

#pragma region OPCode Not Implemented yet

////OpCode 5
CLOVE_TEST(OPCode5_SEVxVy_Test)
{
	CLOVE_FAIL();
}

//OpCode 9
CLOVE_TEST(OPCode9_SNEVxVy_Test)
{
	CLOVE_FAIL();
}

//OpCode B
CLOVE_TEST(OPCodeB_JPV0_Test)
{
	CLOVE_FAIL();
}

#pragma endregion

CLOVE_SUITE_TEARDOWN_ONCE()
{
	SDL_Quit();
}