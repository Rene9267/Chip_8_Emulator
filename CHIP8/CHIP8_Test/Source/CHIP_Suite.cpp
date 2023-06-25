#include "CHIP8Emulator.h"
#define SDL_MAIN_HANDLED
#include "SDL.h"
#define CLOVE_SUITE_NAME CHIP_Suite
#include "clove-unit.h"

#define OUT_OF_STACK 0xFFF

chipotto::Emulator* Emulator = nullptr;

CLOVE_SUITE_SETUP()
{
	Emulator = new chipotto::Emulator();
}

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
	//chipotto::Emulator emulator_init;
	// 
	//Emulator = &EmulatorInit;
	CLOVE_NOT_NULL(Emulator);
	CLOVE_IS_TRUE(/*emulator_init.IsValid()*/ Emulator->IsValid());
	CLOVE_INT_EQ((int)0x200, (int)/*emulator_init.GetPcCounter()*/ Emulator->GetPcCounter());
}

//OpCode 0
#pragma region OPCode 0

CLOVE_TEST(CLS_Test)
{
	chipotto::Emulator emulator_init;
	uint8_t* pixels = nullptr;
	int pitch;

	Emulator->OpcodeD(0xD005);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)Emulator->Opcode0(0x00E0));

	SDL_LockTexture(emulator_init.GetTexture(), nullptr, reinterpret_cast<void**>(&pixels), &pitch);
	uint8_t* returnal = static_cast<uint8_t*>(calloc(emulator_init.GetTextureHeight() * pitch, sizeof(uint8_t)));
	CLOVE_NOT_NULL(returnal);

	CLOVE_INT_EQ(0, memcmp(returnal, pixels, emulator_init.GetTextureHeight() * pitch * sizeof(uint8_t)));
	free(returnal);
}

CLOVE_TEST(RET_STACKOVERFLOW_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator_init.Opcode0(0x00EE));
}

CLOVE_TEST(RET_TEST)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode1(0x1240);
	emulator_init.Opcode2(0x2300);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode0(0x00EE));
	CLOVE_INT_EQ((int)emulator_init.GetPcCounter(), 0x240);
}

#pragma endregion

//OpCode 1
CLOVE_TEST(JP_Test)
{
	//chipotto::Emulator emulator_init;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotIncrementPC, (int)Emulator->Opcode1(0x1300));
	CLOVE_INT_EQ((int)0x0300, (int)/*emulator_init.GetPcCounter()*/Emulator->GetPcCounter());
}

//OpCode 2
#pragma region OPCode 2

CLOVE_TEST(CALL_STACKOVERFLOW_Test)
{
	//chipotto::Emulator EmulatorInit;
	for (size_t i = 0; i < 16; i++)
	{
		Emulator->Opcode2(0x2000);
	}
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)Emulator->Opcode2(0x2000));
}

CLOVE_TEST(CALL_Test)
{
	chipotto::Emulator EmulatorInit;
	uint16_t TempPcCounter = EmulatorInit.GetPcCounter();

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotIncrementPC, (int)EmulatorInit.Opcode2(0x2100));
	CLOVE_INT_EQ((int)0x0100, (int)EmulatorInit.GetPcCounter());
	CLOVE_INT_EQ((int)EmulatorInit.GetFromStack(OUT_OF_STACK), (int)TempPcCounter);
}

#pragma endregion

//OpCode 3
CLOVE_TEST(SEVx_Test)
{
	chipotto::Emulator emulator_init;
	uint16_t temp_pc_counter = emulator_init.GetPcCounter();

	emulator_init.Opcode6(0x6323);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode3(0x3323));
	CLOVE_INT_EQ((int)temp_pc_counter + 2, (int)emulator_init.GetPcCounter());

	emulator_init.Opcode6(0x6423);
	temp_pc_counter = emulator_init.GetPcCounter();
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode3(0x3425));
	CLOVE_INT_NE((int)temp_pc_counter + 2, (int)emulator_init.GetPcCounter());
}

//OpCode 4
CLOVE_TEST(SNEVx_Test)
{
	chipotto::Emulator emulator_init;
	uint16_t temp_pc_counter = emulator_init.GetPcCounter();

	emulator_init.Opcode6(0x6325);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode4(0x3323));
	CLOVE_INT_EQ((int)temp_pc_counter + 2, (int)emulator_init.GetPcCounter());

	emulator_init.Opcode6(0x6423);
	temp_pc_counter = emulator_init.GetPcCounter();
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode4(0x3423));
	CLOVE_INT_NE((int)temp_pc_counter + 2, (int)emulator_init.GetPcCounter());
}

////OpCode 5
CLOVE_TEST(SEVxVy_Test)
{
	/*chipotto::Emulator emulator_init;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::NotImplemented, (int)emulator_init.Opcode5(0x5000));*/
	CLOVE_FAIL();
}

//OpCode 6
CLOVE_TEST(LDVx_Test)
{
	chipotto::Emulator emulator_init;
	uint16_t temp_op = 0x63AC;
	uint8_t temp_register_index = (temp_op >> 8) & 0xF;
	uint8_t temp_register_value = temp_op & 0xFF;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode6(temp_op));
	CLOVE_UINT_EQ(emulator_init.GetFromRegister(temp_register_index), temp_register_value);
}

//Opcode 7
CLOVE_TEST(ADDVx_Test)
{
	chipotto::Emulator emulator_init;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode7(0x7013));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)0x13)
}

//OpCode 8
#pragma region OPCode 8

CLOVE_TEST(LDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6423);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8040));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetFromRegister(4));
}

CLOVE_TEST(ORVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(ANDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6426);
	emulator_init.Opcode6(0x6034);
	uint8_t and_value = 0x24;

	CLOVE_INT_NE((int)emulator_init.GetFromRegister(0), (int)and_value);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8042));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)and_value);
}

CLOVE_TEST(XORVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6426);
	emulator_init.Opcode6(0x6034);
	uint8_t and_value = 0x12;

	CLOVE_INT_NE((int)emulator_init.GetFromRegister(0), (int)and_value);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8043));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)and_value);
}

CLOVE_TEST(ADDVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x64FA);
	emulator_init.Opcode6(0x6003);
	uint8_t add_value = 0xFD;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8044));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)add_value);
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0xF), (int)0);

	emulator_init.Opcode6(0x6406);
	add_value = 0x3;
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x8044));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)add_value);
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0xF), (int)1);

}

CLOVE_TEST(SUBVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(SHRVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(SUBNVxVy_Test)
{
	CLOVE_FAIL();
}

CLOVE_TEST(SHLVxVy_Test)
{
	chipotto::Emulator emulator_init;

	emulator_init.Opcode6(0x6034);
	uint8_t carry_value = 0x00;
	uint8_t result = 0x68;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x800E));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0xF), carry_value);
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), result);

	emulator_init.CleanRegisters();
	emulator_init.Opcode6(0x60F2);
	carry_value = 0x1;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.Opcode8(0x800E));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0xF), carry_value);
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)0xE4);
}

#pragma endregion

//OpCode 9
CLOVE_TEST(SNEVxVy_Test)
{
	CLOVE_FAIL();
}

//OpCode A
CLOVE_TEST(LDI_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeA(0xA540));
	CLOVE_INT_EQ((int)emulator_init.GetI(), (int)0x540);
}

//OpCode B
CLOVE_TEST(JPV0_Test)
{
	CLOVE_FAIL();
}

//OpCode C
CLOVE_TEST(RNDVx_Test)
{
	chipotto::Emulator emulator_init;

	std::srand(1);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeC(0xC0FF));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), 41);
}

#pragma region OPCodeF

CLOVE_TEST(LDVxDT_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF007));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetDelayTimer());
}

CLOVE_TEST(LDVxK_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::WaitForKeyboard, (int)emulator_init.OpcodeF(0xF00A));
	CLOVE_INT_EQ((int)emulator_init.GetFromRegister(0), (int)emulator_init.GetDelayTimer());
}

CLOVE_TEST(LDDTVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF015));
	CLOVE_INT_EQ((int)emulator_init.GetDelayTimer(), 0x25);
	CLOVE_INT_EQ((int)emulator_init.GetDelaTimerTicks(), 17 + SDL_GetTicks64());
}

CLOVE_TEST(LDSTVx_Test)
{
	chipotto::Emulator emulator_init;

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF018));
}

CLOVE_TEST(ADDIVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);
	emulator_init.OpcodeA(0xA239);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF01E));
	CLOVE_INT_EQ((int)emulator_init.GetI(), 0x25E);
}

CLOVE_TEST(LDFVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x6025);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF029));
	CLOVE_INT_EQ((int)emulator_init.GetI(), 0xB9);
}

CLOVE_TEST(LDBVx_Test)
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

CLOVE_TEST(LDIVx_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.Opcode6(0x60F5);
	emulator_init.Opcode6(0x61BA);
	emulator_init.Opcode6(0x6263);
	emulator_init.OpcodeA(0xA239);
	uint16_t temp_i = emulator_init.GetI();
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF255));

	CLOVE_INT_EQ((int)0xF5, emulator_init.GetFromMemoryMap(temp_i));
	CLOVE_INT_EQ((int)0xBA, emulator_init.GetFromMemoryMap(temp_i + 1));
	CLOVE_INT_EQ((int)0x63, emulator_init.GetFromMemoryMap(temp_i + 2));
}

CLOVE_TEST(LDVxI_Test)
{
	chipotto::Emulator emulator_init;
	emulator_init.OpcodeA(0xA239);
	emulator_init.Opcode6(0x60F5);
	emulator_init.Opcode6(0x61BA);
	emulator_init.Opcode6(0x6263);
	uint16_t temp_i = emulator_init.GetI();
	emulator_init.OpcodeF(0xF255);

	emulator_init.CleanRegisters();

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator_init.OpcodeF(0xF265));

	CLOVE_INT_EQ(emulator_init.GetFromRegister(0), (int)0xF5);
	CLOVE_INT_EQ(emulator_init.GetFromRegister(1), (int)0xBA);
	CLOVE_INT_EQ(emulator_init.GetFromRegister(2), (int)0x63);
}
#pragma endregion

//CLOVE_IS_TRUE(Emulator->Tick());
//CLOVE_INT_EQ((int)0x200, (int)Emulator->GetPcCounter());
//
CLOVE_SUITE_TEARDOWN()
{
	delete Emulator;
}
CLOVE_SUITE_TEARDOWN_ONCE()
{
	SDL_Quit();
}