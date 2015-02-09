|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Background.inc
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains background functions that update e3 status and variables,
|- along with many minimally invasive macro functions.
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Updates zone related variables, and safeZone via 'SUB check_ZoneType'.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Zone
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Zone ==>

	|- Set current zone ID
	/varset outerCurrentZone ${Zone.ID}

	|- Update macro timers
	/varset globalIdleTimer ${globalouterIdleInterval}m
	/varset outerRebuffTimer 0
	
	|- Turn off assist / attack
	/if (${outerAssisting} || ${Me.Combat}) /call AssistOff
	
	|- Load Spellset
	/if (${Me.Class.CanCast} && ${Defined[Default_SpellSet]}) /call check_Gems

	|- Reset badSpells list
	/varset badSpells
	
	|- Check Auto-Melody
	/if (${Defined[autoMelody]} && !${songSet.Length}) /playmelody ${autoMelody} /only|Me

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Zone -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to load the spellset specified in your Macro_Ini, if you're missing more than 3 gems on your spellbar.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Gems
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Gems ==>

	|- Count Empty Spell Gems
	/declare i int local
	/declare spellCount int local
	/for i 1 to 9
		/if (${Select[${Me.Gem[${i}]},NULL]}) /varcalc spellCount ${spellCount}+1
	/next i
	
	|- If more than 3 are empty
	/if (${spellCount} > 3) {
		/declare spellsetMem_Timer timer local 300
		/chat ${channelNumber} Loading spells...
		/ssm ${Default_SpellSet}
		:Memloop
		/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Gems -|- :Memloop
		
		/call check_BackgroundEvents
		/if (${outerDebug} || ${outerDebug_Background}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
		/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
		/if (${Me.Sitting}) /stand	
		/chat ${channelNumber} Spell set loaded !
	}
	
	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Gems -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Reports bots who have fallen behind, and attempts to re-acquire follow when it breaks.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Follow
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Follow ==>

	/if (${globalFollowTarget.Equal[${Me.CleanName}]}) {
		/if (!${outerSpamTimer}) {
			/declare i int local 1
			:NextFollower
			
			/if (!${outerSpamTimer_${globalFollowers.Arg[${i},,]}}) {
				/if (${SpawnCount[pc ${globalFollowers.Arg[${i},,]}]} && !${Spawn[pc ${globalFollowers.Arg[${i},,]} radius ${LeashLength}].ID}) {
					/popup ${globalFollowers.Arg[${i},,]} has fallen behind.
					/call createTimer outerSpamTimer_${globalFollowers.Arg[${i},,]} 100
					/varset outerSpamTimer 10
				}
			}

			/varcalc i ${i} + 1
			/if (${globalFollowers.Arg[${i},,].Length}) /goto :NextFollower
		}
	} else {

		/if (${SpawnCount[pc ${globalFollowTarget}]} && ${Spawn[pc ${globalFollowTarget}].Distance} < ${LeashLength} && ${Spawn[pc ${globalFollowTarget}].LineOfSight}) {
			/if (!${Stick.Active}) {
				/call AcquireFollow
			}
			/if (${Me.FeetWet}) /squelch /face fast ${globalFollowTarget}
		}
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Follow -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks to see if your character is idle.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Updates globalIdleTimer to the outerIdleInterval set in your Macro_Ini, whenever you've recently been active, or in combat.
|- Sets idle=TRUE if you're in a safeZone, or when you have been inactive for longer than the outerIdleInterval.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Idle
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Idle ==>
	
	|- Check to see if I have been active recently.  If so, set globalIdle false, and update the globalIdleTimer	
	/if (${outerActiveTimer} || ${outerCombatTimer}) {
		/varset globalIdleTimer ${globalouterIdleInterval}m
		/if (${globalIdle}) /varset globalIdle FALSE
	} else {
		|- If I haven't been active, and the globalIdleTimer is spent, set globalIdle TRUE.
		/if (!${globalIdle} && !${globalglobalIdleTimer}) /varset globalIdle TRUE
	}
	
	/if (${outerDebug}) {
		/echo globalIdleTimer = [${globalglobalIdleTimer}] || globalIdle = [${globalIdle}]
		/echo <== check_Idle -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks to see if your character is active.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Pauses the macro while you're moving, or have a corpse open.
|- Pauses the macro while you have items on your cursor, and clears them after 30 seconds.
|- Pauses the macro when you manually cast spells.
|- Automatically closes your spellbook, if its been open for more than 30 seconds.
|- Announces in /bc when you're feigning death.
|- Pauses the macro when you're trading / banking.
|- Automatically turns off combat when attacking PCs or when you have no target.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Active
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Active ==>

	|- Check Zoning
	/if (!${SpawnCount[pc ${Me}]}) {
		/varset outerActiveTimer 15
		/varset outerRebuffTimer 0
	}
	
	|- Check Movement
	/if (${Me.Moving} || ${Corpse.Open}) {
		/varset outerActiveTimer 5
		/varset outerRebuffTimer ${outerRebuffDelay}
	}

	|- Check Cursor
	/if (${Cursor.ID}) {
		|-- Destroy outerBadItems.
		/if (${Defined[outerBadItems]}) {
			/if (${Select[${Cursor},${outerBadItems}]} || ${Select[${Cursor.ID},${outerBadItems}]}) {
				/echo Destroying ${Cursor}...
				/destroy
			}
		}

		|-- Check to see if I'm waiting to drop this item, or if I've picked up something new
		/if (${outerOldCursorID} != ${Cursor.ID}) {
			/varset outerCursorTimer ${Math.Calc[${outerInventoryTimer} * 10].Int}
			/varset outerOldCursorID ${Cursor.ID}
		} else {
			/if (!${outerCursorTimer}) /call clearCursor
		}
		
		/varset outerActiveTimer 5
		/if (${outerRebuffTimer} < 30) /varset outerRebuffTimer 30
		
	} else {
		/if (${outerOldCursorID}) {
			/varset outerCursorTimer 0
			/varset outerOldCursorID 0
		}
	}
	
	|- Check AutoAttack
	/if (${Me.Combat}) {
		/if (!${Target.ID} || ${Target.Type.Equal[corpse]} && !${outerAssisting}|| ${Spawn[${Target.ID}].Type.Equal[PC]}) /attack off
	}
	
	|- Check Casting
	/if (${Me.Class.ShortName.NotEqual[BRD]} && ${Me.Casting.ID}) {
		/call check_Castings
		/varset outerActiveTimer 5
		/varset outerRebuffTimer ${outerRebuffDelay}
	}
	
	/if (${Window[BigBankWnd].Open} || ${Window[MerchantWnd].Open} || ${Window[TradeWnd].Open} || ${Window[GuildBankWnd].Open} || ${Window[MerchantWnd].Open}) {
		/varset outerActiveTimer 30
		/varset outerRebuffTimer ${outerRebuffDelay}
		/doevents flush
	}
	
	|- Check spellbook
	/if (${Window[SpellbookWnd].Open}) {
		/if (!${spellbook_Timer}) {
			|-- if the timer isn't defined, make one.
			/if (!${Defined[spellbook_Timer]}) {
				/declare spellbook_Timer timer outer ${outerSpellbookTimer}s
			} else {
				/if (${Window[SpellbookWnd].Open}) /stand
			}
		}
		
		/varset outerActiveTimer 30
		/varset outerRebuffTimer ${outerRebuffDelay}
		
	} else {
		/if (${Defined[spellbook_Timer]}) /deletevar spellbook_Timer
	}
	
	|- Check Feigning
	/if (${Me.Feigning}) {
		/if (!${Select[${Me.Class.ShortName},NEC,SHD,MNK]}) {
			/stand
			/if (${outerAssisting} && ${StickOn}) /attack on
		} else {
			/if (!${Defined[feign_Timer]}) {
				/chat ${channelNumber} CURRENTLY IN 'FEIGN DEATH' !
				/declare feign_Timer timer outer 15s
			}
		}
		
		/varset outerActiveTimer 30
		/varset outerRebuffTimer ${outerRebuffDelay}
		
	} else {
		/if (${Defined[feign_Timer]}) /deletevar feign_Timer
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Active -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Zoning
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Zoning ==>

	/varset outerActiveTimer 15
	/varset outerRebuffTimer 0

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Zoning -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Movement
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Movement ==>

	/varset outerActiveTimer 5
	/varset outerRebuffTimer ${outerRebuffDelay}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Movement -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_AutoAttack
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_AutoAttack ==>

	/if (!${Target.ID} || ${Target.Type.Equal[corpse]} && !${outerAssisting}|| ${Spawn[${Target.ID}].Type.Equal[PC]} && !${Toggle-PvP}) /attack off

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_AutoAttack -|
/RETURN	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Casting
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Casting ==>

	/call check_Castings
	/varset outerActiveTimer 5
	/varset outerRebuffTimer ${outerRebuffDelay}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Casting -|
/RETURN	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_MerchantWnd
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_MerchantWnd ==>

	/varset outerActiveTimer 30
	/varset outerRebuffTimer ${outerRebuffDelay}
	/doevents flush

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_MerchantWnd -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Cursor
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Cursor ==>

	/if (${Cursor.ID}) {
	
		|- Destroy outerBadItems.
		/if (${Defined[outerBadItems]}) {
			/if (${Select[${Cursor},${outerBadItems}]} || ${Select[${Cursor.ID},${outerBadItems}]}) {
				/echo Destroying ${Cursor}...
				/destroy
			}
		}

		|- Check to see if I'm waiting to drop this item, or if I've picked up something new
		/if (${outerOldCursorID} != ${Cursor.ID}) {
			/varset outerCursorTimer ${Math.Calc[${outerInventoryTimer} * 10].Int}
			/varset outerOldCursorID ${Cursor.ID}
		} else {
			/if (!${outerCursorTimer}) /call clearCursor
		}
		
		/varset outerActiveTimer 5
		/if (${outerRebuffTimer} < 30) /varset outerRebuffTimer 30
		
	} else {
		/if (${outerOldCursorID}) {
			/varset outerCursorTimer 0
			/varset outerOldCursorID 0
		}
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Cursor -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Spellbook
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Spellbook ==>

	/if (${Window[SpellbookWnd].Open}) {
		/if (!${spellbook_Timer}) {
		
			|- if the timer isn't defined, make one.
			/if (!${Defined[spellbook_Timer]}) {
				/declare spellbook_Timer timer outer ${outerSpellbookTimer}s
			} else {
				/if (${Window[SpellbookWnd].Open}) /stand
			}
		}
		
		/varset outerActiveTimer 30
		/varset outerRebuffTimer ${outerRebuffDelay}
		
	} else {
		/if (${Defined[spellbook_Timer]}) /deletevar spellbook_Timer
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Spellbook -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Feigning
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Feigning ==>

	/if (${Me.Feigning}) {
		/if (!${Select[${Me.Class.ShortName},NEC,SHD,MNK]}) {
			/stand
			/if (${outerAssisting} && ${StickOn}) /attack on
		} else {
		
			/if (!${Defined[feign_Timer]}) {
				/chat ${channelNumber} I AM FEIGNING !
				/declare feign_Timer timer outer 15s
			}
		}
		
		/varset outerActiveTimer 30
		/varset outerRebuffTimer ${outerRebuffDelay}
		
	} else {
		/if (${Defined[feign_Timer]}) /deletevar feign_Timer
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== check_Feigning -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Updates the outerCombatTimer whenever you, or someone in your party, is Assisting, or in combat.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Sets outerCombatTimer to the outerCombatDelay whenever combat is detected.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Combat

	|- Check NetBots to see if anyone is in combat.
	/declare i int local
	/declare localNetBot string local
	/for i 1 to ${NetBots.Counts}
		/varset localNetBot ${NetBots.Client.Arg[${i}, ]}
		/if (${SpawnCount[pc ${localNetBot}]} && ${Spawn[pc ${localNetBot}].Distance} < ${MaxResponseDist} && ${NetBots[${localNetBot}].Attacking} || ${Me.Combat} || ${outerAssisting}) {
			/if (${outerDebug} || ${outerDebug_Background}) /echo |- check_Combat -|- ${NetBots[${localNetBot}]} is attacking, setting outerCombatTimer

			|- Update macro timers.
			/varset outerCombatTimer ${outerCombatDelay}
		}
	/if (!${outerCombatTimer}) /next i

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Completes item casting exchanges, after a sufficient time has passed.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB completePendingExchange
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- completePendingExchanges ==>
	
	|- Check for the item needing to be swapped back.
	/if (!${FindItemCount[${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[1,/]}]}) {
		/if (${outerDebug} || ${outerDebug_Background}) /echo Could not find [${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[1,/]}], skipping swapback.
	} else {
	
		|- Set a retry timer in case the first attempt fails
		/declare swapRetryTimer timer local 30
		
		:SwapBackLoop
		/if (${outerDebug} || ${outerDebug_Background}) /echo |- thfSetup -|- :SwapBackLoop
		
		|- Try to swap the item
		/call SwapItem "${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[1,/]}" ${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[2,/]}
	
		|- If the if item was not returned to it's correct slot
		/if (!${InvSlot[${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[2,/]}].Item.Name.Equal[${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[1,/]}]}) {
			
			|- If I have time to try again, goto SwapBackLoop.  If not, announce to user.
			/if (${retryTimer}) {
				/goto :SwapBackLoop
			} else {
				/echo I have failed to swap [${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange].Arg[1,/]}] back.
			}
		} else {
			|- If the item was returned to it's correct slot
			/varset swapItemBack FALSE
			/call writeToIni "${iniTHF},${MacroQuest.Server}-${Me.CleanName}-${Me.Class.ShortName},Pending Exchange" NONE 1
		}
	}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== completePendingExchanges -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB background_Setup
	/call iniToVar "${iniGeneral},Debug,Debug Background (On/Off)" outerDebug_Background bool outer
	
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- background_Setup ==>

	|- Create outer variables.
	/if (!${Defined[globalIdle]}) /declare globalIdle bool global
	/if (!${Defined[globalIdleTimer]}) /declare globalIdleTimer timer global
	
	/declare outerActionTaken bool outer FALSE
	/declare outerSpamTimer timer outer
	
	/declare outerActiveTimer timer outer
	
	/declare outerCombatTimer timer outer
	/declare outerCombatDelay int outer 100
	
	/declare outerCursorTimer timer outer
	/declare outerOldCursorID int outer 0
	
	/declare outerRebuffTimer timer outer
	/declare outerRebuffDelay int outer 100
	
	/declare outerCurrentZone int outer 0
	
	|- Import Ini Variables.
	
	|- Clear Cursor Timer
	/if (${Ini[${iniGeneral},Background,Auto-Inventory Timer (Sec)].Length}) {
		/call iniToVar "${iniGeneral},Background,Auto-Inventory Timer (Sec)" outerInventoryTimer string outer
	} else {
		/echo Warning: Could not find a setting for [Auto-Inventory Timer] in [${iniGeneral}].  [Auto-Inventory Timer] has been set to 30 seconds.
		/declare outerInventoryTimer int outer 30
	}
	
	|- Close Spellbook Timer
	/if (${Ini[${iniGeneral},Background,Close Spellbook Timer (Sec)].Length}) {
		/call iniToVar "${iniGeneral},Background,Close Spellbook Timer (Sec)" outerSpellbookTimer string outer
	} else {
		/echo Warning: Could not find a setting for [Close Spellbook Timer] in [${iniGeneral}].  [Close Spellbook Timer] has been set to 1 minute.
		/declare outerSpellbookTimer int outer 60
	}
	
	|- globalIdle Timer
	/if (${Ini[${iniGeneral},Background,Idle Time Out (Min)].Length}) {
		/call iniToVar "${iniGeneral},Background,Idle Time Out (Min)" outerIdleInterval int outer
	} else {
		/echo Warning: Could not find a setting for [Idle Time Out] in [${iniGeneral}].  [Idel Time Out] has been set to 10 minutes.
		/declare outerIdleInterval int outer 10
	}

	|- Auto-Destroy Items
	/if (${Ini[${iniGeneral},Background,Auto-Destroy List].Length}) /call iniToVar "${iniGeneral},Background,Auto-Destroy List" outerBadItems string outer
	
	|- Import NetBots HUD array
	/call iniToArray "${iniGeneral},NetBots HUD,NetBot#" netBotsHUDArray

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== background_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB background_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB background_MacroSettings
	/if (${outerDebug}) /echo |- background_MacroSettings ==>

	/call writeToIni "${iniGeneral},Debug,Debug Background (On/Off)" Off

	/call writeToIni "${iniGeneral},Background,Auto-Destroy List"
	/call writeToIni "${iniGeneral},Background,Idle Time Out (Min)" 10
	/call writeToIni "${iniGeneral},Background,Auto-Inventory Timer (Sec)" 30
	/call writeToIni "${iniGeneral},Background,Close Spellbook Timer (Sec)" 30
	
	/if (${outerDebug}) /echo <== background_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB background_CharacterSettings
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- background_CharacterSettings ==>

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== background_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB background_Aliases
	/if (${outerDebug} || ${outerDebug_Background}) /echo |- background_Aliases ==>

	/noparse /squelch /alias /AddAutoDestroy /echo Add Auto-Destro item ${Cursor.Name}
	/noparse /squelch /alias /RemoveAutoDestroy /echo Remove Auto-Destro item ${Cursor.Name}

	/if (${outerDebug} || ${outerDebug_Background}) /echo <== background_Aliases -|
/RETURN