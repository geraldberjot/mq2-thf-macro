|----------------------------------------------------------------------------|
|- e3_Classes_Beastlord.inc v3.0 ( originally designed to work with e3.mac )-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Beastlord specific functions related to,						-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB BST_Setup
/if (${outerDebug}) /echo |- BST_Setup ==>
	
/if (${outerDebug}) /echo <== BST_Setup -|
/RETURN



SUB BST_checkBackgroundEvents

/RETURN



SUB BST_MacroSettings
/if (${outerDebug}) /echo |- BST_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniTHF},BST Functions,BST Function#1" "check_Debuffs" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#2" "check_DoTs" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#3" "check_Burns" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#4" "check_CombatBuffs" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#5" "check_Heals" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#6" "check_PetHeal" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#7" "check_Nukes" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#8" "check_Buffs" 1
	/call writeToIni "${iniTHF},BST Functions,BST Function#9" "check_Pets" 1

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