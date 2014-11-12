|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- thf_Assists.inc
|- Written by Conradd of THF
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains assisting related functions for thf.mac.
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Engages assist functions on ChatSender, and bots who are in the same zone.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, 'Assist on ${Target.ID}' in /bc.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Assist "<#1#> #2# assist on #3#"
SUB EVENT_Assist(String line, String from, String who, Int mobID)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_Assist ==>
	
	/call amIConcerned ${from} ${who} TRUE
	/if (${Macro.Return}) {
		/if (${Defined[PauseTarget]}) /deletevar PauseTarget
	
		| Check that you're not assisting on a corpse, pet or pc
		/if (${Spawn[id ${mobID}].Type.Equal[Corpse]} || !${Select[${Spawn[id ${mobID}].Type},${outerAcceptableTargetTypes}]}) {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo [${Spawn[id ${MobID}].CleanName}] is not an acceptable TargetType.
		} else {
			| Check target distance.
			/if (${Spawn[${mobID}].Distance} > ${outerMaxAssistDistance}) {
				/chat ${channel} ${Spawn[${mobID}].CleanName} is too far away.
			} else {
				| Wait to catch up to group, before engage.
				/if (${Following} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Me.Moving}) {
					/declare Assist_Moving_Timer timer local 100
					:movetoAssist_Loop
					/call checkBackgroundEvents
					/if (${Spawn[=${FollowTarget}].Distance} > ${LeashLength} && ${Assist_Moving_Timer}) /goto :movetoAssist_Loop
				}

				| Disengage follow.
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}) /squelch /netfollow pause
				
				| Set Assist Variables.
				/varset outerAssisting 		TRUE
				/varset outerAssistTarget 	${mobID}
				/varset outerHasTanked 		FALSE
				
				| If I am the assist caller, or I am feigning, don't allow macro control.
				/if (${from.Equal[${Me.CleanName}]} || ${Me.Feigning}) {
					/varset outerAllowControl FALSE
				} else {
					/varset outerAllowControl TRUE
				}
				
				| If the macro is controlling me, engage.
				/if (${outerAllowControl}) {
					/chat ${channel} Attacking - ${outerAssistTarget} - ${Spawn[${outerAssistTarget}].CleanName}
					| Target AssistTarget.
					/if (${Target.ID} != ${outerAssistTarget}) /call TrueTarget ${outerAssistTarget}
					/call amIMelee
					/if (${Macro.Return}) {
						/attack on
						/call getAssistStickDistance
						/call createTimer getCloserTimer 70
						/call stickToAssistTarget
					} else /if (${outerAssistType.Equal[ranged]}) {
						/autofire on
						/squelch /stick hold moveback ${outerRangedStickDistance}
					}
					
					| Send in pets.
					/if (${Me.Pet.ID}) {
						/if (${Target.ID} != ${outerAssistTarget}) /call TrueTarget ${outerAssistTarget}
						/delay 3
						/if (${outerDebug} || ${outerDebug_Assists}) /chat ${channel} Pet Check: Sending pet on - ${outerAssistTarget} - ${Spawn[${outerAssistTarget}].CleanName}
						/pet attack
					} else {
						/call amIPetClass
						/if (${Macro.Return}) {
							/chat ${channel} NO pet to send !
						}
					}
				}
				
				/varset combatTimer ${combatDelay}
				/if (${outerDebug} || ${outerDebug_Assists}) /echo Assist=${outerAssisting} || AssistTarget=${outerAssistTarget} || AllowControl=${outerAllowControl}
			}
		}
	}
	
	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_Assist -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Backs off an AssistTarget, and labels them as a 'PauseTarget'.
|- You must manually call engage to re-engage a PauseTarget.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, '/backoff', or, 'Back off' in /bc.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT BackOff "<#1#> Back Off"
SUB EVENT_BackOff(String line, String from)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_BackOff ==>

	|- Verify the event
	/call amIConcerned ${from} ${Me.CleanName} TRUE
	/if (${Macro.Return}) {
		/if (${outerAssisting} && ${outerAllowControl}) /chat ${channel} Backing off.
		/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${outerAssistTarget}
		/if (${Me.Pet.ID}) /pet back off
		/if (${Me.Combat}) /attack off
		/if (${Me.AutoFire}) /autofire off
		/if (${Debuff_Targets.Size}) /deletevar Debuff_Targets
		/if (${DoT_Targets.Size}) /deletevar DoT_Targets
		/delay 30 !${Me.Casting.ID} && !${Me.Combat} && !${Me.AutoFire}
		/call AssistOff
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_BackOff -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Turns off Assist_Functions, and resets assisting variables.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB AssistOff
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- AssistOff -| ==>
	
	| Interrupt spells, turn off attack.
	/if (${outerAllowControl}) {
		/if (${Me.Casting.ID} && !${Spell[${Me.Casting}].SpellType.Find[beneficial]} && ${Me.Class.ShortName.NotEqual[BRD]}) /stopcast
		/if (${Me.Combat}) /attack off
		/if (${Me.AutoFire})  /autofire off
	}
	
	| Reset assist variables.
	/varset outerAssisting 		FALSE
	/varset outerAssistTarget 	0
	/varset outerAllowControl 	FALSE
	/varset outerHasTanked 		FALSE
	/varset outerEnraged 		FALSE
	/varset outerEnrageAttack 	FALSE
	
	| Reset Burns
	/if (${Defined[use_QuickBurns]}) /deletevar use_QuickBurns
	/if (${Defined[use_FullBurns]}) /deletevar use_FullBurns
	
	| Check loot, and reacquire follow.
	/if (${Select[${Stick},ON]}) /squelch /stick off
	/if (${Following}) /call AquireFollow
	
	/if (${outerDebug}) {
		/echo Assisting: [${outerAssisting}] || AssistTarget: [${outerAssistTarget}]
		/echo |- AssistOff -| <==
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Calculates a stick distance to your AssistTarget.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB getAssistStickDistance
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- getAssistStickDistance ==>
	
	/if (${outerAssistStickDistance} == 0) {
		/varset outerAssistStickDistance ${Math.Calc[${Spawn[${Target.ID}].MaxRangeTo}*.9].Int}
		| Check MaxRangeTo, make sure it is not out of bounds.
		/if (${outerAssistStickDistance} > 25) /varset outerAssistStickDistance 25
	}

	/if (${outerDebug}) {
		/echo AssistStickDistance = [${outerAssistStickDistance}]
		/echo <== getAssistStickDistance -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Engages /stick on an AssistTarget.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB stickToAssistTarget
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- stickToAssistTarget ==>

	/if (!${outerHasTanked}) {
		/if (${outerAssistStickPoint.Equal[behind]}) {
			
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attempting to stick behind [${outerAssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 15
			/squelch /stick hold moveback behind ${outerAssistStickDistance} uw	
			/delay 10 ${Me.Moving}
			
		} else /if (${outerAssistStickPoint.Equal[behindonce]}) {
		
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attempting to stick behindonce [${outerAssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch /stick hold moveback behindonce ${outerAssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${outerAssistStickPoint.Equal[pin]}) {
		
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attempting to stick pin [${outerAssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch /stick hold moveback pin ${outerAssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${outerAssistStickPoint.Equal[front]}) {
		
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attempting to stick front [${outerAssistStickDistance}].
			/squelch /stick hold front ${outerAssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${outerAssistStickPoint.Equal[!front]}) {
			
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attempting to stick !front [${outerAssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch stick hold moveback !front ${outerAssistStickDistance} uw
			/delay 10 ${Me.Moving}
		} else {

			/if (${outerDebug} || ${outerDebug_Assists}) /echo [${outerAssistStickPoint}] is not a valid stick point.  Defaulting to stick behind.
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 15
			/squelch /stick hold moveback behind ${outerAssistStickDistance} uw	
			/delay 10 ${Me.Moving}
		}
	}
	
	/declare miscTimer timer local 50
	:waitToStop
	/if (${outerDebug} && ${spamTimer_waitToStop}) {
		/echo |- stickToAssistTarget -| :waitToStop
		/call createTimer spamTimer_waitToStop 30
	}

	/call checkBackgroundEvents
	
	/if (${Me.Moving}) {
		/if (${miscTimer}) /goto :waitToStop
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== stickToAssistTarget -|	
/RETURN

|----------------------------------------------------|
|- Checks range and combat status while assisting.	-|
|----------------------------------------------------|
SUB checkAssistStatus
	/if (${outerDebug} || ${outerDebug_Assists}) {
		/echo |- checkAssistStatus ==>
		/echo AssistTarget = [${outerAssistTarget}] || outerAllowControl = [${outerAllowControl}] || Distance check = [${Spawn[id ${outerAssistTarget}].Distance} < ${outerMaxAssistDistance}] || TargetType = [${Spawn[id ${outerAssistTarget}].Type}]
	}
	
	| Check range.
	/if (${Spawn[${outerAssistTarget}].Distance} > ${outerMaxAssistDistance}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo [${Spawn[${outerAssistTarget}].CleanName} - ${Spawn[${outerAssistTarget}].ID}] is out of range,  holding assist functions.
	} else {
		| Check feigning
		/if (${Me.Feigning}) {
			/if (${Me.Class.ShortName.Equal[MNK]}) {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo I am feigned, holding assist functions. 
			} else {
				/stand
			}
		} else  {
			| Check AllowControl bool.
			/if (!${outerAllowControl}) {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo Macro control is not allowed.  Assist functions are restricted. 
				| Check enrage when enrageAttack is on
				/if (${outerEnraged} && ${outerEnrageAttack} && !${Stick.Behind}) /attack off
			} else {
				| Check Target.
				/if (${Target.ID}!=${outerAssistTarget} && ${Target.PctHPs} && !${Spawn[${outerAssistTarget}].Type.Equal[Corpse]}) /call TrueTarget ${outerAssistTarget}
				|- Melee functions
				/if (${outerAssistType.Equal[Melee]}) {
					/if (!${Me.AutoFire}) {
						| Check Target of Target, and who is tanking
						/if (!${outerHasTanked}) {
							/if (${Me.TargetOfTarget.CleanName.Equal[${Me.CleanName}]} && !${Bool[${Int[${Target.Speed}]}]}) {	
								/if (${outerDebug} || ${outerDebug_Assists}) /echo Setting hasTanked to TRUE, sticking without position.
								/varset outerHasTanked TRUE
								/stick off
								/if (${Target.Distance} > ${outerAssistStickDistance}) /stick hold moveback ${outerAssistStickDistance}
							}
						} else {
							| If the mob is not targeting me, or is moving
							/if (!${Me.TargetOfTarget.CleanName.Equal[${Me.CleanName}]} || ${Target.Speed}) {
								/if (${outerDebug} || ${outerDebug_Assists}) /echo I've tanked, but mob is no longer targeting me or is moving. Sticking normally.
								/varset outerHasTanked FALSE
								/if (${Target.Distance} > ${outerAssistStickDistance}) /call stickToAssistTarget
							}
						}
						
						| If mob is not enraged and I'm not attacking, start attacking.
						/if (!${outerEnraged}) {
							/if (!${Me.Combat}) /attack on
						| If mob is enraged
						} else {
							| If enrageAttack is FALSE turn off attack
							/if (!${outerEnrageAttack}) {
								/if (${Me.Combat}) /attack off
							} else {
								| Check if the mob is looking at me
								/call check_IsLookingAt
								| If the mob IS looking at me
								/if (${Macro.Return}) {
									/if (${outerDebug} || ${outerDebug_Assists}) /echo I am not behind the target. Stopping attack.
									/attack off
								} else /if ((!${Macro.Return} || ${Target.Speed}) && !${Me.Combat}) {
									/if (${outerDebug} || ${outerDebug_Assists}) /echo I am behind the target. Attacking.
									/attack on
								}
							}
						}
					}
					
				|- Ranged functions
				} else /if (${outerAssistType.Equal[Ranged]}) {
					/if (!${Me.Combat} && ${Target.Distance} < 35 || ${Target.Distance} > ${outerRangedStickDistance}) /squelch /stick hold moveback ${outerRangedStickDistance}
					/if (!${Me.AutoFire}) /autofire on
				}
			}
		}
		/varset combatTimer ${combatDelay}
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== checkAssistStatus -|
/RETURN

|----------------------------------------------------------------|
|- Checks to see if the AssistTarget has died, calls Assistoff.	-|
|----------------------------------------------------------------|
SUB checkAssistTarget
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- checkAssistTarget ==>
	
	/if (${Spawn[${outerAssistTarget}].Type.Equal[Corpse]} || ${Select[${Spawn[${outerAssistTarget}].Type.Equal[Corpse]},NULL]}) /call AssistOff
	/varset combatTimer ${combatDelay}
	
	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== checkAssistTarget -|
/RETURN

|--------------------------------------------------------|
|- This script was taken from,							-|
|- genbot.mac											-|
|- botcombat.inc										-|
|- Bot combat module.									-|
|- Version 13.4.0										-|
|- Date:07/08/2005										-|
|- Originally by ascii38, maintained by Mortefreddo.	-|
|- This script has been modified to work with thf.mac.	-|
|--------------------------------------------------------|
SUB check_IsLookingAt
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- CheckIsLookingAt ==>

	/declare MobHeading int local 0 
	/declare HeadingToPC int local 
	/declare DeltaX local float 
	/declare DeltaY local float 
	/declare HeadingDelta local float
	/declare FacingMe int local 0
	
	/varcalc MobHeading ${Spawn[id ${outerAssistTarget}].Heading.Degrees}
	/varcalc DeltaX ${Spawn[id ${Me.ID}].X}-${Spawn[id ${outerAssistTarget}].X}
	/varcalc DeltaY ${Spawn[id ${Me.ID}].Y}-${Spawn[id ${outerAssistTarget}].Y}
	
	/if (${DeltaX}>0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+270 
	} else /if (${DeltaX}<0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+90 
	} else {
		/if (${DeltaY}>0) {
			/varcalc HeadingToPC 90 
		} else {
			/varcalc HeadingToPC 270 
		}
	}

	/varcalc HeadingDelta ${Math.Abs[${HeadingToPC}-${MobHeading}]}
	
	/if (${HeadingDelta}<4 || ${HeadingDelta}>356) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo |- CheckIsLookingAt -| [${Spawn[id ${outerAssistTarget}].CleanName}] is facing me.
		/varset FacingMe 1
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== CheckIsLookingAt -|
/RETURN ${FacingMe}

#EVENT setEnrageAttack "[MQ2] Setting enrage attack on #1# to #2#"
#EVENT setEnrageAttack "[MQ2] Set enrage attack on #1# to #2#"
SUB EVENT_setEnrageAttack(String line, String npcName, String enrageSetting)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_setEnrageAttack ==>

	| Set enrage attack mode to enrageSetting (ON/OFF) in the Enrage Settings.ini file for npcName.
	/call writeToIni "${iniEnrage},${Zone.ShortName},${npcName}" "${enrageSetting}" TRUE
	/if (${outerDebug} || ${outerDebug_Assists}) /echo Attacking on enrage set to ${enrageSetting} for ${npcName}.

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_setEnrageAttack -|
/RETURN

|------------------------------------------------------------|
|- Turns off attack when a mob you're attacking enrages		-|
|- if that mob is specified in Enrage Settings.ini.         -|
|------------------------------------------------------------|
#EVENT EnrageOn "#1# has become ENRAGED."
SUB EVENT_EnrageOn(String line, String RageOn)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_EnrageOn ==>

	| Verify that the enraged npc is what I am currently fighting.
	/if (${Me.Combat} && ${Target.ID} == ${Spawn[${RageOn}].ID} && ${Target.PctHPs} < 20) {
		/varset enraged TRUE
		/if (${Me.Pet.ID}) /pet back off
		/if (${outerDebug} || ${outerDebug_Assists}) /echo enraged = ${outerEnraged}
		
		| Check if the entry for this NPC exists
		/if (!${Ini[${iniEnrage},${Zone.ShortName},${Target.CleanName}].Length}) {
			/echo ${Target.CleanName} is not in Enrage Settings.ini.  Defaulting to ON
			/call writeToIni "${iniEnrage},${Zone.ShortName},${RageOn}" "ON" 0
		}
		
		| If the entry is set to 'OFF'
		/if (!${Ini[${iniEnrage},${Zone.ShortName},${Target.CleanName}].Equal[ON]}) {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Attack through enrage is set to [${Ini[${iniEnrage},${Zone.ShortName},${Target.CleanName}]}] for ${Target.CleanName}.  Turning attack off.
			/attack off
		} else {
			| If the entry exists, and is set to ON, check position.
			/if (${outerDebug} || ${outerDebug_Assists}) /echo ${Target.CleanName} found in iniEnrage and set to ON. Checking position.
			/if (!${Stick.Behind}) {			
				/if (${outerDebug} || ${outerDebug_Assists}) /echo I am not behind the mob. Turning attack off.
				/attack off
			| Else, I am behind the mob. Keep attacking.
			} else {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo I am behind the target. Continuing to attack.
				/varset enrageAttack TRUE
			}
		}
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_EnrageOn -|
/RETURN

|------------------------------------------------|
|- Turns attack back on, after enrage is over.	-|
|------------------------------------------------|
#EVENT EnrageOff "#1# is no longer enraged."
SUB EVENT_EnrageOff(String line, String RageOn)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo Enrage OFF, turning attack ON
		
	/if (${outerEnraged}) {
		/if (${Target.ID} == ${Spawn[${RageOn}].ID}) {
			/attack on
			/if (${Me.Pet.ID}) /pet attack
			/varset enraged FALSE
		} else {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Not attacking enraged target. Doing nothing.
		}
	} else {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo I was not holding for enrage. Doing nothing.
	}

	/if (${outerEnrageAttack}) /varset enrageAttack FALSE

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_EnrageOff -|
/RETURN

#EVENT getCloser "Your target is too far away, get closer!"
SUB EVENT_getCloser
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_getCloser ==>

	/if (${outerAssisting} && ${outerAllowControl} && !${getCloserTimer}) {
		/call createTimer getCloserTimer 50
		
		| If the spawn has moved, stick back to the target
		/if (${Spawn[id ${outerAssistTarget}].Distance} <= ${AssistStickDistanceSetting} && ${AssistStickDistanceSetting} > 12) /varcalc AssistStickDistance ${outerAssistStickDistance} - 2
		/if (${outerDebug} || ${outerDebug_Assists}) /echo AssistStickDistance has been set to [${outerAssistStickDistance}]
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_getCloser -|
/RETURN

|------------------------------------------------|
|- Uses combat abilities, AAs, and disciplines.	-|
|------------------------------------------------|
SUB CombatAbilities
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- CombatAbilities ==>

	| Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
	} else {
		/declare abilityName string local
		/declare minEnd int local
		/declare i int local
	
		| For 1 to the size of MyAbilities array, use each ability.
		/for i 1 to ${MyAbilities.Size}
			/varset abilityName ${MyAbilities[${i}].Arg[1,/]}
			| Check for MinEnd
			/if (${MyAbilities[${i}].Find[/MinEnd|]}) {
				/call argueString MinEnd "${MyAbilities[${i}]}"
				/varset minEnd ${Macro.Return}
			} else {
				/varset minEnd 0
			}
	
			/if (${Me.PctEndurance} < ${minEnd}) {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo I don't have enough endurance to use [${abilityName}].
			} else {
				| Use ability. 
				/if (${Me.Ability[${abilityName}]}) {
					/if (!${Me.AbilityReady[${abilityName}]}) {
						/if (${outerDebug} || ${outerDebug_Assists}) /echo Ability [${abilityName}] is not ready.
					} else {
						| Use Backstab.
						/if (${abilityName.Equal[Backstab]}) {
							/call Attempt_Backstab
						| Use Bash.
						} else /if (${abilityName.Equal[Bash]}) {
							/call Attempt_Bash
						| Use Taunt.
						} else /if (${abilityName.Equal[Taunt]}) {
							|/if (${outerAutoAggro}) /call Attempt_Taunt
							/doability Taunt
						| Reset Tail Rake to Dragon Punch.  Tail Rake causes an error because it returns as "Dragon Punch" when /echoed in Me.Ability[#]
						} else /if (${abilityName.Equal[Tail Rake]}) {
							/varset ${MyAbilities[${i}]} Dragon Punch
						| Use a misc Ability.
						} else {
							/doability "${abilityName}"
						}
					}	
				| Use alternate ability
				} else /if (${Me.AltAbility[${abilityName}]}) {
					/if (!${Me.AltAbilityReady[${abilityName}]} || ${altTimer_${Me.AltAbility[${abilityName}].ID}}) {
						/if (${outerDebug} || ${outerDebug_Assists}) /echo Alternate Ability [${abilityName}] is not ready.
					} else {
						/call e3_Cast ${outerAssistTarget} "${abilityName}"
					}
				| Use combat ability (discipline)
				} else /if (${Me.CombatAbility[${abilityName}]}) {
					/if (!${Me.CombatAbilityReady[${abilityName}]}) {
						/if (${outerDebug} || ${outerDebug_Assists}) /echo Discipline [${abilityName}] is not ready.
					} else {
						| Rogue Assassin's Strike.
						/if (${Select[${abilityName},${rogueStrikes}]}) {
							/call check_rogStrike "${abilityName}"
						| Warrior provoke.
						} else /if (${Select[${abilityName},${warriorBellows}]}) {
							/if (${outerAutoAggro}) /call check_warProvoke "${abilityName}"
						| All other disciplines.
						} else {
							/call e3_Cast ${outerAssistTarget} "${abilityName}/Instant"
						}
					}
				| Use rogue evade as a combat ability 
				} else /if (${abilityName.Equal[Evade]}) {
					/if (${Me.Ability[Hide]}) /call AutoEvade
				}
			}
		/next i
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== CombatAbilities -|
/RETURN



SUB Attempt_Bash
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- Attempt_Bash ==>

	/if (!${Select[${Me.Inventory[Offhand].Type},Shield]} && !${Me.AltAbility[2 Hand Bash]}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo You must equip a shield, or train in the alternate ability "2 Hand Bash" to use the ability [Bash].
	} else {
		/doability Bash
	}
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== Attempt_Bash -|
/RETURN



SUB Attempt_Taunt
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- Attempt_Taunt ==> 

	/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
		| /if (${Me.Skill[Taunt]} < Find max skill formula) /goto :tauntAnyway
		/if (${outerDebug} || ${outerDebug_Assists}) /echo I currently have aggro on [${Target.CleanName}], skipping taunt.
	} else {
		:tauntAnyway
		/doability Taunt
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== Attempt_Taunt -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempt_Backstab
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Attempt_Backstab
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- Attempt_Backstab ==>

	| Check that I have a piercer in my main hand
	/if (${Me.Inventory[Mainhand].Type.NotEqual[Piercing]}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo I need a piercing weapon in my main hand to backstab...
	} else {
		
		| If backstab is ready, use it.
		/if (${Me.AbilityReady[Backstab]}) /doability Backstab
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== Attempt_Backstab -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Casts direct damage spells on a specified target.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Nukes
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- check_Nukes ==>

	/if (${MyNukes.Size} && ${outerAssisting} && ${use_Nukes}) {
		/if (${delayTimer_MyNukes}) {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo Waiting [${delayTimer_MyNukes}] to cast next nuke.
		} else {
			/if (${Target.ID} != ${outerAssistTarget}) {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo I am not targeting the assistTarget, holding nukes.
			} else {
				/declare castName string local
				/if (!${Defined[nukeCounter]}) {
					/declare nukeCounter int outer 1
				} else {
					/if (${nukeCounter} > ${MyNukes.Size}) /varset nukeCounter 1
				}
				
				/varset castName ${MyNukes[${nukeCounter}].Arg[1,/]}
				/call e3_Cast ${outerAssistTarget} "${MyNukes[${nukeCounter}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]} || ${Me.Combat},0,90]},]}"
				
				| If cast was a success, update 
				/if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[SUCCESS]}) {
					
					| /Delay| Check.
					/if (${MyNukes[${nukeCounter}].Find[/Delay|]}) {
						/call argueString Delay "${MyNukes[${nukeCounter}]}"
						/call createTimer "delayTimer_MyNukes" "${Int[${Macro.Return}]}0"
					}
					
					| Update nukeCounter
					/varcalc nukeCounter ${nukeCounter} + 1
				} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOTREADY]}) {
					| Update nukeCounter, skip to next nuke
					/varcalc nukeCounter ${nukeCounter} + 1
				}
			}
		}
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== check_Nukes -|
/RETURN


|---------------------------------------------------------------------------------------------------------------------------------------------------|
|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Debuffs
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- check_Debuffs ==>
	
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
	/if (${outerAssisting} && ${Assist_Debuffs.Size}) /call cast_longTermSpells "${outerAssistTarget}" "Assist_Debuffs"

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== check_Debuffs -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_DoTs
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- check_DoTs ==>
	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
	/if (${outerAssisting} && ${Assist_DoTs.Size}) /call cast_longTermSpells "${outerAssistTarget}" "Assist_DoTs"

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== check_DoTs -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Casts debuffs and dots on various assist targets
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB cast_longTermSpells(int spellTarget, string arrayName)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- cast_longTermSpells - [${arrayName}] ==>

	/declare castName string local
	/declare spellName string local
	
	/declare spellTimer string local
	/declare newTimerSetting int local
	
	/declare i int local
	
	/for i 1 to ${${arrayName}.Size}
	
		/varset castName ${${arrayName}[${i}].Arg[1,/]}
		/varset spellName ${If[${FindItemCount[=${castName}]},${FindItem[=${castName}].Spell},${castName}]}
		/varset spellTimer spellTimer_${spellTarget}_${arrayName}-${i}
		/varset newTimerSetting ${Math.Calc[${Spell[${spellName}].Duration} * 60].Int}
		
		/if (${${spellTimer}}) {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo |- cast_longTermSpells - [${arrayName}] -|	Waiting to recast [${castName}].
		} else {
		
			| cast the spell
			/call e3_Cast ${spellTarget} "${${arrayName}[${i}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]},3s,${If[${Me.Combat} || ${outerAssistType.Equal[melee]},0s,${Math.Calc[${Spell[${castName}].RecastTime} + 5].Int}s]}]},]}"
			
			| If the spell was resisted.
			/if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[RESISTED]}) {
				/if (${Target.ID}) /chat ${channel} **Warning** ${Spawn[id ${spellTarget}].CleanName} resisted ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.

			| If the spell did not take hold.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOHOLD]}) {
				/call createTimer "${spellTimer}" "30m"
				
			| If the target was immune.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[IMMUNE]}) {
				/if (${Target.ID}) /chat ${channel} **Warning** ${Spawn[id ${spellTarget}].CleanName} is IMMUNE to ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.
				/call createTimer "${spellTimer}" "30m"
				
			| If the cast was successful.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[SUCCESS]}) {
				/if (${Verbosity} && ${Target.ID}) /chat ${channel} Landed ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]} on ${Spawn[id ${spellTarget}].CleanName}!!.
				/if (${outerDebug} || ${outerDebug_Assists}) /echo |- longTermSpells - [${arrayName}] -| Creating timer [${spellTimer}] -- [${newTimerSetting}]
				/if (${longTerm_recastBuffer}) /varcalc newTimerSetting ${newTimerSetting} - ${longTerm_recastBuffer}
				/call createTimer "${spellTimer}" "${newTimerSetting}"
			
			| If the spell was not ready
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOTREADY]}) {
				
				/if (${FindItemCount[=${castName}]} || ${Me.AltAbility[${castName}]}) {
					/if (${outerDebug} || ${outerDebug_Assists}) /echo |- longTermSpells - [${arrayName}] -| [${castName}] is not ready.  Creating a recast timer for 1/2 the spells normal duration.
					/varcalc newTimerSetting ${newTimerSetting} / 2
					/call createTimer "${spellTimer}" "${newTimerSetting}"
				}
			}
		}

	/if (!${ActionTaken}) /next i

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== cast_longTermSpells - [${arrayName}] -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of Debuff_Targets. Used for Debuffs_OnCommand.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, '/Debuff' or 'Debuffs on ${Target.ID}' in /tell, /group, or /bc.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT DebuffsOn "<#1#> Debuffs on #2#"
SUB EVENT_DebuffsOn(line, ChatSender, MobID)
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_DebuffsOn ==>

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Check if this character debuffs to cast.
		/if (${Command_Debuffs.Size}) {
			
			| Check if the target is a corpse
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]}) {
				/echo Why would I debuff a corpse?
			} else {
			
				| Check range to the debuff target.
				/if (${Spawn[id ${MobID}].Distance} > ${outerMaxAssistDistance}) {
					/chat ${channel} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Check if the Debuff_Targets array is already defined
					/if (${Debuff_Targets.Size}) {
						/declare i int local
						/for i 1 to ${Debuff_Targets.Size}
							/if (${Debuff_Targets[${i}].Arg[1,_]} == ${MobID}) {
								/chat ${channel} I am already debuffing ${Spawn[id ${MobID}].CleanName}.
								/goto :Skip
							}
						/next i
					}
				
					| If the target was not found on the DebuffTargets array, add it.
					/call BuildArray Debuff_Targets ${MobID}_${Zone.ID}
				}
			}
		}
	}

	:Skip
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_DebuffsOn -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Cancels debuffs on command on all targets.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT DebuffsOff "<#1#> End debuffs"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs'"

#EVENT DebuffsOff "<#1#> End debuffs on #2#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs on #2#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs on #2#'"

#EVENT DebuffsOff "<#1#> End debuffs |-|#*#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs |-|#*#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs |-|#*#'"

#EVENT DebuffsOff "<#1#> End debuffs on #2# |-|#*#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs on #2# |-|#*#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs on #2# |-|#*#'"
SUB EVENT_DebuffsOff(line, ChatSender, MobID)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_DebuffsOff ==>

	| If I'm casting debuffs
	/if (${Debuff_Targets.Size}) {

		|- Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
		
			/if (${Defined[${MobID}]}) {
				/chat ${channel} Ending Debuffs on ${Spawn[id ${MobID}].CleanName}.
				/call RemoveArrayElement Debuff_Targets ${MobID}_${Zone.ID}
			} else {
				/chat ${channel} Ending Debuffs on all targets.
				/deletevar Debuff_Targets
			}
		}
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_DebuffsOff -|
/RETURN



|--------------------------------------------------------------------|
|- Casts Command_Debuffs on targets listed in DebuffTargets array.	-|
|--------------------------------------------------------------------|
SUB Debuffs_OnCommand
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- Debuffs_OnCommand ==>

	/declare targetID int local
	/declare targetZone int local
	/declare i int local
	
	/for i 1 to ${Debuff_Targets.Size}
	
		/varset targetID ${Debuff_Targets[${i}].Arg[1,_]}
		/varset targetZone ${Debuff_Targets[${i}].Arg[2,_]}
	
		| Check Mob Zone
		/if (${Zone.ID} != ${targetZone}) {
			/call RemoveArrayElement Debuff_Targets ${Debuff_Targets[${i}]}
		} else {	
			| If the Debuff_Target has died, remove them from the array.
			/if (${Spawn[id ${targetID}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${targetID}].Type.Equal[Corpse]},NULL]}) {
				/call RemoveArrayElement Debuff_Targets ${Debuff_Targets[${i}]}
			} else {
		
				/call cast_longTermSpells "${targetID}" "Command_Debuffs"
		
			}
		}
		
	/if (!${ActionTaken}) /next i
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== Debuffs_OnCommand -|
/RETURN



|---------------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of Debuff_Targets. Used for Debuffs_OnCommand.	 -|
|---------------------------------------------------------------------------------------------|
|- Use, '/DoTs' or 'DoTs on ${Target.ID}' in /tell, /group, or /bc.							 -|
|---------------------------------------------------------------------------------------------|
#EVENT DoTsOn "<#1#> DoTs on #2#"
#EVENT DoTsOn "#1# tells you, 'DoTs on #2#'"
#EVENT DoTsOn "#1# tells the group, 'DoTs on #2#'"
SUB EVENT_DoTsOn(line, ChatSender, MobID)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_DoTsOn ==>

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Check if this character debuffs to cast.
		/if (${Command_DoTs.Size}) {
			
			| Check if the target is a corpse
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]}) {
				/echo Why would I DoT a corpse?
			} else {
			
				| Check range to the DoTs target.
				/if (${Spawn[id ${MobID}].Distance} > ${outerMaxAssistDistance}) {
					/chat ${channel} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Check if the DoT_Targets array is already defined
					/if (${DoT_Targets.Size}) {
						/declare i int local
						/for i 1 to ${DoT_Targets.Size}
							/if (${DoT_Targets[${i}].Arg[1,_]} == ${MobID}) {
								/chat ${channel} I am already debuffing ${Spawn[id ${MobID}].CleanName}.
								/goto :Skip
							}
						/next i
					}
				
					| If the target was not found on the DoT Targets array, add it.
					/call BuildArray DoT_Targets ${MobID}_${Zone.ID}
				}
			}
		}
	}

	:Skip
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_DoTsOn -|
/RETURN



|------------------------------------------------|
|- Cancels debuffs on command on all targets. 	-|
|------------------------------------------------|
#EVENT DoTsOff "<#1#> End DoTs"
#EVENT DoTsOff "#1# tells you, 'End DoTs'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs'"

#EVENT DoTsOff "<#1#> End DoTs on #2#"
#EVENT DoTsOff "#1# tells you, 'End DoTs on #2#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs on #2#'"

#EVENT DoTsOff "<#1#> End DoTs |-|#*#"
#EVENT DoTsOff "#1# tells you, 'End DoTs |-|#*#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs |-|#*#'"

#EVENT DoTsOff "<#1#> End DoTs on #2# |-|#*#"
#EVENT DoTsOff "#1# tells you, 'End DoTs on #2# |-|#*#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs on #2# |-|#*#'"
SUB EVENT_DoTsOff(line, ChatSender, MobID)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_DoTsOff ==>

	| If I'm casting debuffs
	/if (${DoT_Targets.Size}) {

		|- Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
		
			/if (${Defined[${MobID}]}) {
				/chat ${channel} Ending DoTs on ${Spawn[id ${MobID}].CleanName}.
				/call RemoveArrayElement DoT_Targets ${MobID}_${Zone.ID}
			} else {
				/chat ${channel} Ending DoTs on all targets.
				/deletevar DoT_Targets
			}
		}
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_DoTsOff -|
/RETURN



|----------------------------------------------------------------|
|- Casts Command_DoTs on targets listed in DoT_Targets array.	-|
|----------------------------------------------------------------|
SUB DoTs_OnCommand
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- DoTs_OnCommand ==>

	/declare targetID int local
	/declare targetZone int local
	/declare i int local
	
	/for i 1 to ${DoT_Targets.Size}
	
		/varset targetID ${DoT_Targets[${i}].Arg[1,_]}
		/varset targetZone ${DoT_Targets[${i}].Arg[2,_]}
	
		| Check Mob Zone
		/if (${Zone.ID} != ${targetZone}) {
			/call RemoveArrayElement DoT_Targets ${DoT_Targets[${i}]}
		} else {	
			| If the DoT_Target has died, remove them from the array.
			/if (${Spawn[id ${targetID}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${targetID}].Type.Equal[Corpse]},NULL]}) {
				/call RemoveArrayElement DoT_Targets ${DoT_Targets[${i}]}
			} else {
		
				/call cast_longTermSpells "${targetID}" "Command_DoTs"
		
			}
		}
		
	/if (!${ActionTaken}) /next i
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== DoT_OnCommand -|
/RETURN



|----------------------------|
|- Turns rain functions on.	-|
|----------------------------|
#EVENT RainOn "<#1#> Rain on"
#EVENT RainOn "#1# tells you#*#Rain on'"
#EVENT RainOn "#1# tells the group, 'Rain on"
SUB EVENT_RainOn(line, ChatSender)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_RainOn ==>

/if (${Rain_Spells.Size}) {
	
	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| If I'm already casting rain spells
		/if (!${Defined[do_Rain]}) {
			/declare do_Rain bool outer TRUE
			/chat ${channel} Casting rain spells...
		}
	}
}	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_RainOn -|
/RETURN



|--------------------------------|
|- Turns rain functions off.	-|
|--------------------------------|
#EVENT RainOff "<#1#> Rain off"
#EVENT RainOff "#1# tells you#*#RainOff'"
#EVENT RainOff "#1# tells the group, 'Rain off"
SUB EVENT_RainOff(line, ChatSender)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_RainOff ==>

/if (${Defined[do_Rain]}) {
	
	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {	
		
		/if (${Me.Casting.ID}) /interrupt 
		/deletevar do_Rain
		/chat ${channel} Ending rain.
		/doevents flush
		/varset combatTimer 10s
	}
}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_RainOff -|	
/RETURN



|------------------------------------------------------------------------------------------------------------|
|- Casts spells listed in the [Rain] section of the iniCharacter on mobs which wander into your outerAoeRadius.	-|
|------------------------------------------------------------------------------------------------------------|
SUB check_Rain
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- check_Rain ==>

	/if (${do_Rain} && ${Rain_Spells.Size}) {
		
		/declare castName string local

		/declare i int local

		/for i 1 to ${Rain_Spells.Size}
		
			/varset castName ${Rain_Spells[${i}].Arg[1,/]}
	 
			/if (${SpawnCount[npc radius ${outerAoeRadius}]}) {
			
				/if (!${rainTimer_RainSpell${i}}) {
				
					/call e3_Cast ${NearestSpawn[npc radius ${outerAoeRadius}].ID} "${Rain_Spells[${i}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]} || ${Me.Combat},0,90]},]}"
				
				}
			}
			
		/if (!${ActionTaken}) /next i
		
		/varset combatTimer ${combatDelay}
	}
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== check_Rain -|	
/RETURN



|----------------------------------------------------------------|
|- Casts swarm pets from Mages, Necros, Shaman, and Wizards.	-|
|----------------------------------------------------------------|
#EVENT SwarmPets "<#1#> Swarm Pets on #2#"
SUB EVENT_SwarmPets(line, ChatSender, int swarmTarget)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_SwarmPets ==>

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Validate target.
		/if (${SpawnCount[id ${swarmTarget}]} && ${Spawn[${swarmTarget}].Type.NotEqual[Corpse]} && ${Select[${Spawn[${swarmTarget}].Type},${outerAcceptableTargetTypes}]} && ${SpawnCount[pc ${ChatSender}]}) {

			|- -------------- Declare a swarmPets array based on class
		
			| Magician abilities.
			/if (${Me.Class.ShortName.Equal[MAG]}) {
				/declare swarmPets[2] string local
				/varset swarmPets[1] Servant of Ro
				/varset swarmPets[2] Host of the Elements
			| Necromancer abilities.
			} else /if (${Me.Class.ShortName.Equal[NEC]}) {
				/declare swarmPets[2] string local
				/varset swarmPets[1] Swarm of Decay
				/varset swarmPets[2] Wake the Dead
				
			| Shaman abilities.
			} else /if (${Me.Class.ShortName.Equal[SHM]}) {
				/declare swarmPets[1] string local
				/varset swarmPets[1] Spirit Call
			| Wizard abilities.
			} else /if (${Me.Class.ShortName.Equal[WIZ]}) {
				/declare swarmPets[1] string local
				/varset swarmPets[1] Call of Xuzl
			}
			
			| Cast swarm pets
			
			/declare i int local
			
			/for i 1 to ${swarmPets.Size}
			
				/call e3_Cast ${swarmTarget} "${swarmPets[${i}]}"
				/delay 10
				
			/next i
	
		}
	}
	
	/varset combatTimer ${combatDelay}
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_SwarmPets -|
/RETURN



SUB check_Burns
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- check_Burns ==>

	/if (${use_FullBurns}) /call useBurns ${outerAssistTarget} "FullBurns"
	/if (${use_QuickBurns}) /call useBurns ${outerAssistTarget} "QuickBurns"

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== check_Burns -|
/RETURN



|----------------------------------------------------|
|- Engages QuickBurns listed in the iniCharacter.	-|
|----------------------------------------------------|
#EVENT QuickBurn "<#1#> Quick Burn"
SUB EVENT_QuickBurn(line, ChatSender)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_QuickBurn ==>

	| Check Assisting
	/if (!${outerAssisting}) {
		/echo You must be attacking something to burn.
	} else {
	
		| Check for quickburns to cast
		/if (${Defined[QuickBurns]}) {
		
			|- Verify the event
			/call verifyEvent "${ChatSender}" "${line}"
			/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
			|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
			/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
				/chat ${channel} Using Quick Burns...
				/if (!${Defined[use_QuickBurns]}) /declare use_QuickBurns bool outer TRUE
			}
		}
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_QuickBurn -|
/RETURN



|----------------------------------------------------|
|- Engages FullBurns listed in the iniCharacter.	-|
|----------------------------------------------------|
#EVENT FullBurn "<#1#> Full Burn"
SUB EVENT_FullBurn(line, ChatSender)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_FullBurn ==>

	| Check Assisting
	/if (!${outerAssisting}) {
		/echo You must be attacking something to burn.
	} else {
	
		| Check for quickburns to cast
		/if (${Defined[FullBurns]}) {
		
			|- Verify the event
			/call verifyEvent "${ChatSender}" "${line}"
			/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
			|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
			/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
				/chat ${channel} Using Full Burns...
				/if (!${Defined[use_FullBurns]}) /declare use_FullBurns bool outer TRUE
				/if (!${Defined[use_QuickBurns]}) /declare use_QuickBurns bool outer TRUE
			}
		}
	}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_FullBurn -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- FullBurn's MainLoop hook, calls FullBurns, and QuickBurns, in succession until your target dies.	-|
|----------------------------------------------------------------------------------------------------|
SUB FullBurns
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- FullBurns ==>

	/declare castName string local
	
	/declare i int local
	
	/for i 1 to ${FullBurns.Size}
	
		/varset castName ${FullBurns[${i}].Arg[1,/]}
	
		| If the Quickburn to use is a discipline
		/if (${Me.CombatAbility[${castName}]) {
			
			| Check the discipline window to see if something is already active before casting
			/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
				/if (${outerDebug} || ${outerDebug_Assists}) /echo |- QuickBurn -| Waiting on disc [${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}]
			} else {
				/call e3_Cast ${outerAssistTarget} "${QuickBurns[${i}]}"
			}
		} else {
			/call e3_Cast ${outerAssistTarget} "${QuickBurns[${i}]}/GiveUpTimer|${If[${Me.Gem[${castName}]},0,100]}"
		}
	
	/if (!${ActionTaken}) /next i
	
	/varset combatTimer ${combatDelay}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== FullBurns -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------||- Casts spells, abilities, and burns through disciplines on a given targetID.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|SUB useBurns(int burnTarget, arrayName)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- useBurns Target=${burnTarget}- Array=${arrayName} ==>

	/declare castName string local
	
	/declare i int local
	
	/for i 1 to ${${arrayName}.Size}
	
		/varset castName ${${arrayName}[${i}].Arg[1,/]}
		
		/call e3_Cast ${burnTarget} "${${arrayName}[${i}]}/GiveUpTimer|${If[${Me.Gem[${castName}]},0,100]}"
		
	/if (!${ActionTaken}) /next i
	
	/varset combatTimer ${combatDelay}

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== useBurns -|
/RETURN

#EVENT toggle_Taunt "[#1#] Toggle Taunt #2#"
#EVENT toggle_Taunt "<#1#> Toggle Taunt #2#"
#EVENT toggle_Taunt "#1# tells you, 'Toggle Taunt #2#'"
#EVENT toggle_Taunt "#1# tells the group, 'Toggle Taunt #2#'"
#EVENT toggle_Taunt "#1# you tell your party, 'Toggle Taunt #2#'"
SUB EVENT_toggle_Taunt(line, ChatSender, eventParams)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_toggle_Taunt ==>

	/declare tauntSet string local ${eventParams.Arg[1, ]}

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| Turn taunt On
		/if (${tauntSet.Find[on]} || ${tauntSet.Find[true]} || ${Int[${tauntSet}]}) {
			/varset AutoAggro TRUE
			
		| Turn PvP Off.
		} else {
			/varset AutoAggro FALSE
		}

		/echo Auto-Aggro [${If[${outerAutoAggro},ON,OFF]}]
	}
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_toggle_Taunt -|
/RETURN



#EVENT toggle_outerAssistType "[#1#] Toggle Assist Type #2#"
#EVENT toggle_outerAssistType "<#1#> Toggle Assist Type #2#"
#EVENT toggle_outerAssistType "#1# tells you, 'Toggle Assist Type #2#'"
#EVENT toggle_outerAssistType "#1# tells the group, 'Toggle Assist Type #2#'"
#EVENT toggle_outerAssistType "#1# you tell your party, 'Toggle Assist Type #2#'"
SUB EVENT_toggle_outerAssistType(line, ChatSender, eventParams)
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- EVENT_toggle_outerAssistType ==>

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		/if (${Defined[outerAssistType]}) {
		
			/declare outerAssistType_Set string local ${eventParams.Arg[1, ]}
		
			/if (${outerAssistType_Set.Find[range]}) {
				/varset outerAssistType Ranged
			} else /if (${outerAssistType_Set.Find[melee]}) {
				/varset outerAssistType Melee
			} else {
				/echo [${outerAssistType_Set}] is not a recognized assist type.
			}

			/echo Assist Type [${outerAssistType}]
		}
	}
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== EVENT_toggle_outerAssistType -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB setupAssist
	/call iniToVar "${iniAdvanced},Debug,Debug Assists (On/Off)" Debug_Assists bool outer
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- setupAssist ==>
	
	| Add Assists Paths
	/if (!${Ini[${iniTHF},Paths,Enrage Settings].Length}) /call writeToIni "${iniTHF},Paths,Enrage Settings" "thf Macro Inis\Enrage Settings.ini"
	/if (!${Ini[${iniTHF},Paths,Stick Settings].Length}) /call writeToIni "${iniTHF},Paths,Stick Settings" "thf Macro Inis\Stick Settings.ini"
	
	| Enrage Settings
	/if (!${Ini[${iniTHF},Paths,Enrage Settings].Length}) {
		/echo ERROR: Could not find designated file path for [Enrage Settings].  Please review review settings in [${iniTHF} > Paths].
		/endMacro
	} else {
		/declare iniEnrage string outer ${Ini[${iniTHF},Paths,Enrage Settings]}
	}
	
	| Create variables used in Assist scripts.
	/declare outerAssisting 				bool 	outer FALSE
	/declare outerAssistTarget 				int 	outer 0
	/declare outerAllowControl 				bool 	outer FALSE
	/declare outerAssistStickDistance 		int 	outer
	/declare outerAutoAggro 				bool 	outer TRUE
	/declare outerHasTanked 				bool 	outer FALSE
	/declare outerEnraged 					bool 	outer FALSE
	/declare outerEnrageAttack 				bool 	outer FALSE
	/declare longTerm_recastBuffer 			int 	outer 180
	/declare use_Nukes 						bool 	outer TRUE
	
	|- Import Macro_Ini Settings.
	/call iniToVar "${iniGeneral},Assists,Max Engage Distance" outerMaxAssistDistance int outer
	/call iniToVar "${iniGeneral},Assists,AE Threat Range" outerAoeRadius int outer
	/call iniToVar "${iniGeneral},Assists,Acceptable Target Types" outerAcceptableTargetTypes string outer
	/call iniToVar "${iniGeneral},Assists,Auto-Assist (On/Off)" outerAutoAssistOn bool outer
	
	|- Import iniCharacter Settings.

	| [Melee/Ranged].
	/if (${Ini[${iniCharacter},Assist Settings,Assist Type (Melee/Ranged)].Length}) /call iniToVar "${iniCharacter},Assist Settings,Assist Type (Melee/Ranged)" outerAssistType string outer
	/if (${Ini[${iniCharacter},Assist Settings,Melee Stick Point].Length}) /call iniToVar "${iniCharacter},Assist Settings,Melee Stick Point" outerAssistStickPoint string outer
	/if (${Ini[${iniCharacter},Assist Settings,Melee Distance].Length}) /call iniToVar "${iniCharacter},Assist Settings,Melee Distance" outerAssistDistanceSetting string outer
	/if (${Ini[${iniCharacter},Assist Settings,Ranged Distance].Length}) /call iniToVar "${iniCharacter},Assist Settings,Ranged Distance" outerRangedStickDistance string outer
	/if (${Ini[${iniCharacter},Assist Settings,Auto-Assist Engage Percent].Length}) /call iniToVar "${iniCharacter},Assist Settings,Auto-Assist Engage Percent" outerEngagePct int outer
	/if (!${Defined[outerEngagePct]}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo ERROR: @[outerEngagePct] - outerEngagePct is invalid, defaulting to 98%
		/declare outerEngagePct int outer 98
	} else /if (${Defined[outerEngagePct]} && !${outerEngagePct}) {
		/if (${outerDebug} || ${outerDebug_Assists}) /echo ERROR: @[outerEngagePct] - outerEngagePct is invalid, defaulting to 98%
		/varset outerEngagePct 98
	}
	
	| [Melee Abilities].
	/if (${Ini[${iniCharacter},Melee Abilities,Ability#1].Length}) /call INItoArray "${iniCharacter},Melee Abilities,Ability#" MyAbilities

	| [Nukes].
	/if (${Ini[${iniCharacter},Nukes,Nuke#1].Length}) /call INItoArray "${iniCharacter},Nukes,Nuke#" MyNukes
	
	| [DoTs].
	
	|- On Assist
	/if (${Ini[${iniCharacter},DoTs,DoT on Assist#1].Length}) /call INItoArray "${iniCharacter},DoTs,DoT on Assist#" Assist_DoTs

	|- On Command
	/if (${Ini[${iniCharacter},DoTs,DoT on Command#1].Length}) /call INItoArray "${iniCharacter},DoTs,DoT on Command#" Command_DoTs

	| [Debuffs].

	|- On Assist
	/if (${Ini[${iniCharacter},Debuffs,Debuff on Assist#1].Length}) /call INItoArray "${iniCharacter},Debuffs,Debuff on Assist#" Assist_Debuffs

	|- On Command
	/if (${Ini[${iniCharacter},Debuffs,Debuff on Command#1].Length}) /call INItoArray "${iniCharacter},Debuffs,Debuff on Command#" Command_Debuffs
	
	| [Rain].
	/if (${Ini[${iniCharacter},Rain,Rain#1].Length}) /call INItoArray "${iniCharacter},Rain,Rain#" Rain_Spells

	| [QuickBurns].
	/if (${Ini[${iniCharacter},Burn,Quick Burn#1].Length}) /call IniToArray "${iniCharacter},Burn,Quick Burn#" QuickBurns
	
	| [FullBurns].	
	/if (${Ini[${iniCharacter},Burn,Full Burn#1].Length}) /call IniToArray "${iniCharacter},Burn,Full Burn#" FullBurns
	
	| If Assisting variables are defined, turn assists on.
	/if (${Defined[outerAssistType]} || ${Defined[MyAbilities]} || ${Defined[${arrayName}]} || ${Defined[Assist_DoTs]} || ${Defined[Command_DoTs]} || ${Defined[Assist_Debuffs]} || ${Defined[Command_Debuffs]} || ${Defined[Rain_Spells]} || ${Defined[QuickBurns]} || ${Defined[FullBurns]}) /declare AssistsOn bool outer TRUE

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== setupAssist -|
/RETURN


|---------------------------------------------------------------------------------------------------------------------------------------------------|
|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB checkAssistBackgroundEvents
	
	/if (${outerAssisting}) {
		/doevents Assist
		/doevents BackOff
		/call checkAssistTarget
		/call checkAssistStatus
		/doevents EnrageOn
		/doevents EnrageOff
	}
	
	/if (${outerAutoAssistOn}) /call checkAutoAssist
	/if (!${Me.Casting.ID} && ${Me.Combat} && ${MyAbilities.Size}) /call CombatAbilities
	
	/doevents getCloser
	/doevents DebuffsOn
	/doevents DebuffsOff
	/doevents DoTsOn
	/doevents DoTsOff
	/doevents TauntOff
	/doevents TauntOn
	/doevents RainOff
	/doevents RainOn
	/doevents QuickBurn
	/doevents FullBurn
	/doevents toggle-PvP
	/doevents setEnrageAttack

/RETURN



SUB assist_MacroSettings
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- assist_MacroSettings ==>

	/call writeToIni "${iniAdvanced},Debug,Debug Assists (On/Off)"

	/call writeToIni "${iniGeneral},Assists,Auto-Assist (On/Off)" On
	/if (${Ini[${iniGeneral},Assists,Auto-Assist Engage Percent].Length}) /call writeToIni "${iniGeneral},Assists,Auto-Assist Engage Percent" "DEPRECATED - YOU MAY DELETE THIS ENTRY" 1
	/call writeToIni "${iniGeneral},Assists,Max Engage Distance" 250
	/call writeToIni "${iniGeneral},Assists,AE Threat Range" 100
	/call writeToIni "${iniGeneral},Assists,Acceptable Target Types" NPC,Pet
	
/if (${outerDebug} || ${outerDebug_Assists}) /echo <== assist_MacroSettings -|
/RETURN



SUB assistCharacterSettings
	/if (${outerDebug} || ${outerDebug_Assists}) /echo |- assistCharacterSettings ==>

	| Add Assist_Stick entries.
	/call writeToIni "${iniCharacter},Assist Settings,Assist Type (Melee/Ranged)" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},Melee,]}
	/call writeToIni "${iniCharacter},Assist Settings,Auto-Assist Engage Percent" 98
	/call writeToIni "${iniCharacter},Assist Settings,Melee Stick Point" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},Behind,]}
	/call writeToIni "${iniCharacter},Assist Settings,Melee Distance" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},MaxMelee,]}
	/call writeToIni "${iniCharacter},Assist Settings,Ranged Distance" 100

	| If I'm a melee class, add Melee Abilities entries.
	/if (${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
		/if (!${Ini[${iniCharacter},Melee Abilities].Length}) /call writeToIni "${iniCharacter},Melee Abilities,Ability#1"
	}

	| If I'm a casting class, add Nuke entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,PAL,NEC,RNG,SHD,SHM,WIZ]}) {
		/if (!${Ini[${iniCharacter},Nukes].Length}) /call writeToIni "${iniCharacter},Nukes,Nuke#1"
	}

	| If I'm a casting class, add DoT entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,NEC,RNG,SHD,SHM]}) {
		/if (!${Ini[${iniCharacter},DoTs].Length}) {
			/call writeToIni "${iniCharacter},DoTs,DoT on Assist#1"
			/call writeToIni "${iniCharacter},DoTs,DoT on Command#1"
		}
	}

	| If I'm a casting class, add Debuff entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,NEC,SHM]}) {
		/if (!${Ini[${iniCharacter},Debuffs].Length}) {
			/call writeToIni "${iniCharacter},Debuffs,Debuff on Assist#1"
			/call writeToIni "${iniCharacter},Debuffs,Debuff on Command#1"
		}
	}

	| If I'm a casting class, add Rain entries.
	/if (${Select[${Me.Class.ShortName},DRU,MAG,SHM,WIZ]}) {
		/if (!${Ini[${iniCharacter},Rain].Length}) /call writeToIni "${iniCharacter},Rain,Rain#1"
	}

	| Burn entries.
	/if (!${Ini[${iniCharacter},Burn].Length}) {
		/call writeToIni "${iniCharacter},Burn,Quick Burn#1"
		/call writeToIni "${iniCharacter},Burn,Full Burn#1"
	}

	/if (${outerDebug} || ${outerDebug_Assists}) /echo <== assistCharacterSettings -|
/RETURN



SUB assist_Aliases
/if (${outerDebug} || ${outerDebug_Assists}) /echo |- assist_Aliases ==>

	/noparse /squelch /alias /AssistOn /bc Assist on ${Target.ID}
	/noparse /squelch /alias /AssistMe /bc Assist on ${Target.ID}
	/squelch /alias /BackOff /bc Back off
	/noparse /squelch /alias /debuff /bc Debuffs on ${Target.ID}
	/noparse /squelch /alias /debuffson /bc Debuffs on ${Target.ID}
	/squelch /alias /debuffsoff /bc End Debuffs
	/noparse /squelch /alias /dot /bc DoTs on ${Target.ID}
	/noparse /squelch /alias /dotson /bc DoTs on ${Target.ID}
	/squelch /alias /dotsoff /bc End DoTs
	/squelch /alias /rain /bc Rain on
	/squelch /alias /rainOn /bc Rain on
	/squelch /alias /rainOff /bc Rain off
	/noparse /squelch /alias /SwarmPets /bc Swarm pets on ${Target.ID}
	/squelch /alias /QuickBurns /bc Quick burns
	/squelch /alias /FullBurns /bc Full burns
	/squelch /alias /pvpOn /bc Toggle PvP ON
	/squelch /alias /pvpOff /bc Toggle PvP OFF
	/squelch /alias /TauntOn /bc Toggle Taunt ON
	/squelch /alias /TauntOff /bc Toggle Taunt OFF
	/squelch /alias /outerAssistType /bc Toggle Assist Type

/if (${outerDebug} || ${outerDebug_Assists}) /echo <== assist_Aliases -|
/RETURN