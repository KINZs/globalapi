/* 	
	GOKZ Stocks

	Website: https://bitbucket.org/kztimerglobalteam/gokz
*/

#if defined _gokz_included_
#endinput
#endif
#define _gokz_included_



// =========================  DEFINITIONS  ========================= //

#define GOKZ_VERSION "0.17.1"
#define TICKRATE 128.0
#define EPSILON 0.000001
#define SPEED_NORMAL 250.0
#define SPEED_NO_WEAPON 260.0



// =========================  ENUMERATIONS  ========================= //

enum ObsMode
{
	ObsMode_None = 0,  // Not in spectator mode
	ObsMode_DeathCam,  // Special mode for death cam animation
	ObsMode_FreezeCam,  // Zooms to a target, and freeze-frames on them
	ObsMode_Fixed,  // View from a fixed camera position
	ObsMode_InEye,  // Follow a player in first person view
	ObsMode_Chase,  // Follow a player in third person view
	ObsMode_Roaming // Free roaming
};



// =========================  DATA  ========================= //

stock char gC_WeaponNames[][] = 
{
	"weapon_knife", "weapon_hkp2000", "weapon_deagle", "weapon_elite", "weapon_fiveseven", 
	"weapon_glock", "weapon_p250", "weapon_tec9", "weapon_decoy", "weapon_flashbang", 
	"weapon_hegrenade", "weapon_incgrenade", "weapon_molotov", "weapon_smokegrenade", "weapon_taser", 
	"weapon_ak47", "weapon_aug", "weapon_awp", "weapon_bizon", "weapon_famas", 
	"weapon_g3sg1", "weapon_galilar", "weapon_m249", "weapon_m4a1", "weapon_mac10", 
	"weapon_mag7", "weapon_mp7", "weapon_mp9", "weapon_negev", "weapon_nova", 
	"weapon_p90", "weapon_sawedoff", "weapon_scar20", "weapon_sg556", "weapon_ssg08", 
	"weapon_ump45", "weapon_xm1014"
};

stock int gI_WeaponRunSpeeds[sizeof(gC_WeaponNames)] = 
{
	250, 240, 230, 240, 240, 
	240, 240, 240, 245, 245, 
	245, 245, 245, 245, 220, 
	215, 220, 200, 240, 220, 
	215, 215, 195, 225, 240, 
	225, 220, 240, 150, 220, 
	230, 210, 215, 210, 230, 
	230, 215
};



// =========================  STOCKS  ========================= //

/**
 * Represents a time float as a string e.g. 01:23.45.
 *
 * @param time			Time in seconds.
 * @param precise		Whether to include fractional seconds.
 * @return				String representation of time.
 */
stock char[] GOKZ_FormatTime(float time, bool precise = true)
{
	char formattedTime[12];
	
	int roundedTime = RoundFloat(time * 100); // Time rounded to number of centiseconds
	
	int centiseconds = roundedTime % 100;
	roundedTime = (roundedTime - centiseconds) / 100;
	int seconds = roundedTime % 60;
	roundedTime = (roundedTime - seconds) / 60;
	int minutes = roundedTime % 60;
	roundedTime = (roundedTime - minutes) / 60;
	int hours = roundedTime;
	
	if (hours == 0)
	{
		if (precise)
		{
			FormatEx(formattedTime, sizeof(formattedTime), "%02d:%02d.%02d", minutes, seconds, centiseconds);
		}
		else
		{
			FormatEx(formattedTime, sizeof(formattedTime), "%d:%02d", minutes, seconds);
		}
	}
	else
	{
		if (precise)
		{
			FormatEx(formattedTime, sizeof(formattedTime), "%d:%02d:%02d.%02d", hours, minutes, seconds, centiseconds);
		}
		else
		{
			FormatEx(formattedTime, sizeof(formattedTime), "%d:%02d:%02d", hours, minutes, seconds);
		}
	}
	return formattedTime;
}

/**
 * Checks if the value is a valid client entity index, if they are in-game and not GOTV.
 *
 * @param client		Client index.
 * @return				Whether client is valid.
 */
stock bool IsValidClient(int client)
{
	return client >= 1 && client <= MaxClients && IsClientInGame(client) && !IsClientSourceTV(client);
}

/**
 * Returns the greater of two float values.
 *
 * @param value1		First value.
 * @param value2		Second value.
 * @return				Greatest value.
 */
stock float FloatMax(float value1, float value2)
{
	if (value1 >= value2)
	{
		return value1;
	}
	return value2;
}

/**
 * Returns the lesser of two float values.
 *
 * @param value1		First value.
 * @param value2		Second value.
 * @return				Lesser value.
 */
stock float FloatMin(float value1, float value2)
{
	if (value1 <= value2)
	{
		return value1;
	}
	return value2;
}

/**
 * Returns the greater of two int values.
 *
 * @param value1		First value.
 * @param value2		Second value.
 * @return				Greatest value.
 */
stock int IntMax(int value1, int value2)
{
	if (value1 >= value2)
	{
		return value1;
	}
	return value2;
}

/**
 * Returns the lesser of two int values.
 *
 * @param value1		First value.
 * @param value2		Second value.
 * @return				Lesser value.
 */
stock int IntMin(int value1, int value2)
{
	if (value1 <= value2)
	{
		return value1;
	}
	return value2;
}

/**
 * Sets all characters in a string to lower case.
 *
 * @param input			Input string.
 * @param output		Output buffer.
 * @param size			Maximum size of output.
 * @noreturn
 */
stock void String_ToLower(const char[] input, char[] output, int size)
{
	size--;
	int i = 0;
	while (input[i] != '\0' && i < size)
	{
		output[i] = CharToLower(input[i]);
		i++;
	}
	output[i] = '\0';
}

/**
 * Gets the client's observer mode.
 *
 * @param client		Client index.
 * @return				Current observer mode.
 */
stock ObsMode GetObserverMode(int client)
{
	return view_as<ObsMode>(GetEntProp(client, Prop_Send, "m_iObserverMode"));
}

/**
 * Gets the player a client is spectating.
 *
 * @param client		Client index.
 * @return				Client index of target, or -1 if not spectating anyone.
 */
stock int GetObserverTarget(int client)
{
	ObsMode mode = GetObserverMode(client);
	if (mode == ObsMode_InEye || mode == ObsMode_Chase)
	{
		return GetEntPropEnt(client, Prop_Send, "m_hObserverTarget");
	}
	return -1;
}

/**
 * Emits a sound to other players that are spectating the client.
 *
 * @param client		Client being spectated.
 * @param sound			Sound to play.
 * @noreturn
 */
stock void EmitSoundToClientSpectators(int client, const char[] sound)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetObserverTarget(i) == client)
		{
			EmitSoundToClient(i, sound);
		}
	}
}

/**
 * Calculates the delta angle between two angles (-180 to 180).
 *
 * @param startAngle	Start angle.
 * @param endAngle		End angle.
 * @return				Delta angle.
 */
stock float CalcDeltaAngle(float startAngle, float endAngle)
{
	float difference = endAngle - startAngle;
	if (difference > 180)
	{
		difference + 360;
	}
	else if (difference < -180)
	{
		difference - 360;
	}
	return difference;
}

/**
 * Strips all color control characters in a string.
 * The Output buffer can be the same as the input buffer.
 * Original code by Psychonic, thanks.
 * Source: smlib
 *
 * @param input				Input String.
 * @param output			Output String.
 * @param size				Max Size of the Output string
 * @noreturn
 */
stock void Color_StripFromChatText(const char[] input, char[] output, int size)
{
	int x = 0;
	for (int i = 0; input[i] != '\0'; i++) {
		
		if (x + 1 == size) {
			break;
		}
		
		int character = input[i];
		
		if (character > 0x08) {
			output[x++] = character;
		}
	}
	
	output[x] = '\0';
}

/**
 * Returns an integer as a string.
 *
 * @param num				Integer to stringify.
 * @return					Integer as a string.
 */
stock char[] IntToStringEx(int num)
{
	char string[12];
	IntToString(num, string, sizeof(string));
	return string;
} 