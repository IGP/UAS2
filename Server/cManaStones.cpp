/*
* This file is part of UAS2.
*
* UAS2 is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* UAS2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with UASv1; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 *	@file cManaStones.cpp
 *	Implements functionality for mana stones.
 *
 *	This class is referenced whenever a mana stone is created, used, or assessed.
 *	Inherits from the cObject class.
 */

#include "avatar.h"
#include "object.h"
#include "cItemModels.h"
#include "client.h"
#include "WorldManager.h"
#include "MasterServer.h"

/**
 *	Handles the message sent for the creation of mana stones in the world.
 *
 *	This function is called whenever a mana stone should be created in the world for a client.
 *
 *	@return cMessage - Returns a Create Object (0x0000F745) server message.
 */
cMessage cManaStones::CreatePacket( )
{
	cMessage cmReturn;

	cItemModels *pcModel = cItemModels::FindModel( m_dwItemModelID );
	cObject *pcObject = cWorldManager::FindObject( m_dwGUID );

	if( pcModel )
	{
		cmReturn	<< 0xF745L << m_dwGUID << BYTE(0x11); //0x11 is a constant
		cmReturn	<< pcModel->m_bPaletteChange
					<< pcModel->m_bTextureChange
					<< pcModel->m_bModelChange;

		// The Model Vectors
		if ( pcModel->m_bPaletteChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bPaletteChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorPal[i],sizeof(pcModel->m_vectorPal[i]));
			}
		}
		
		if (pcModel->m_bPaletteChange != 0) 
		{
			//Cubem0j0:  Test code for armor only.
			cmReturn << WORD( 0x0C50 );
		}
		
		if ( pcModel->m_bTextureChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bTextureChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorTex[i],sizeof(pcModel->m_vectorTex[i]));
			}
		}

		if ( pcModel->m_bModelChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bModelChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorMod[i],sizeof(pcModel->m_vectorMod[i]));
			}
		}
	}

	cmReturn.pasteAlign(4);
	pcModel->m_dwFlags1 += 0x00008000;
	cmReturn << pcModel->m_dwFlags1 << 0x414L << 0x65L;


	// MASK 0x00008000 -- Location 
	if ( !m_fIsOwned )
		cmReturn.CannedData( (BYTE *)&m_Location, sizeof( cLocation ) );

	// MASK 0x00000001 -- ModelNumber
	DWORD dwModel = 0x02000000L + pcModel->m_dwModelNumber;
	cmReturn << dwModel;

	// SeaGreens
	WORD wNuminteracts = 0x0;
	WORD wNumbubbles = 0x0;
	WORD wNumJumps = 0x0;
	WORD wNumOverrides = 0x0;
	WORD wUnkFlag8 = 0x0;
	WORD wUnkFlag10 = 0x0;
	
	cmReturn	<< m_wPositionSequence
				<< m_wNumAnims //wNuminteracts 
				<< wNumbubbles 
				<< wNumJumps 
				<< m_wNumPortals 
				<< m_wNumAnims 
				<< wNumOverrides 
				<< wUnkFlag8
				<< m_wNumLogins
				<< wUnkFlag10;

	DWORD dwFlags2 = 0x10280018;

	cmReturn << dwFlags2;
	cmReturn << Name( );					// Objects Name
	cmReturn << pcModel->m_wModel;			// Objects Model
	cmReturn << pcModel->m_wIcon;			// Objects Icon
	cmReturn << pcModel->m_dwObjectFlags1;	// Object Flags
	cmReturn << pcModel->m_dwObjectFlags2;	// Object Flags

	//Lockpicks flag2 - 00280c18

	// Mask 0x0008 - Value
	if(pcModel->m_dwFlags2 & 0x00000008)
	{
		cmReturn << pcModel->m_dwValue;
	}
	// Mask 0x0010 dwUnknown_v2
	cmReturn << DWORD(0x000A0008);
		
	// Mase 0x080000 Useable On
	if(pcModel->m_dwFlags2 & 0x00080000)
	{
		cmReturn << 0x891FL;
	}
	// Mase 0x0080 Icon Highlight
	if(pcModel->m_dwFlags2 & 0x00000080)
	{
		cmReturn << pcModel->m_dwIconHighlight;
	}
	// Mask 0x00200000 - Burden
	if(pcModel->m_dwFlags2 & 0x00200000)
	{
		cmReturn << pcModel->m_wBurden;
	}
	//Mask 0x100000000 - Hooks
	if(pcModel->m_dwFlags2 & 0x10000000)
	{
		cmReturn << pcModel->m_wHooks;
	}

	return cmReturn;
}

/**
 *	Handles the message sent for the creation of mana stones in a container.
 *
 *	This function is called whenever a mana stone should be created in the inventory of another object.
 *
 *	@return cMessage - Returns a Create Object (0x0000F745) server message.
 */
cMessage cManaStones::CreatePacketContainer(DWORD Container, DWORD ItemModelID)
{
	cMessage cmReturn;

	cItemModels *pcModel = cItemModels::FindModel( ItemModelID );
	cObject *pcObject = cWorldManager::FindObject( m_dwGUID );

	if( pcModel )
	{
		cmReturn	<< 0xF745L << m_dwGUID << BYTE(0x11); //0x11 is a constant
		cmReturn	<< pcModel->m_bPaletteChange
					<< pcModel->m_bTextureChange
					<< pcModel->m_bModelChange;

		// The Model Vectors
		if ( pcModel->m_bPaletteChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bPaletteChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorPal[i],sizeof(pcModel->m_vectorPal[i]));
			}
		}
		
		if (pcModel->m_bPaletteChange != 0) 
		{
			//Cubem0j0:  Test code for armor only.
			cmReturn << WORD( 0x0C50 );
		}
		
		if ( pcModel->m_bTextureChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bTextureChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorTex[i],sizeof(pcModel->m_vectorTex[i]));
			}
		}

		if ( pcModel->m_bModelChange != 0) 
		{
			for (int i = 0; i < pcModel->m_bModelChange; i++)
			{
				cmReturn.pasteData((UCHAR*)&pcModel->m_vectorMod[i],sizeof(pcModel->m_vectorMod[i]));
			}
		}
	}

	cmReturn.pasteAlign(4);
	cmReturn << pcModel->m_dwFlags1 << 0x414L << 0x65L;

	// MASK 0x00008000 -- Location 
	if ( !m_fIsOwned )
		cmReturn.CannedData( (BYTE *)&m_Location, sizeof( cLocation ) );

	// MASK 0x00000001 -- ModelNumber
	DWORD dwModel = 0x02000000L + pcModel->m_dwModelNumber;
	cmReturn << dwModel;

	// SeaGreens
	WORD wNuminteracts = 0x0;
	WORD wNumbubbles = 0x0;
	WORD wNumJumps = 0x0;
	WORD wNumOverrides = 0x0;
	WORD wUnkFlag8 = 0x0;
	WORD wUnkFlag10 = 0x0;
	
	cmReturn	<< m_wPositionSequence
				<< m_wNumAnims //wNuminteracts 
				<< wNumbubbles 
				<< wNumJumps 
				<< m_wNumPortals 
				<< m_wNumAnims 
				<< wNumOverrides 
				<< wUnkFlag8
				<< m_wNumLogins
				<< wUnkFlag10;
	

	cmReturn << pcModel->m_dwFlags2;		// Object Flags
	cmReturn << Name( );					// Objects Name
	cmReturn << pcModel->m_wModel;			// Objects Model
	cmReturn << pcModel->m_wIcon;			// Objects Icon
	cmReturn << pcModel->m_dwObjectFlags1;	// Object Flags
	cmReturn << pcModel->m_dwObjectFlags2;	// Object Flags

	// Mask 0x0008 - Value
	if(pcModel->m_dwFlags2 & 0x00000008)
	{
		cmReturn << pcModel->m_dwValue;
	}
	// Mask 0x0010 dwUnknown_v2
	cmReturn << DWORD(0x000A0008);	
	// Mase 0x080000 Useable On
	if(pcModel->m_dwFlags2 & 0x00080000)
	{
		cmReturn << 0x891FL;
	}
	// Mase 0x0080 Icon Highlight
	if(pcModel->m_dwFlags2 & 0x00000080)
	{
		cmReturn << pcModel->m_dwIconHighlight;
	}
	// Mask 0x4000 Container ID
	if(pcModel->m_dwFlags2 & 0x00004000)
	{
		cmReturn << Container;
	}
	// Mask 0x00200000 - Burden
	if(pcModel->m_dwFlags2 & 0x00200000)
	{
		cmReturn << pcModel->m_wBurden;
	}
	//Mask 0x100000000 - Hooks
	if(pcModel->m_dwFlags2 & 0x10000000)
	{
		cmReturn << pcModel->m_wHooks;
	}

	return cmReturn;
}

/**
 *	Handles the actions of mana stone objects.
 *
 *	This function is called whenever a mana stone is used or should perform an action.
 */
void cManaStones::Action(cClient *who)
{
	cItemModels *pcModel = cItemModels::FindModel( m_dwItemModelID );
	cObject *pcObject = cWorldManager::FindObject( m_dwGUID );

	cMessage cmActionComplete;
	cmActionComplete << 0xF7B0L << who->m_pcAvatar->GetGUID( ) << ++who->m_dwF7B0Sequence << 0x01C7L << 0L;
	who->AddPacket(WORLD_SERVER,cmActionComplete,4);

	if(pcModel->m_wUses == 0)
	{
		//Remove the item
		who->m_pcAvatar->DeleteFromInventory(pcObject);
		cMessage cmRemoveItem;
		cmRemoveItem << 0x0024L << m_dwGUID;
		who->AddPacket(WORLD_SERVER,cmRemoveItem,4);
	}
}

/**
 *	Handles the assessment of mana stone objects.
 *
 *	This function is called whenever a mana stone is assessed by a client.
 *
 *	Returns a Game Event (0x0000F7B0) server message of type Identify Object (0x000000C9).
 */
void cManaStones::Assess( cClient *pcAssesser )
{
	cItemModels *pcModel = cItemModels::FindModel( m_dwItemModelID );

	cMessage cmAssess;
	DWORD flags = 0x00000009;
	cmAssess << 0xF7B0L << pcAssesser->m_pcAvatar->GetGUID() << ++pcAssesser->m_dwF7B0Sequence << 0xC9L << m_dwGUID
	<< flags 
	<< 0x01L
	<< WORD(0x05)
	<< WORD(0x10)
	<< 0x13L
	<< pcModel->m_dwValue
	<< 0x05L
	<< DWORD(pcModel->m_wBurden)
	<< 0x6BL						//Current mana
	<< 0x00L

	<< WORD(0x01)
	<< WORD(0x08)
	<< 0x89L						//Chance of Destruction
	<< float(50.0)
	
	<< WORD(0x01)
	<< WORD(0x08)
	<< 0x0EL
	<< pcModel->m_strDescription.c_str();

	pcAssesser->AddPacket(WORLD_SERVER, cmAssess,4);

	cMessage cmActionComplete;
	cmActionComplete << 0xF7B0L << pcAssesser->m_pcAvatar->GetGUID( ) << ++pcAssesser->m_dwF7B0Sequence << 0x01C7L << 0L;
	pcAssesser->AddPacket(WORLD_SERVER,cmActionComplete,4);
}