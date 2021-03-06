#include "ScriptModule.h"
#include "../DLL.h"

namespace
{

bool GetProcedures(ScriptModule& module)
{
	module.ai = nullptr;
	DLLError err;
	err = GetDLLProcedure(&module.ai, module.dll, "ExecuteAlienScript");
	if (err != DLLError::ok)
	{
		return false;
	}
	return true;
}

}

bool InitScriptModule(ScriptModule& module, const char* dllFileName)
{
	DLLError err = LoadDLL(module.dll, dllFileName);
	if (err != DLLError::ok)
	{
		return false;
	}
	return GetProcedures(module);
}

bool ReloadScriptModule(ScriptModule& module)
{
	// FIXME Release current DLL on success only ?
	bool res = true;
	if (ReloadDLL(module.dll) == DLLError::ok)
	{
		res = GetProcedures(module);
	}
	return res;
}
