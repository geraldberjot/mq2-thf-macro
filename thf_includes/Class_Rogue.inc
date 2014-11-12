|----------------------------------------------------------------------------|
|- thf_Rogue.inc v1.0                                                       -|
|- Written by Conradd of THF		 	 	 	 	 	 	 	 	 	 	 -|
|----------------------------------------------------------------------------|

|--------------------------------------------------------------------------------|
|- rogueStrike	                                 	 	 	 	 	 	 	 	 -|
|--------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.	 -|
|--------------------------------------------------------------------------------|
SUB check_rogStrike()
	/if (${outerDebug}) /echo |- rogueStrike ==>
	
	/if (${Me.AbilityReady[Hide]}) {
	 	 | Stop moving.
	 	 /if (${Stick.Active}) {
	 	 	/squelch /stick pause
	 	 	/delay 2s !${Me.Moving}
	 	 }
	 	 
	 	 | Turn off combat.
	 	 /if (${Me.Combat}) {
	 	 	/attack off
	 	 	/delay 2s !${Me.Combat}
	 	 }
	 	 
	 	 | Sneak
	 	 /doability Sneak
	 	 /delay 20 ${Me.Sneaking}
	 	 
	 	 | Hide.
	 	 /doability Hide
	 	 /delay 20 ${Me.Invis}
	 	 
	 	 | Use 'Assassin's Strike' type disc.
	/if (${Me.Invis}) {
		/if (${Me.CombatAbilityReady[${strikeDiscipline}]) {
			/disc  ${strikeDiscipline}
		} else {
			/if (!${Defined[strikeItem]}) /declare strikeItem item local
			/varset strikeItem ${FindItem[=Frozen Whispering Tunic]}
			/if (${strikeItem.ID}) {
				/if (${timer-${strikeItem.ID}.Value} <= 0) {
					/casting ${strikeItem.ID}|item
					/varset ${timer-${strikeItem.ID}} ${timer-${strikeItem.ID}.OriginalValue}
				}
			} else {
				/varset strikeItem ${FindItem[=Cloak of the Deceiver]}
				/if (${strikeItem.ID}) {
					/if (${timer-${strikeItem.ID}.Value} <= 0) {
						/casting ${strikeItem.ID}|item
						/varset ${timer-${strikeItem.ID}} ${timer-${strikeItem.ID}.OriginalValue}
					}
				} else {
					/varset strikeItem ${FindItem[=Cloak of the Deceiver]}
					/if (${strikeItem.ID}) {
						/if (${timer-${strikeItem.ID}.Value} <= 0) {
							/casting ${strikeItem.ID}|item
							/varset ${timer-${strikeItem.ID}} ${timer-${strikeItem.ID}.OriginalValue}
						}
					} else {
					}
				}
			}
		}

		/attack on
		/squelch /stick unpause
		/delay 10
	}

	/﻿if (${outerDebug}) /echo <== rogueStrike -|
/RETURN

|------------------------------------------------------------|
|- AutoHide	 	 	 	 	 	 	 	 	 	 	 	 	 -|
|------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.	 -|
|------------------------------------------------------------|
SUB AutoHide
	 /if (${outerDebug}) /echo |- AutoHide ==>

	 | Engage sneak.
	 /if (!${Me.Sneaking} && ${Me.AbilityReady[Sneak]}) {
	 	 /doability Sneak
	 	 /delay 2s ${Me.Sneaking}
	 }
	 
	 | Engage hide.
	 /if (${Me.Sneaking} && ${Me.AbilityReady[Hide]}) {
	 	 /doability Hide
	 	 /delay 2s ${Me.Invis}
	 }

	 /if (${outerDebug}) /echo <== AutoHide -|
/RETURN

|------------------------------------------------|
|- AutoEvade	 	 	 	 	 	 	 	 	 -|
|------------------------------------------------|
|- Automatically engages hide while in combat.	 -|
|------------------------------------------------|
SUB AutoEvade
	 /if (${outerDebug}) /echo |- AutoEvade ==>

	 /if (${Me.AbilityReady[Hide]}) {
	 	 
	 	 | Turn off combat.
	 	 /if (${Me.Combat}) {
	 	 	 /attack off
	 	 	 /delay 2s !${Me.Combat}
	 	 }
	 	 
	 	 | Hide.
	 	 /doability Hide
	 	 /delay 20 ${Me.Invis}
	 	 
	 	 | Turn combat back on.
	 	 /if (!${Me.Combat}) {
	 	 	 /attack on
	 	 	 /delay 2s ${Me.Combat}
	 	 }
	 }

	 /if (${outerDebug}) /echo <== AutoEvade -|
/RETURN

|------------------------------------------------|
|- 	 	 	 	 	 	 	 	 	 	 	 	 -|
|------------------------------------------------|
SUB ROG_Setup
	/if (${outerDebug}) /echo |- ROG_Setup ==>

	| Auto-Hide
	/call iniToVar "${iniCharacter},Rogue,Auto-Hide (On/Off)" AutoHide bool outer
	
	| Auto-Evade
	/call iniToVar "${iniCharacter},Rogue,Auto-Evade (On/Off)" AutoEvade bool outer
	/if (${AutoEvade}) /call BuildArray MyAbilities Evade
﻿	
	| Pick up the lastest discipline
	/call getLatestStrikeDiscipline
	
	/call createStrikeItemsTimer

	 /if (${outerDebug}) /echo <== ROG_Setup -|
/RETURN

|--------------------------------------------------------|
|- Check every strike discipline to get the highest one -|
|--------------------------------------------------------|
SUB getLatestStrikeDiscipline()
	/if (${outerDebug}) /echo |- getLatestStrikeDiscipline ==>
	
	/if (!${Defined[strikeDiscipline]}) /declare strikeDiscipline string outer

	/if (${Me.CombatAbility[Death Bringer Rk. III]}) {
		/varset strikeDiscipline "Death Bringer Rk. III"
	} else /if (${strikeDisc.Equal[Death Bringer Rk. II]}) {
		/varset strikeDiscipline "Death Bringer Rk. II"
	} else /if (${strikeDisc.Equal[Death Bringer Rk. I]}) {
		/varset strikeDiscipline "Death Bringer Rk. I"
	} else /if (${strikeDisc.Equal[Daggerfall]}) {
		/varset strikeDiscipline "Daggerfall"
	} else /if (${strikeDisc.Equal[Ancient: Chaos Strike]}) {
		/varset strikeDiscipline "Ancient: Chaos Strike"
	} else /if (${strikeDisc.Equal[Kyv Strike]}) {
		/varset strikeDiscipline "Kyv Strike"
	} else /if (${strikeDisc.Equal[Assassin's Strike]}) {
		/varset strikeDiscipline "Assassin's Strike"
	} else /if (${strikeDisc.Equal[Thief's Vengeance]}) {
		/varset strikeDiscipline "Thief's Vengeance"
	} else /if (${strikeDisc.Equal[Sneak Attack]}) {
		/varset strikeDiscipline "Sneak Attack"
	}
	
	/if (${outerDebug}) /echo <== getLatestStrikeDiscipline -|
/RETURN

|--------------------------------------------------------|
|- Create the timers of the strike items available      -|
|--------------------------------------------------------|
SUB createStrikeItemsTimer()
	/if (${outerDebug}) /echo |- createStrikeItemsTimer ==>
	
	/if (!${Defined[strikeItem]}) /declare strikeItem item local
	
	/varset strikeItem ${FindItem[=Frozen Whispering Tunic]}
	/if (${strikeItem.ID}) /declare timer-${strikeItem.ID} timer outer 5m
	
	/varset strikeItem ${FindItem[=Cloak of the Deceiver]}
	/if (${strikeItem.ID}) /declare timer-${strikeItem.ID} timer outer 40s
	
	/varset strikeItem ${FindItem[=Shroud of the Deceiver]}
	/if (${strikeItem.ID}) /declare timer-${strikeItem.ID} timer outer 40s

	/varset strikeItem ${FindItem[=Fizzlethorp's Cloak of Shadows]}
	/if (${strikeItem.ID}) /declare timer-${strikeItem.ID} timer outer 40s

	/if (${outerDebug}) /echo <== createStrikeItemsTimer -|
/RETURN

SUB ROG_checkBackgroundEvents

	 /if (${AutoHide} && !${Me.Invis} && !${Me.Moving} && !${Me.Combat}) /call AutoHide

/RETURN



SUB ROG_MacroSettings
/if (${outerDebug}) /echo |- ROG_MacroSettings ==>

	 | iniTHF settings
	 |/call writeToIni "${iniTHF},Class Specific Info,Rogue Sneak Attack Disciplines" "Sneak Attack,Thief's Vengeance,Assassin's Strike,Kyv Strike,Ancient: Chaos Strike,Daggerfall,Razorarc,Swiftblade,Swiftblade Rk. II,Swiftblade Rk. III,Daggerlunge,Daggerlunge Rk. II,Daggerlunge Rk. III,Daggerswipe,Daggerswipe Rk. II,Daggerswipe Rk. III,Daggerstrike,Daggerstrike Rk. II,Daggerstrike Rk. III,Daggerthrust,Daggerthrust Rk. II,Daggerthrust Rk. III" 0
   /call writeToIni "${iniTHF},Class Specific Info,Rogue Sneak Attack Disciplines" "Assassin's Chaos Rk. I,Assassin's Chaos Rk. II,Assassin's Chaos Rk. III,Deep Shadows Rk. I,Deep Shadows Rk. II,Deep Shadows Rk. III,Feint of Shadow Rk. I,Feint of Shadow Rk. II,Feint of Shadow Rk. III,Death Bringer Rk. I,Death Bringer Rk. II,Death Bringer Rk. III" 0

	 | Adv Settings
	 /call writeToIni "${iniAdvanced},ROG Functions,ROG Function#1" "check_Burns" 1
	 /call writeToIni "${iniAdvanced},ROG Functions,ROG Function#2" "check_CombatBuffs" 1
	 /call writeToIni "${iniAdvanced},ROG Functions,ROG Function#3" "checkBuffs" 1

	/if (${outerDebug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${outerDebug}) /echo |- ROG_CharacterSettings ==>

	 /call writeToIni "${iniCharacter},${Me.Class},Auto-Hide (On/Off)"
	 /call writeToIni "${iniCharacter},${Me.Class},Auto-Evade (On/Off)"

/if (${outerDebug}) /echo <== ROG_CharacterSettings -|
/RETURN



SUB ROG_Aliases
/if (${outerDebug}) /echo |- ROG_Aliases ==>

/if (${outerDebug}) /echo <== ROG_Aliases -|
/RETURN