/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "common.h"
#include "utlvector.h"
#include "steam/steamclientpublic.h"
#include <playerslot.h>
#include "bitvec.h"

enum class ETargetType {
	NONE,
	PLAYER,
	SELF,
	RANDOM,
	RANDOM_T,
	RANDOM_CT,
	ALL,
	T,
	CT,
};

class ZEPlayer
{
public:
	ZEPlayer(CPlayerSlot slot, bool m_bFakeClient = false): m_slot(slot), m_bFakeClient(m_bFakeClient)
	{ 
		m_bAuthenticated = false;
		m_iAdminFlags = 0;
		m_SteamID = nullptr;
		m_bGagged = false;
		m_bMuted = false;
		m_iHideDistance = 0;
		m_bConnected = false;
		m_iTotalDamage = 0;
	}

	bool IsFakeClient() { return m_bFakeClient; }
	bool IsAuthenticated() { return m_bAuthenticated; }
	bool IsConnected() { return m_bConnected; }
	uint64 GetSteamId64() { return m_SteamID->ConvertToUint64(); }
	const CSteamID* GetSteamId() { return m_SteamID; }
	bool IsAdminFlagSet(uint64 iFlag);
	
	void SetAuthenticated() { m_bAuthenticated = true; }
	void SetConnected() { m_bConnected = true; }
	void SetSteamId(const CSteamID* steamID) { m_SteamID = steamID; }
	void SetAdminFlags(uint64 iAdminFlags) { m_iAdminFlags = iAdminFlags; }
	void SetPlayerSlot(CPlayerSlot slot) { m_slot = slot; }
	void SetMuted(bool muted) { m_bMuted = muted; }
	void SetGagged(bool gagged) { m_bGagged = gagged; }
	void SetTransmit(int index, bool shouldTransmit) { shouldTransmit ? m_shouldTransmit.Set(index) : m_shouldTransmit.Clear(index); }
	void ClearTransmit() { m_shouldTransmit.ClearAll(); }
	void SetHideDistance(int distance) { m_iHideDistance = distance; }
	void SetTotalDamage(int damage) { m_iTotalDamage = damage; }

	bool IsMuted() { return m_bMuted; }
	bool IsGagged() { return m_bGagged; }
	bool ShouldBlockTransmit(int index) { return m_shouldTransmit.Get(index); }
	int GetHideDistance() { return m_iHideDistance; }
	CPlayerSlot GetPlayerSlot() { return m_slot; }
	int GetTotalDamage() { return m_iTotalDamage; }
	
	void OnAuthenticated();
	void CheckAdmin();
	void CheckInfractions();

private:
	bool m_bAuthenticated;
	bool m_bConnected;
	const CSteamID* m_SteamID;
	bool m_bFakeClient;
	bool m_bMuted;
	bool m_bGagged;
	CPlayerSlot m_slot;
	uint64 m_iAdminFlags;
	int m_iHideDistance;
	CBitVec<MAXPLAYERS> m_shouldTransmit;
	int m_iTotalDamage;
};

class CPlayerManager
{
public:
	CPlayerManager()
	{
		V_memset(m_vecPlayers, 0, sizeof(m_vecPlayers));
		V_memset(m_UserIdLookup, -1, sizeof(m_UserIdLookup));
		m_nUsingStopSound = 0;
		m_nUsingSilenceSound = -1; // On by default
		m_nUsingStopDecals = -1; // On by default
	}

	bool OnClientConnected(CPlayerSlot slot);
	void OnClientDisconnect(CPlayerSlot slot);
	void OnBotConnected(CPlayerSlot slot);
	void TryAuthenticate();
	void CheckInfractions();
	void CheckHideDistances();
	CPlayerSlot GetSlotFromUserId(int userid);
	ZEPlayer *GetPlayerFromUserId(int userid);
	ETargetType TargetPlayerString(int iCommandClient, const char* target, int &iNumClients, int *clients);
	ZEPlayer *GetPlayer(CPlayerSlot slot) { return m_vecPlayers[slot.Get()]; };

	uint64 GetStopSoundMask() { return m_nUsingStopSound; }
	uint64 GetSilenceSoundMask() { return m_nUsingSilenceSound; }
	uint64 GetStopDecalsMask() { return m_nUsingStopDecals; }
	
	void SetPlayerStopSound(int slot, bool set);
	void SetPlayerSilenceSound(int slot, bool set);
	void SetPlayerStopDecals(int slot, bool set);

	void ResetPlayerFlags(int slot);

	bool IsPlayerUsingStopSound(int slot) { return m_nUsingStopSound & ((uint64)1 << slot); }
	bool IsPlayerUsingSilenceSound(int slot) { return m_nUsingSilenceSound & ((uint64)1 << slot); }
	bool IsPlayerUsingStopDecals(int slot) { return m_nUsingStopDecals & ((uint64)1 << slot); }

private:
	ZEPlayer* m_vecPlayers[MAXPLAYERS];
	uint16 m_UserIdLookup[USHRT_MAX+1];

	uint64 m_nUsingStopSound;
	uint64 m_nUsingSilenceSound;
	uint64 m_nUsingStopDecals;
};

extern CPlayerManager *g_playerManager;