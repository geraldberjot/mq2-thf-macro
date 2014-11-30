|----------------------------------------------------------------------------|
|- e3_Classes_Wizard.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Wizard specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB check_Harvest

	/if (${autoHarvest} && ${Me.PctMana} < 70 && ${Me.PctHPs} > 35) {
		| AltAbility
		/if (${Me.PctMana} < 65 && ${Me.AltAbilityReady[Harvest of Druzzil]}) /call e3_Cast 0 "Harvest of Druzzil"

		/if (!${ActionTaken}) {
		
			/declare castName string local
			/declare i int local
			
			/for i 1 to ${harvestSpells.Size}
			
				/varset castName ${harvestSpells[${i}].Arg[1,/]}
				/call e3_Cast 0 "${harvestSpells[${i}]}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"

			/if (!${ActionTaken}) /next i
		}
	}

/RETURN



SUB WIZ_Setup
/if (${outerDebug}) /echo |- WIZ_Setup ==>
	
	/declare i int local
	
	/call iniToVar "${iniToon},Wizard,Evac Spell" Evac_Spell string outer
	/call iniToVar "${iniToon},Wizard,Auto-Harvest (On/Off)" autoHarvest bool outer
	/call IniToArray "${iniToon},Wizard,Harvest#" harvestSpells
	
	| Look for a familiar to cast
	/if (${Defined[SelfBuffs]}) {
		/for i 1 to ${SelfBuffs.Size}
			
			
		/next i
	}

/if (${outerDebug}) /echo <== WIZ_Setup -|
/RETURN



SUB WIZ_checkBackgroundEvents

	/if (${Me.Pet.ID} && ${Me.Pet.CleanName.Find[${Me.CleanName}`s Familiar]} && ${MacroQuest.Server.Equal[PEQTGC]}) /pet get lost

/RETURN



SUB WIZ_MacroSettings
/if (${outerDebug}) /echo |- WIZ_MacroSettings ==>

	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#1" "check_Burns" 1
	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#2" "check_CombatBuffs" 1
	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#3" "check_Rain" 1
	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#4" "check_Nukes" 1
	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#5" "check_Harvest" 1
	/call writeToIni "${iniTHF},WIZ Functions,WIZ Function#6" "check_Buffs" 1

/if (${outerDebug}) /echo <== WIZ_MacroSettings -|
/RETURN



SUB WIZ_CharacterSettings
/if (${outerDebug}) /echo |- WIZ_CharacterSettings ==>

	/call writeToIni "${iniToon},Wizard,Evac Spell"
	/call writeToIni "${iniToon},Wizard,Auto-Harvest (On/Off)"
	/call writeToIni "${iniToon},Wizard,Harvest#1"

/if (${outerDebug}) /echo <== WIZ_CharacterSettings -|
/RETURN



SUB WIZ_Aliases
/if (${outerDebug}) /echo |- WIZ_Aliases ==>

/if (${outerDebug}) /echo <== WIZ_Aliases -|
/RETURN