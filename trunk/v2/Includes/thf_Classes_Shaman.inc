|----------------------------------------------------------------------------|
|- e3_Classes_Shaman.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Shaman specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Shaman's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Shaman class dynamic.	-|
|--------------------------------------------------------------------|
SUB SHM_Main
/if (${Debug}) /echo |- SHM_Main ==>

	| check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
	/if (!${actionTaken}) {
	
		| check_Heals.
		/if (${Heals}) /call CheckHeals
		/if (!${actionTaken}) {
	
			| checkCombatBuffs.
			/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
			/if (!${actionTaken}) {
	
				| Assist Functions.
				/if (${assisting}) /call ${Me.Class.ShortName}_Assist
			}
		}
	}

/if (${Debug}) /echo <== SHM_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Shaman's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Shaman class dynamic.	-|
|--------------------------------------------------------------------|
SUB SHM_Assist
/if (${Debug}) /echo |- SHM_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== SHM_Assist -|
/RETURN



SUB SHM_Setup
/if (${Debug}) /echo |- SHM_Setup ==>
	
/if (${Debug}) /echo <== SHM_Setup -|
/RETURN



SUB SHM_backgroundEvents
/RETURN



SUB SHM_MacroSettings
/if (${Debug}) /echo |- SHM_MacroSettings ==>

/if (${Debug}) /echo <== SHM_MacroSettings -|
/RETURN



SUB SHM_CharacterSettings
/if (${Debug}) /echo |- SHM_CharacterSettings ==>

/if (${Debug}) /echo <== SHM_CharacterSettings -|
/RETURN



SUB SHM_Aliases
/if (${Debug}) /echo |- SHM_Aliases ==>

/if (${Debug}) /echo <== SHM_Aliases -|
/RETURN