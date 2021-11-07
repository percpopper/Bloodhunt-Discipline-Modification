#include "MinHook/MinHook.h"
#include "Engine.h"

/*
 This can be done by hooking the function itself instead of waiting in process event for the function to be passed.
 This is the function we will be looking into.
 Class Tiger.TigerDisciplineComponent
 Function Tiger.TigerDisciplineComponent.Server_UseDiscipline
 void Server_UseDiscipline(FDisciplineUse InDisciplineUse);

 MinHook Was Used For Hooking: https://github.com/TsudaKageyu/minhook
 CheatIt Was Used For Engine.h/cpp & Util To Find The Function: https://github.com/guttir14/CheatIt
 */

void(*OProcessEvent)(UObject*, UFunction*, void*) = nullptr;
void __fastcall ProcessEvent(UObject* thisUObject, UFunction* Function, void* Src)
{
	/*
		This modifys the parameters when this function is called through process event. Specifically the FVectors passed for the use of launching your character.
	*/

	// Check if Function Tiger.TigerDisciplineComponent.Server_UseDiscipline is passed as the function.
	if (Function == Server_UseDisciplineUFunction) {

		/*
			For this location, think of it like a slingshot, if its further from you, your velocity when exiting your ability will be greater.
			Easy way to use is visualize this location and have controls for moving it or you can do some math based on your characters location and rotation.
		*/

		// This is just for this example purpose. You will want to set it somewhere you can control.
		FVector NewLocation = FVector{ 0.f, 0.f, 350.f };

		// Modify parameters. I dont know what does what so I set them all. Not all are required for launching your player.
		((FDisciplineUse*)(Src))->DisciplineSpecificVector1 = NewLocation;
		((FDisciplineUse*)(Src))->DisciplineSpecificVector2 = NewLocation;
		((FDisciplineUse*)(Src))->Location = NewLocation;
		((FDisciplineUse*)(Src))->Direction = NewLocation;
		((FDisciplineUse*)(Src))->bDisciplineSpecificBool1 = TRUE;
		((FDisciplineUse*)(Src))->bDisciplineSpecificBool2 = TRUE;
	}

	return OProcessEvent(thisUObject, Function, Src);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return FALSE;

    if (!EngineInit()) return FALSE;

    if (MH_Initialize() != MH_STATUS::MH_OK) return FALSE;

    if (MH_CreateHook(ProcessEventAddress, ProcessEvent, reinterpret_cast<LPVOID*>(&OProcessEvent)) != MH_STATUS::MH_OK) return FALSE;

    if (MH_EnableHook(ProcessEventAddress) != MH_STATUS::MH_OK) return FALSE;

    return TRUE;
}
