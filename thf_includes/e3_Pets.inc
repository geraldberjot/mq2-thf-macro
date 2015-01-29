|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- e3_Macro_Pets.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Adapted for THF by Conradd (macro from Killians of PEQ)											-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains pet related functions for e3.mac							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|- Add verification to events
|- /tell relays for suspend, return, and shrink pet commands

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Pets.incs Mainloop hook.													-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks your pets status. Automatically, resummons, buffs, and shrinks.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Pets
/if (${Debug} || ${Debug_Pets}) /echo |- check_PetStatus ==>

	/if (${Pets} && !${suspend_Pets} && !${outerCombatTimer} && !${outerActiveTimer}) {
	
		|- Check if you have a pet.
		/if (!${Me.Pet.ID}) /call summonPet
		/if (!${outerActionTaken} && ${Me.Pet.ID}) {
		
			|- Maintain pet buffs.
			/if (${petBuffs.Size} && !${pet_outerRebuffTimer}) /call buffPet
			/if (!${outerActionTaken}) {
			
				|- Check Auto_Shrink.
				/if (${petAutoShrink}) /call check_AutoShrink
			}
		}
	}
	
/if (${Debug} || ${Debug_Pets}) /echo <== check_PetStatus -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Summons a new pet.										-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Looks for reagents, and checks mana, before summoning.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB summonPet
/if (${Debug} || ${Debug_Pets}) /echo |- summonPet ==>

	/declare castName string local ${petSpell.Arg[1,/]}

	/if (${Debug} || ${Debug_Pets}) /echo |- summon_Pet -|- Attempting to summon [${castName}].
	/call thf_Casting ${Me.ID} "${petSpell}/giveuptimer|${If[${Me.Book[${castName}]} && !${Me.Gem[${castName}]},90,0]}"
	/if (${petTaunt}) {
		/pet taunt on
	} else {
		/pet taunt off
	}
/if (${Debug} || ${Debug_Pets}) /echo <== summonPet -|
/RETURN



#EVENT suspendMinion "<#1#> Suspend Pets#*#"
SUB event_suspendMinion(live, ChatSender)
/if (${Debug} || ${Debug_Pets}) /echo |- EVENT_suspendMinion ==>

	|- -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
	
		/if (${Me.Pet.ID}) /chat ${channelNumber} Suspending my pet...
		
		/if (${Me.Casting.ID}) /call interrupt
		
		/if (${Me.AltAbilityReady[Suspended Minion]}) {
			/call thf_Casting 0 "Suspended Minion"
			/delay 40 !${Me.Pet.ID}
			/varset currently_Suspended TRUE
		}
		/if (${Me.Pet.ID}) /pet get lost
		/if (${Defined[suspend_Pets]}) /varset suspend_Pets TRUE
	}

/if (${Debug} || ${Debug_Pets}) /echo <== EVENT_suspendMinion -|
/RETURN



#EVENT returnMinion "<#1#> return Pets#*#"
SUB event_returnMinion(line, ChatSender)
/if (${Debug} || ${Debug_Pets}) /echo |- EVENT_returnMinion ==>

	|- -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
		
		/if (${suspend_Pets}) {
			/chat ${channelNumber} Returning my pet...
			/if (${currently_Suspended}) {
				/call thf_Casting 0 "Suspended Minion"
				/varset currently_Suspended FALSE
			}
			/varset suspend_Pets FALSE
		}
	}
	
/if (${Debug} || ${Debug_Pets}) /echo <== EVENT_returnMinion -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|-|
|- Automatically shrinks pets.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|-|
SUB check_AutoShrink
/if (${Debug} || ${Debug_Pets}) /echo |- check_AutoShrink ==>
	
	/if (${Me.Pet.Height}>1) {
		/if (${FindItemCount[=Symbol of Ancient Summoning]}) {
			/call thf_Casting ${Me.Pet.ID} "Symbol of Ancient Summoning"
		} else /if (${Me.Book[Tiny Companion]}) {
			/call thf_Casting ${Me.Pet.ID} "Tiny Companion/giveuptimer|${If[${Me.Gem[Tiny Companion]},0,90]}"
		}
	}

/if (${Debug} || ${Debug_Pets}) /echo <== check_AutoShrink -|
/RETURN



|----------------------------|
|- Manually shrinks pets.	-|
|----------------------------|
|- Use, /bc Shrink Pets		-|
|----------------------------|
#EVENT shrinkPets "<#1#> Shrink Pets"
SUB event_shrinkPets(line, ChatSender)
/if (${Debug} || ${Debug_Pets}) /echo |- event_shrinkPets ==>
	
	|- -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
	
		|- Attempt to shrink pets.
		/if (${Me.Pet.ID} && ${Me.Pet.Height}>1 && ${Me.Book[Tiny Companion]}) /call thf_Casting ${Me.Pet.ID} "Tiny Companion/GiveUpTimer|20s"
	}

/if (${Debug} || ${Debug_Pets}) /echo <== event_shrinkPets -|	
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks pet health, and attempts to cast PetHeal spells.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_petHeal
/if (${Debug} || ${Debug_Pets}) /echo |- PetHeal ==>

	/if (${Defined[petHeal]} && ${Me.Pet.ID}) {
		
		/declare castName string local ${petHeal.Arg[1,/]}
		/declare healPct int local
		/declare gemNumber int local ${DefaultGem}

		|- Check Mend Companion AA.
		/if (${Me.AltAbilityReady[Mend Companion]} && ${Me.Pet.PctHPs} < ${petMendPct}) {
			/varset petMended TRUE
			/call thf_Casting ${Me.Pet.ID} "Mend Companion"
		}
		
		/if (!${petMended}) {
			|- Check Pet Health and cast heal spell.
			/if (${petHeal.Find[/HealPct|]}) {
				/call argueString HealPct "${petHeal}"
				/varset healPct ${Macro.Return}
			}
			
			|- Check Pet Health and cast heal spell.
			/if (${petHeal.Find[/gem|]}) {
				/call argueString gem "${petHeal}"
				/varset gemNumber ${Macro.Return}
			}
		}
		/if (${Me.Pet.PctHPs} < ${healPct}) /call thf_Casting ${Me.Pet.ID} "${castName}/GiveUpTimer|${If[${Me.Gem[${Spell[${Pet_Heal.Arg[1,/]}]}]},15,90]}"
	}

/if (${Debug} || ${Debug_Pets}) /echo <== PetHeal -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Rebuff pet.															-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Casts spells, items, and alternate abilities, from MyPetBuffs array.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB buffPet
/if (${Debug} || ${Debug_Pets}) /echo |- buffPet ==>

	|- -Declare local variables.
	/declare castName string local
	/declare buffName string local
	/declare i int local
	
	/for i 1 to ${petBuffs.Size}
	
	
		/varset castName ${petBuffs[${i}].Arg[1,/]}
		/varset buffName ${If[${Me.Book[${castName}]} || ${Me.AltAbility[${castName}]},${castName},${FindItem[${castName}].Spell}]}
		
		|-  /BuffName Check.
		/if (${petBuffs[${i}].Find[/buffname|]}) {
			/call argueString buffname "${petBuffs[${i}]}"
			/varset buffName ${Macro.Return}
		}
		
		|- Check if the buff will stack
		/if (!${Spell[${buffName}].StacksPet}) {
			/if (${Debug} || ${Debug_Pets}) /echo |- buffPet -|- [${castName}] will not stack, skipping.
		} else {
			
			|- Check if the pet is missing the buff
			/if (!${Me.PetBuff[${buffName}]}) /call thf_Casting ${Me.Pet.ID} "${petBuffs[${i}]}/GiveUpTimer|${If[${Me.Book[${castName}]} && !${Me.Gem[${castName}]},90,0]}"
		}

	/if (!${outerActionTaken}) /next i

/if (${Debug} || ${Debug_Pets}) /echo <== buffPet -|
/RETURN



SUB pet_Setup
/call iniToVar "${iniGeneral},Debug,Debug Pets (On/Off)" Debug_Pets bool outer
/if (${Debug} || ${Debug_Pets}) /echo |- pet_Setup ==>

	/if (${Ini[${iniToon},Pets].Length}) {
		/if (${Ini[${iniToon},Pets,Pet Spell].Length}) /call iniToVar "${iniToon},Pets,Pet Spell" petSpell string outer
		/if (${Ini[${iniToon},Pets,Pet Taunt (On/Off)].Length}) /call iniToVar "${iniToon},Pets,Pet Taunt (On/Off)" petTaunt bool outer
		/if (${Defined[petSpell]}) {
			/declare Pets bool outer TRUE
			/if (!${Defined[suspend_Pets]}) /declare suspend_Pets bool global FALSE
			/if (!${Defined[currently_Suspended]}) /declare currently_Suspended bool global FALSE
			
			/if (${Ini[${iniToon},Pets,Pet Auto-Shrink (On/Off)].Length}) /call iniToVar "${iniToon},Pets,Pet Auto-Shrink (On/Off)" petAutoShrink bool outer	
			/if (${Ini[${iniToon},Pets,Pet Heal].Length}) /call iniToVar "${iniToon},Pets,Pet Heal" petHeal string outer
			/if (${Ini[${iniToon},Pets,Pet Mend (Pct)].Length}) /call iniToVar "${iniToon},Pets,Pet Mend (Pct)" petMendPct int outer
			/if (${Ini[${iniToon},Pets,Pet Buff#1].Length}) {
				/call iniToArray "${iniToon},Pets,Pet Buff#" petBuffs
				/declare pet_outerRebuffTimer timer outer
			}
		}
	}

/if (${Debug} || ${Debug_Pets}) /echo <== pet_Setup -|
/RETURN



SUB pet_Background_Events

	/doevents suspendMinion

/RETURN



SUB pet_MacroSettings
	/if (${Debug}) /echo |- pet_MacroSettings ==>

	/call writeToIni "${iniGeneral},Debug,Debug Pets (On/Off)" Off

	/if (${Debug}) /echo <== pet_MacroSettings -|
/RETURN



SUB pet_CharacterSettings
/if (${Debug} || ${Debug_Pets}) /echo |- pet_CharacterSettings ==>

	/if (${Select[${Me.Class.ShortName},BST,ENC,MAG,NEC,SHD,SHM]}) {	
		/call writeToIni "${iniToon},Pets,Pet Spell"
		/call writeToIni "${iniToon},Pets,Pet Taunt (On/Off)"
		/call writeToIni "${iniToon},Pets,Pet Auto-Shrink (On/Off)"
		/call writeToIni "${iniToon},Pets,Pet Heal"
		/call writeToIni "${iniToon},Pets,Pet Mend (Pct)"
		/call writeToIni "${iniToon},Pets,Pet Buff#1"
	}

/if (${Debug} || ${Debug_Pets}) /echo <== pet_CharacterSettings -|
/RETURN



SUB pet_Aliases
/if (${Debug} || ${Debug_Pets}) /echo |- pet_Aliases ==>

	/squelch /alias /SuspendPets /bc Suspend Pets
	/squelch /alias /SusPets /bc Suspend Pets
	/squelch /alias /RETURNPets /bc Return Pets
	/squelch /alias /RetPets /bc Return Pets
	/squelch /alias /ShrinkPets /bc Shrink Pets

/if (${Debug} || ${Debug_Pets}) /echo <== pet_Aliases -|
/RETURN