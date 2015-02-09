|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Setup.inc
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains setup functions for thf.mac
|---------------------------------------------------------------------------------------------------------------------------------------------------|

#INCLUDE thf_includes\ninjadvloot.inc
#INCLUDE thf_includes\wait4Rez.inc
|- #INCLUDE thf_includes\sub.inc
|- #INCLUDE thf_includes\events.inc
#INCLUDE thf_includes\MQ2Cast_Spell_Routines.inc

#INCLUDE thf_includes\Alerts.inc
#INCLUDE thf_includes\Assists.inc
#INCLUDE thf_includes\Background.inc
#INCLUDE thf_includes\Basics.inc
#INCLUDE thf_includes\BuffCheck.inc
#INCLUDE thf_includes\Casting.inc
#INCLUDE thf_includes\Heals.inc
#INCLUDE thf_includes\Utilities.inc

#INCLUDE thf_includes\class_Bard.inc
#INCLUDE thf_includes\class_Beastlord.inc
#INCLUDE thf_includes\class_Berserker.inc
#INCLUDE thf_includes\class_Cleric.inc
#INCLUDE thf_includes\class_Druid.inc
#INCLUDE thf_includes\class_Enchanter.inc
#INCLUDE thf_includes\class_Magician.inc
#INCLUDE thf_includes\class_Monk.inc
#INCLUDE thf_includes\class_Necromancer.inc
#INCLUDE thf_includes\class_Paladin.inc
#INCLUDE thf_includes\class_Ranger.inc
#INCLUDE thf_includes\class_Rogue.inc
#INCLUDE thf_includes\class_ShadowKnight.inc
#INCLUDE thf_includes\class_Shaman.inc
#INCLUDE thf_includes\class_Warrior.inc
#INCLUDE thf_includes\class_Wizard.inc

SUB thfSetup(bool debug)
	/if (${outerDebug}) /echo |- thfSetup ==>

	|- Debug
	/declare outerDebug bool outer FALSE
	/if (${Bool[${debug}]}) /varset outerDebug TRUE

	|- The file path for THF.ini will still need to be updated in corresponding includes because you must use /noparse to write variables to inis.
	/declare iniTHF string outer thf_inis\THF.ini
	/declare i int local

	/call checkPlugins
	
	/echo Loading THF macro...

	|- create a macro data
	/if (!${Ini[${iniTHF}].Length}) {
		/echo Welcome to THF ! performing first time setup...
		/call makeThfIni
	}

	|- Import Paths
	|-- General Settings
	/if (!${Ini[${iniTHF},Paths,General].Length}) {
		/echo ERROR: Could not find designated file path for [General].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniGeneral string outer ${Ini[${iniTHF},Paths,General]}
	}

	|-- Toon Settings
	/if (!${Ini[${iniTHF},Paths,Toon].Length}) {
		/echo ERROR: Could not find designated file path for [Toon].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniToon string outer ${Ini[${iniTHF},Paths,Toon]}
	}
	
	|- Create/Read Ini files
	|-- Macro Setting Inis
	/if (${Bool[${Ini[${iniGeneral}]}]}) {
		/call readMacroSettings
	} else {
		/call EVENT_MakeMacroSettingsIni
	}
	
	|-- Generate iniToon.
	/if (!${Bool[${Ini[${iniToon}]}]}) /call EVENT_MakeCharacterSettingsIni
	
	|- Initializing functions
	/if (${Bool[${Ini[${iniTHF},${Me.Class},Functions]}]}) {
		/call splitFromIni "${iniTHF},${Me.Class},Functions" functions outer /
	} else {
		/call MakeFunctionsSettings
	}

	
	|-- Macro Aliases
	/call EVENT_UpdateAliases

	|- Import ini settings
	/echo Importing user settings...
	
	|- Complete a pending exchange
	/if (${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Length} && ${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].NotEqual[NONE]} && !${itemExchange_Timer}) /call completePendingExchange
	
	/call checkToons
	
	/echo THF loaded !
	/echo
	
	/if (${outerDebug}) {
		/echo
		/echo
		/echo *** Debug_Mode ON ***
		/echo The macro has paused so you can review setup information, type /mqpause to continue.
		/echo
		/echo
		/mqpause
	}

	/if (${outerDebug}) /echo <== thfSetup -|	
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Creates a file settings file to change Paths, and store various pieces of data
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB makeThfIni()
	/if (${outerDebug}) /echo |- makeThfIni ==>

	/noparse /ini "thf_inis\THF.ini" "Paths" "Toon" "thf_inis\${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName}.ini"
	/noparse /ini "thf_inis\THF.ini" "Paths" "General" "thf_inis\General.ini"

	/if (${outerDebug}) /echo <== makeThfIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Launch subroutine for each array element
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB loadAll(String tableau, String subToCall)
	/if (${outerDebug}) /echo |- makeThfIni ==>

	/echo Calling ${Me.Class.ShortName}_${subToCall}
	/call ${Me.Class.ShortName}_${subToCall}
	/declare i int local
	/for i 1 to ${${tableau}.Size}
		/if (${outerDebug}) /echo Calling ${${tableau}[${i}]}_${subToCall}
		/if (${Bool[${${tableau}[${i}]}]}) /call ${${tableau}[${i}]}_${subToCall}
	/next i

	/if (${outerDebug}) /echo <== makeThfIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generate the general Macro Settings file
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeMacroSettingsIni "[MQ2] Make Macro ini"
SUB EVENT_MakeMacroSettingsIni()
	/if (${outerDebug}) /echo |- MakeMacroSettingsIni ==>

	/echo Creating macro settings files...

	/call writeToIni "${iniGeneral},General,Channel" team
	/call writeToIni "${iniGeneral},Debug,Debug Mode (On/Off)" Off TRUE
	/call writeToIni "${iniGeneral},General,Includes" Assist/Background/Basics/buffCheck/Casting
	/call writeToIni "${iniGeneral},General,StickCommand" "5 hold behindonce uw"
	/call writeToIni "${iniGeneral},General,Main Tank" "[your tank name here]"
	
	/call splitFromIni "${iniGeneral},General,Includes" includes outer /
	
	/call loadAll includes MacroSettings
	
	/if (${outerDebug}) /echo <== MakeMacroSettingsIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Read the general Macro Settings file
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB readMacroSettings()
	/if (${outerDebug}) /echo |- readMacroSettings ==>

	|-- Chat Channel
	/if (!${Defined[channelNumber]}) /declare channelNumber int outer 0
	/call iniToVar "${iniGeneral},General,Channel" channel string outer
	/call joinChannel

	/call iniToVar "${iniGeneral},General,StickCommand" outerStickCommand string outer
	
	/call iniToVar "${iniGeneral},General,Main Tank" outerMainTank string outer
	/if (!${Defined[outerMainTankID]}) /declare outerMainTankID int outer 0
	/if (${Me.CleanName.Equal[${outerMainTank}]}) {
		/chat ${channelNumber} I'm the main tank !
		/varset outerMainTankID ${Me.ID}
	}
	
	/call splitFromIni "${iniGeneral},General,Includes" includes outer /
	/call loadAll includes Setup
	
	/if (${outerDebug}) /echo <== readMacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeCharacterSettingsIni "[MQ2] Make bot ini"
SUB EVENT_MakeCharacterSettingsIni()
	/if (${outerDebug}) /echo |- EVENT_MakeCharacterSettingsIni ==>

	/echo Creating ${Me.CleanName}'${If[${Me.CleanName.Right[1].Equal[s]},,s]} settings file...
	
	/call loadAll includes CharacterSettings
	
	/if (${outerDebug}) /echo <== EVENT_MakeCharacterSettingsIni -|
/RETURN	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generates functions for each class
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MakeFunctionsSettings()
	/ini ${iniTHF} "Bard" "functions" "check_Burns/check_CombatBuffs/check_Buffs" 
	/ini ${iniTHF} "Beastlord" "functions" "check_Debuffs/check_DoTs/check_Burns/check_CombatBuffs/check_Heals/check_PetHeal/check_Nukes/check_Buffs/check_Pets" 
	/ini ${iniTHF} "Berserker" "functions" "check_Burns/check_CombatBuffs/check_Buffs" 
	/ini ${iniTHF} "Cleric" "functions" "check_DivineArb/check_Yaulp/check_celestialRegen/check_Heals/check_CombatBuffs/check_Debuffs/check_Nukes/check_Buffs" 
	/ini ${iniTHF} "Druid" "functions" "check_Heals/check_CombatBuffs/check_Burns/check_Debuffs/check_DoTs/check_Rain/check_Nukes/check_Buffs" 
	/ini ${iniTHF} "Enchanter" "functions" "check_Rune/check_CombatBuffs/check_Burns/check_Debuffs/check_DoTs/check_Rain/check_Nukes/check_Buffs/check_Pets" 
	/ini ${iniTHF} "Magician" "functions" "check_CombatBuffs/check_petHeal/check_Burns/check_Debuffs/check_Rain/check_Nukes/check_Buffs/check_Pets/check_PetWeapons" 
	/ini ${iniTHF} "Monk" "functions" "check_Burns/check_CombatBuffs/check_Buffs" 
	/ini ${iniTHF} "Necromancer" "functions" "check_Burns/check_castLich/check_manaDump/check_Debuffs/check_DoTs/check_CombatBuffs/check_lifeTap/check_Nukes/check_petHeal/check_Buffs/check_Pets" 
	/ini ${iniTHF} "Paladin" "functions" "check_Yaulp/check_Heals/check_Burns/check_CombatBuffs/check_Nukes/check_Buffs" 
	/ini ${iniTHF} "Ranger" "functions" "check_Burns/check_CombatBuffs/check_Heals/check_DoTs/check_Nukes/check_Buffs" 
	/ini ${iniTHF} "Rogue" "functions" "check_Burns/check_CombatBuffs/check_Buffs" 
	/ini ${iniTHF} "ShadowKnight" "functions" "check_Burns/check_lifeTap/check_Debuffs/check_CombatBuffs/check_DoTs/check_Nukes/check_Buffs/check_Pets" 
	/ini ${iniTHF} "Shaman" "functions" "check_Debuffs/check_Burns/check_Heals/check_CombatBuffs/check_DoTs/check_petHeal/check_Rain/check_Nukes/check_Buffs/check_Pets/check_Canni" 
	/ini ${iniTHF} "Warrior" "functions" "check_Burns/check_CombatBuffs/check_Buffs" 
	/ini ${iniTHF} "Wizard" "functions" "check_Burns/check_CombatBuffs/check_Rain/check_Nukes/check_Harvest/check_Buffs" 
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Deletes and Recreates aliases associated with thf.mac.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT UpdateAliases "[MQ2] Update macro aliases"
SUB EVENT_UpdateAliases()
	/if (${outerDebug}) /echo |- EVENT_UpdateAliases ==>

	/echo Updating macro aliases....
	/echo (WARNING: Your clients will lock up for a few moments.  Stand fast, friend...)
	/echo Aliases may not work properly until you restart MQ2 on ALL clients.
	
	|- Core aliases
	/squelch /alias /updateAliases /echo Update macro aliases
	/squelch /alias /makeCharIni /echo Make Character Ini
	/squelch /alias /makeSettingsIni /echo Make General Settings Ini
	/squelch /alias /makeAdvSettingsIni /echo Make Advanced Settings Ini
	/squelch /alias /thfHelp /echo Help
	/squelch /alias /debugon /varset Debug 1
	/squelch /alias /debugoff /varset Debug 0

	/call loadAll includes Aliases
	
	|/call VetAAs_Aliases

	/if (${outerDebug}) /echo <== EVENT_UpdateAliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for and enables required macro plugins.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB checkPlugins()
	/echo Checking plugins...

	|- REQUIRED PLUGINS ==>
	
	|- Check MQ2EQBC.
	/if (!${Plugin[MQ2EQBC].Name.Length}) {
		/echo Plugin MQ2EQBC is not loaded, attempting to resolve...
		/plugin MQ2EQBC
		/delay 10s !${Select[${EQBC},NULL]}
		/if (${Select[${EQBC},NULL]}) {
			/echo ***WARNING*** Could not load MQ2EQBC, macro functionality may be limited.
			/echo Starting in Solo Mode...
		}
	}

	|- Auto-Connect to EQBCS.
	/if (${Bool[${EQBC}]}) {
		/if (!${EQBC.Connected}) {
			/bccmd connect
			/delay 100 ${EQBC.Connected}
			/delay 5
			/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS! Please open EQBCS and try again.  Macro functionality may be limited...
		}


		|- Check MQ2NetBots.
		/if (!${Plugin[MQ2NetBots].Name.Length}) {
			/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
			/plugin MQ2NetBots
			/delay 3s ${NetBots}
			/if (!${NetBots}) {
				/echo ***WARNING*** Could not load MQ2NetBots! Macro functionality may be limited.
			}
		}
		
		|- Auto-Connect to NetBots.
		/if (${NetBots}) {
			/squelch /netbots on grab=on send=on
		}
		
	}
	
	|- Check MQ2MoveUtils.
	/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
		/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
		/plugin MQ2MoveUtils
		/delay 3s !${Select[${Stick.Status},NULL]}
		/if (${Select[${Stick.Status},NULL]}) {
			/echo ***WARNING*** Could not load MQ2MoveUtils! Macro functionality may be limited.
			/echo Assist and NPC stick DISABLED.
			/echo Follow restricted to NetBots.
		}
	}	

	|- Check MQ2Exchange.
	/if (!${Plugin[MQ2Exchange].Name.Length}) {
		/echo Plugin MQ2Exchange is not loaded, attempting to resolve...
		/plugin MQ2Exchange
		/delay 3s ${Plugin[MQ2Exchange].Name.Length}
		/if (!${Plugin[MQ2Exchange].Name.Length}) {
			/echo ***WARNING*** Could not load MQ2Exchange! Macro functionality may be limited.
			/echo Item swapping is DISABLED.
		}
	}

	|- Check MQ2Twist.
	/if (${Select[${Me.Class.ShortName},BRD]}) {
		/if (!${Plugin[MQ2Twist].Name.Length}) {
			/echo Plugin Unable to detect MQ2Twist, attempting to resolve...
			/plugin MQ2Twist
			/delay 3s !${Select[${Twist},NULL]}
			/if (${Select[${Twist},NULL]}) {
				/echo ***WARNING*** Could not load MQ2Twist! Macro functionality may be limited.
				/echo Bard melodies DISABLED.
			}
		}	
	}

	|- Check MQ2ItemDisplay.
	/if (!${Plugin[mq2itemdisplay].Name.Length}) /plugin MQ2ItemDisplay

	|- Check MQ2HUD.
	/if (!${Plugin[mq2hud].Name.Length}) /plugin MQ2HUD	
	
	|- <== REQUIRED PLUGINS
/RETURN

