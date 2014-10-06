|----------------------------------------------------------------------------|
|- thf_Rogue.inc v1.0                                                       -|
|- Written by Conradd of THF﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  -|
|----------------------------------------------------------------------------|

|--------------------------------------------------------------------------------|
|- rogueStrike﻿                                  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  -|
|--------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.﻿  -|
|--------------------------------------------------------------------------------|
Sub check_rogStrike()
﻿  /if (${Debug}) /echo |- rogueStrike ==>
﻿  
﻿	| Pick up the lastest discipline
	/if (!${Defined[strikeDiscipline]}) /declare strikeDiscipline string outer
	/call pickLatestStrikeDiscipline

﻿  /if (${Me.AbilityReady[Hide]}) {
﻿  ﻿  | Stop moving.
﻿  ﻿  /if (${Stick.Active}) {
﻿  ﻿  ﻿  /squelch /stick pause
﻿  ﻿  ﻿  /delay 2s !${Me.Moving}
﻿  ﻿  }
﻿  ﻿  
﻿  ﻿  | Turn off combat.
﻿  ﻿  /if (${Me.Combat}) {
﻿  ﻿  ﻿  /attack off
﻿  ﻿  ﻿  /delay 2s !${Me.Combat}
﻿  ﻿  }
﻿  ﻿  
﻿  ﻿  | Sneak
﻿  ﻿  /doability Sneak
﻿  ﻿  /delay 20 ${Me.Sneaking}
﻿  ﻿  
﻿  ﻿  | Hide.
﻿  ﻿  /doability Hide
﻿  ﻿  /delay 20 ${Me.Invis}
﻿  ﻿  
﻿  ﻿  | Use 'Assassin's Strike' type disc.
﻿  ﻿  /if (${Me.Invis}) {
﻿  ﻿  ﻿  /if (${Me.CombatAbilityReady[${strikeDiscipline}]) {
﻿  ﻿  ﻿  ﻿  /disc  ${strikeDiscipline}
﻿  ﻿  ﻿  } else {
﻿  ﻿  ﻿  ﻿  /declare i int local
﻿  ﻿  ﻿  ﻿  /for i 1 to ${strikeItemsArray.Size}
			/if (!${Defined[myItem]}) /declare myItem item local
			/varset myItem ${FindItem[=${strikeItemsArray[${i}]}
			/if (!${Defined[timer-${myItem.ID}]}) {
				/declare timer-${myItem.ID} timer outer ${myTimer.TimerReady}
			}
			﻿/if (${timer-${myItem.ID}} <= 0) {
				﻿/casting "${strikeItemsArray[${i}]}" item
				/varset timer-${myItem.ID} ${myTimer.TimerReady}
			﻿}
﻿  ﻿  ﻿  ﻿  /next i
﻿  ﻿  ﻿  }
﻿  ﻿  ﻿  
﻿  ﻿  ﻿  /attack on
﻿  ﻿  ﻿  /squelch /stick unpause
﻿  ﻿  ﻿  /delay 10
﻿  ﻿  }
﻿  }

﻿  /if (${Debug}) /echo <== rogueStrike -|
/return

SUB pickLatestStrikeDiscipline()
	/if (${Me.CombatAbility[Death Bringer Rk. III]}) {
		/varset strikeDiscipline "Death Bringer Rk. III"
		/RETURN
	}
	/if (${strikeDisc.Equal[Death Bringer Rk. II]}) {
		/varset strikeDiscipline "Death Bringer Rk. II"
		/RETURN
	}
	/if (${strikeDisc.Equal[Death Bringer Rk. I]}) {
		/varset strikeDiscipline "Death Bringer Rk. I"
		/RETURN
	}
	/if (${strikeDisc.Equal[Daggerfall]}) {
		/varset strikeDiscipline "Daggerfall"
		/RETURN
	}
	/if (${strikeDisc.Equal[Ancient: Chaos Strike]}) {
		/varset strikeDiscipline "Ancient: Chaos Strike"
		/RETURN
	}
	/if (${strikeDisc.Equal[Kyv Strike]}) {
		/varset strikeDiscipline "Kyv Strike"
		/RETURN
	}
	/if (${strikeDisc.Equal[Assassin's Strike]}) {
		/varset strikeDiscipline "Assassin's Strike"
		/RETURN
	}
	/if (${strikeDisc.Equal[Thief's Vengeance]}) {
		/varset strikeDiscipline "Thief's Vengeance"
		/RETURN
	}
	/if (${strikeDisc.Equal[Sneak Attack]}) {
		/varset strikeDiscipline "Sneak Attack"
	}
/RETURN

|------------------------------------------------------------|
|- AutoHide﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  -|
|------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.﻿  -|
|------------------------------------------------------------|
sub AutoHide
﻿  /if (${Debug}) /echo |- AutoHide ==>

﻿  | Engage sneak.
﻿  /if (!${Me.Sneaking} && ${Me.AbilityReady[Sneak]}) {
﻿  ﻿  /doability Sneak
﻿  ﻿  /delay 2s ${Me.Sneaking}
﻿  }
﻿  
﻿  | Engage hide.
﻿  /if (${Me.Sneaking} && ${Me.AbilityReady[Hide]}) {
﻿  ﻿  /doability Hide
﻿  ﻿  /delay 2s ${Me.Invis}
﻿  }

﻿  /if (${Debug}) /echo <== AutoHide -|
/return

|------------------------------------------------|
|- AutoEvade﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  -|
|------------------------------------------------|
|- Automatically engages hide while in combat.﻿  -|
|------------------------------------------------|
sub AutoEvade
﻿  /if (${Debug}) /echo |- AutoEvade ==>

﻿  /if (${Me.AbilityReady[Hide]}) {
﻿  ﻿  
﻿  ﻿  | Turn off combat.
﻿  ﻿  /if (${Me.Combat}) {
﻿  ﻿  ﻿  /attack off
﻿  ﻿  ﻿  /delay 2s !${Me.Combat}
﻿  ﻿  }
﻿  ﻿  
﻿  ﻿  | Hide.
﻿  ﻿  /doability Hide
﻿  ﻿  /delay 20 ${Me.Invis}
﻿  ﻿  
﻿  ﻿  | Turn combat back on.
﻿  ﻿  /if (!${Me.Combat}) {
﻿  ﻿  ﻿  /attack on
﻿  ﻿  ﻿  /delay 2s ${Me.Combat}
﻿  ﻿  }
﻿  }

﻿  /if (${Debug}) /echo <== AutoEvade -|
/return

|------------------------------------------------|
|- ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  ﻿  -|
|------------------------------------------------|
Sub ROG_Setup
﻿  /if (${Debug}) /echo |- ROG_Setup ==>
﻿  
  | Auto-Hide
﻿  /call IniToVar "${iniCharacter},Rogue,Auto-Hide (On/Off)" AutoHide bool outer
﻿  
﻿  | Auto-Evade
﻿  /call IniToVar "${iniCharacter},Rogue,Auto-Evade (On/Off)" AutoEvade bool outer
﻿  /if (${AutoEvade}) /call BuildArray MyAbilities Evade
﻿  
﻿  /if (${Debug}) /echo <== ROG_Setup -|
/return



Sub ROG_Background_Events

﻿  /if (${AutoHide} && !${Me.Invis} && !${Me.Moving} && !${Me.Combat}) /call AutoHide

/return



SUB ROG_MacroSettings
/if (${Debug}) /echo |- ROG_MacroSettings ==>

﻿  | MacroData_Ini settings
﻿  |/call WriteToIni "${MacroData_Ini},Class Specific Info,Rogue Sneak Attack Disciplines" "Sneak Attack,Thief's Vengeance,Assassin's Strike,Kyv Strike,Ancient: Chaos Strike,Daggerfall,Razorarc,Swiftblade,Swiftblade Rk. II,Swiftblade Rk. III,Daggerlunge,Daggerlunge Rk. II,Daggerlunge Rk. III,Daggerswipe,Daggerswipe Rk. II,Daggerswipe Rk. III,Daggerstrike,Daggerstrike Rk. II,Daggerstrike Rk. III,Daggerthrust,Daggerthrust Rk. II,Daggerthrust Rk. III" 0
   /call WriteToIni "${MacroData_Ini},Class Specific Info,Rogue Sneak Attack Disciplines" "Assassin's Chaos Rk. I,Assassin's Chaos Rk. II,Assassin's Chaos Rk. III,Deep Shadows Rk. I,Deep Shadows Rk. II,Deep Shadows Rk. III,Feint of Shadow Rk. I,Feint of Shadow Rk. II,Feint of Shadow Rk. III,Death Bringer Rk. I,Death Bringer Rk. II,Death Bringer Rk. III" 0

﻿  | Adv Settings
﻿  /call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#1" "check_Burns" 1
﻿  /call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#2" "check_CombatBuffs" 1
﻿  /call WriteToIni "${advSettings_Ini},ROG Functions,ROG Function#3" "check_Buffs" 1

/if (${Debug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${Debug}) /echo |- ROG_CharacterSettings ==>

﻿  /call WriteToIni "${iniCharacter},${Me.Class},Auto-Hide (On/Off)"
﻿  /call WriteToIni "${iniCharacter},${Me.Class},Auto-Evade (On/Off)"

/if (${Debug}) /echo <== ROG_CharacterSettings -|
/RETURN



Sub ROG_Aliases
/if (${Debug}) /echo |- ROG_Aliases ==>

/if (${Debug}) /echo <== ROG_Aliases -|
/return