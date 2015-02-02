|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- e3_Classes_Beastlord.inc v3.0 ( originally designed to work with e3.mac )-|
|- Adapted for THF by Conradd (macro from Killians of PEQ)												-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Beastlord specific functions related to,						-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|



SUB BST_Setup
/if (${outerDebug}) /echo |- BST_Setup ==>
	
/if (${outerDebug}) /echo <== BST_Setup -|
/RETURN



SUB BST_BackgroundEvents

/RETURN



SUB BST_MacroSettings
/if (${outerDebug}) /echo |- BST_MacroSettings ==>

	|- Adv Settings
	/call writeToIni "${iniTHF},Beastlord,Functions" "check_Debuffs/check_DoTs/check_Burns/check_CombatBuffs/check_Heals/check_PetHeal/check_Nukes/check_Buffs/check_Pets" FALSE

/if (${outerDebug}) /echo <== BST_MacroSettings -|
/RETURN



SUB BST_CharacterSettings
/if (${outerDebug}) /echo |- BST_CharacterSettings ==>

/if (${outerDebug}) /echo <== BST_CharacterSettings -|
/RETURN



SUB BST_Aliases
/if (${outerDebug}) /echo |- BST_Aliases ==>

/if (${outerDebug}) /echo <== BST_Aliases -|
/RETURN