#include <stdio.h>
#include "plugin.h"

std::string g_sEndingMapName;
bool g_bUseHostname;

MMSPlugin g_ThisPlugin;
PLUGIN_EXPOSE(MMSPlugin, g_ThisPlugin);

MMSPlugin::MMSPlugin() : m_Serialize(&INetworkMessages::Serialize, this, &MMSPlugin::Hook_NetSerialize, nullptr)
{
}

bool MMSPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngineServer, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);

	const char* pConfigPath = "addons/configs/change_scoreboard_mapname.ini";

	KeyValues::AutoDelete pKeyValues("Change Scoreboard Mapname");
	if (!pKeyValues->LoadFromFile(g_pFullFileSystem, pConfigPath))
	{
		g_SMAPI->Format(error, maxlen, "[%s] Failed to load %s\n", GetLogTag(), pConfigPath);
		return false;
	}

	g_bUseHostname = pKeyValues->GetBool("use_hostname");
	if (!g_bUseHostname)
		g_sEndingMapName = pKeyValues->GetString("map_ending");

	// Install the hook only after configuration succeeds.
	m_Serialize.Add(g_pNetworkMessages);

	return true;
}

KHook::Return<bool> MMSPlugin::Hook_NetSerialize(INetworkMessages *pMessages, bf_write &pBuf, const CNetMessage *pData)
{
	NetMessageInfo_t *pMessageInfo = pData->GetNetMessage()->GetNetMessageInfo();

	if (pMessageInfo->m_MessageId == svc_ClearAllStringTables)
	{
		CNetMessagePB<CSVCMsg_ClearAllStringTables> *pMsg = const_cast<CNetMessage*>(pData)->ToPB<CSVCMsg_ClearAllStringTables>();
		const std::string originalMapName = pMsg->mapname();
		const bool hadMapName = pMsg->has_mapname();
		
		if (g_bUseHostname)
			pMsg->set_mapname(pMsg->mapname() + " | " + GetHostname());
		else
			pMsg->set_mapname(pMsg->mapname() + g_sEndingMapName);

		// The engine can serialize the same message for multiple clients. Keep
		// the suffix on the wire only, without changing the shared message.
		const bool result = m_Serialize.CallOriginal(pMessages, pBuf, pData);
		if (hadMapName)
			pMsg->set_mapname(originalMapName);
		else
			pMsg->clear_mapname();
		return { KHook::Action::Supersede, result };
	}

	return { KHook::Action::Ignore };
}


std::string MMSPlugin::GetHostname()
{
	static ConVarRefAbstract cvHostname("hostname");
	if (cvHostname.IsValidRef())
		return cvHostname.GetString().String();
	return "";
}

bool MMSPlugin::Unload(char *error, size_t maxlen)
{
	m_Serialize.Remove(g_pNetworkMessages);

	return true;
}
