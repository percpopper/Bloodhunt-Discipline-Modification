#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

struct FNameEntryHandle {
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 65535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntryHeader
{
	uint16_t bIsWide : 1;
	static constexpr uint32_t ProbeHashBits = 5;
	uint16_t LowercaseProbeHash : ProbeHashBits;
	uint16_t Len : 10;
};

struct FNameEntry {
	FNameEntryHeader Header;

	union
	{
		char AnsiName[1024];
		wchar_t	WideName[1024];
	};

	std::string String();

	inline int32_t GetId() const
	{
		return *(uint16_t*)&Header;
	}
};

struct FNameEntryAllocator
{
	unsigned char frwLock[0x8];
	int32_t CurrentBlock;
	int32_t CurrentByteCursor;
	uint8_t* Blocks[8192];

	inline int32_t NumBlocks() const
	{
		return CurrentBlock + 1;
	}
	inline FNameEntry* GetById(int32_t key) const
	{
		int block = key >> 16;
		int offset = (uint16_t)key;

		if (!IsValidIndex(key, block, offset))
			return reinterpret_cast<FNameEntry*>(Blocks[0] + 0); // "None"

		return reinterpret_cast<FNameEntry*>(Blocks[block] + ((uint64_t)offset * 0x02));
	}
	inline bool IsValidIndex(int32_t key) const
	{
		uint32_t block = key >> 16;
		uint16_t offset = key;
		return IsValidIndex(key, block, offset);
	}
	inline bool IsValidIndex(int32_t key, uint32_t block, uint16_t offset) const
	{
		return (key >= 0 && block < NumBlocks() && offset * 0x02 < 0x1FFFE);
	}
};

struct FNamePool
{
	FNameEntryAllocator Allocator;
	uint32_t AnsiCount;
	uint32_t WideCount;
};

struct FName {
	uint32_t Index;
	uint32_t Number;

	FName() :
		Index(0),
		Number(0)
	{ }

	FName(int32_t i) :
		Index(i),
		Number(0)
	{ }
	std::string GetName();
};

struct UObject {
	void** VFTable;
	uint32_t ObjectFlags;
	uint32_t InternalIndex;
	struct UClass* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	std::string GetName();
	std::string GetFullName();
};

// Class CoreUObject.Field
struct UField : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
};

// Class CoreUObject.Struct
struct UStruct : UField {
	char pad_30[0x10]; // 0x30(0x10)
	UStruct* SuperStruct; // 0x40(0x8)
	char UnknownData_48[0x68]; // 0x48(0x80)
};

// Class CoreUObject.Function
struct UFunction : UStruct {
	char pad_B0[0x30]; // 0xb0(0x30)
};

// Class CoreUObject.Class
struct UClass : UStruct {
	char UnknownData_B0[0x180]; // 0xb0(0x180)
};

struct TUObjectArray {
	BYTE** Objects;
	BYTE* PreAllocatedObjects;
	uint32_t MaxElements;
	uint32_t NumElements;
	uint32_t MaxChunks;
	uint32_t NumChunks;
	UObject* GetObjectPtr(uint32_t id) const;
	UObject* FindObject(const char* name) const;
};

template<class T>
struct TArray
{
public:
	inline T& operator[](int i)
	{
		return Data[i];
	};
private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FVector {
	float X;
	float Y;
	float Z;
};

struct APawn  {

};

enum class ETigerDisciplinesEnum {
	TE_DisciplineNone = 0,
	TE_DisciplineTeleport = 1,
	TE_DisciplineStun = 2,
	TE_DisciplineFOTT = 3,
	TE_DisciplineJump = 4,
	TE_DisciplineSwiftStrike = 5,
	TE_DisciplineFlowerOfDeath = 6,
	TE_DisciplineDisplacement = 7,
	TE_DisciplineEarthshock = 8,
	TE_DisciplineFrenzy = 9,
	TE_DisciplineVanish = 10,
	TE_DisciplineScouts = 11,
	TE_DisciplineLaceratingThrow = 12,
	TE_DisciplineReposition = 13,
	TE_DisciplineIncitePain = 14,
	TE_DisciplineMajesty = 15,
	TE_DisciplineGuardBreak = 16,
	TE_DisciplinePassion = 17,
	TE_DisciplineCharm = 18,
	TE_DisciplineProjectionDash = 19,
	TE_DisciplineBlindingBeauty = 20,
	TE_DisciplineRejuvenatingVoice = 21,
	TE_DisciplineShackles = 22,
	TE_DisciplineTraps = 23,
	TE_DisciplineShockwavePunch = 24,
	TE_DisciplineFleshOfMarble = 25,
	TE_DisciplineTankyCharge = 26,
	TE_WeaponAbilityAxeThrow = 27,
	TE_WeaponAbilityDash = 28,
	TE_WeaponAbilityHook = 29,
	TE_WeaponAbilityReflect = 30,
	TE_WeaponAbilitySpikedBat = 31,
	TE_WeaponAbilityGreatsword = 32,
	TE_WeaponAbilityThrowingKnifes = 33,
	TE_DisciplineCount = 34,
	TE_MAX = 35
};

struct FDisciplineUse {
	ETigerDisciplinesEnum DisciplineUsed;
	char pad_1[0x3];
	FVector Location; 
	FVector Direction; 
	char pad_1C[0x4]; 
	TArray<APawn*> Targets; 
	FVector DisciplineSpecificVector1; 
	FVector DisciplineSpecificVector2; 
	bool bDisciplineSpecificBool1;
	bool bDisciplineSpecificBool2; 
	char pad_4A[0x6]; 
};

bool EngineInit();
extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UObject* Server_UseDisciplineUFunction;
extern void* ProcessEventAddress;
