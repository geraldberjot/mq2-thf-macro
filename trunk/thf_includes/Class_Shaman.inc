|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Shaman.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Shaman specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- autoCanni
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Uses cannibalize spells and AAs when under 95% mana.
|- Spells must be memmed to be used automatically.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Canni
	/if (${outerDebug}) /echo |- autoCanni ==>

	|- Check if Auto-Canni is on
	/if (${AutoCanni}) {
	
		|- AltAbility Cannibalization.
		/if (${Me.PctHPs} > 65 && ${Me.PctMana} < 65 && ${Me.AltAbilityReady[Cannibalization]}) /call thf_Casting 0 "Cannibalization"

		/if (!${outerActionTaken} && !${medBreak}) {
		
			/declare castName string local
			/declare minMana int local
			/declare i int local
			
			/for i 1 to ${canniSpells.Size}
			
				/varset castName ${canniSpells[${i}].Arg[1,/]}
				
				|- Get MinMana
				/if (${canniSpells[${i}].Find[/MinMana|]}) {
					/call argueString MinMana "${canniSpells[${i}]}"
					/varset minMana ${Macro.Return}
				} else {
					/varset minMana 90
				}
				
				/if (${Me.PctMana} < ${minMana} && ${Me.PctHPs} > 35) /call thf_Casting 0 "${canniSpells[${i}]}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"

			/if (!${outerActionTaken}) /next i
		}
	}
	
	/if (${outerDebug}) /echo <== autoCanni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHM_Setup
/if (${outerDebug}) /echo |- SHM_Setup ==>
	
	/call iniToVar "${iniToon},Shaman,Auto-Canni (On/Off)" AutoCanni bool outer
	/call iniToArray "${iniToon},Shaman,Canni#" canniSpells
	
/if (${outerDebug}) /echo <== SHM_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHM_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHM_MacroSettings
	/if (${outerDebug}) /echo |- SHM_MacroSettings ==>

	/call writeToIni "${iniTHF},Shaman,Functions" "check_Debuffs/check_Burns/check_Heals/check_CombatBuffs/check_DoTs/check_petHeal/check_Rain/check_Nukes/check_Buffs/check_Pets/check_Canni" FALSE

	/if (${outerDebug}) /echo <== SHM_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHM_CharacterSettings
	/if (${outerDebug}) /echo |- SHM_CharacterSettings ==>

	/call writeToIni "${iniToon},Shaman,Auto-Canni (On/Off)"
	/call writeToIni "${iniToon},Shaman,Canni spells"

	/if (${outerDebug}) /echo <== SHM_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHM_Aliases
	/if (${outerDebug}) /echo |- SHM_Aliases ==>

	/if (${outerDebug}) /echo <== SHM_Aliases -|
/RETURN