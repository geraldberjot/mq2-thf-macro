|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Wizard.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Wizard specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB check_Harvest

	/if (${autoHarvest} && ${Me.PctMana} < 70 && ${Me.PctHPs} > 35) {
		|- AltAbility
		/if (${Me.PctMana} < 65 && ${Me.AltAbilityReady[Harvest of Druzzil]}) /call thf_Casting 0 "Harvest of Druzzil"

		/if (!${outerActionTaken}) {
		
			/declare castName string local
			/declare i int local
			
			/for i 1 to ${harvestSpells.Size}
			
				/varset castName ${harvestSpells[${i}].Arg[1,/]}
				/call thf_Casting 0 "${harvestSpells[${i}]}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"

			/if (!${outerActionTaken}) /next i
		}
	}

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WIZ_Setup
	/if (${outerDebug}) /echo |- WIZ_Setup ==>
	
	/declare i int local
	
	/call iniToVar "${iniToon},Wizard,Auto-Harvest (On/Off)" autoHarvest bool outer
	/call splitFromIni "${iniToon},Wizard,Harvest spells" harvestSpells outer /
	
	|- Look for a familiar to cast
	/if (${Defined[SelfBuffs]}) {
		/for i 1 to ${SelfBuffs.Size}
			
			
		/next i
	}

	/if (${outerDebug}) /echo <== WIZ_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WIZ_CheckBackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WIZ_MacroSettings
	/if (${outerDebug}) /echo |- WIZ_MacroSettings ==>

	/call writeToIni "${iniTHF},Wizard,Functions" "check_Burns/check_CombatBuffs/check_Rain/check_Nukes/check_Harvest/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== WIZ_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WIZ_CharacterSettings
	/if (${outerDebug}) /echo |- WIZ_CharacterSettings ==>

	/call writeToIni "${iniToon},Wizard,Auto-Harvest (On/Off)"
	/call writeToIni "${iniToon},Wizard,Harvest spells"

	/if (${outerDebug}) /echo <== WIZ_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WIZ_Aliases
	/if (${outerDebug}) /echo |- WIZ_Aliases ==>

	/if (${outerDebug}) /echo <== WIZ_Aliases -|
/RETURN