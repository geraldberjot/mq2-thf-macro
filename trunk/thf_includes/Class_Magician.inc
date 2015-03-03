|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Magician.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Magician specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

#EVENT petWeapons "<#1#> Pet Weapons"
#EVENT petWeapons "<#1#> Pet Toys"
SUB EVENT_petWeapons(string line, string from)
	/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons ==>

	/if (${petItems.Size}) {
		|- Verify the event
		/call amIConcerned ${from} ${Me.CleanName} FALSE
		/if (${Macro.Return}) {
			/call iniToVar "${iniGeneral},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
			:allowAccess
			
			/if (!${SpawnCount[pc ${from}]}) {
				/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons -|- [${from}] is not in ${Zone}.
			} else {
				
				/if (${Spawn[pc ${from}].Distance} > 200) {
					/tell ${from} You're too far away.
				} else {
				
					/if (!${Spawn[pc ${from}].LineOfSight}) {
						/tell ${from} I can't see you.
					} else {

						/if (!${Spawn[pet ${Spawn[pc ${from}].Pet}].ID}) {
							/tell ${from} You don't have a pet, jackass! 
						} else {
						
							/if (${from.NotEqual[${Me}]}) /tell ${from} Incoming pet weapons.  Please stay still for a few moments.
							
							|- Declare local variables
							/declare i int local
							/declare x int local
							/declare e int local
							/declare retryTimer timer local
							/declare miscTimer timer local
							/declare summonedItem string local
							
							|- Target the pet, and move to it
							/call trueTarget ${Spawn[pet ${Spawn[pc ${from}].Pet}].ID}
							/call MoveTo -SpawnID|${Spawn[pet ${Spawn[pc ${from}].Pet}].ID}
							
							/for i 1 to ${petItems.Size}
							
							/if (${Target.Distance} < 20) {
								/call thf_Casting 0 "${petItems[${i}]}/GiveUpTimer|${If[${Me.Gem[${petItems[${i}].Arg[1,/]}]},50,150]}"
								/delay 20 ${Cursor.ID}

								/if (${Cursor.ID}) {
									
									/varset summonedItem ${Cursor.Name}
									/varset retryTimer 50
									
									:OpenTrade_Loop
									
									/click left target
									/delay 10 ${Window[GiveWnd].Open}
									
									/if (!${Window[GiveWnd].Open}) {
										/if (${retryTimer}) {
											/goto :OpenTrade_Loop
										} else {
											/echo Failed to open trade with ${Target.CleanName}.
										}
									} else {
										
										/varset retryTimer 30
										:WaitAccept_Loop
										
										/notify GiveWnd GVW_Give_Button LeftMouseUp
										/delay 5 !${Window[GiveWnd].Open}
										
										/if (${Window[GiveWnd].Open}) {
											/if (${retryTimer}) {
												/goto :WaitAccept_Loop
											} else {
												/echo Failed to open trade with ${Target.CleanName}.
											}
										}
									}
								}
								
								/if (${FindItemCount[=${summonedItem}]}) {
									
									/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons -|- Dropped [${summonedItem}] in my inventory.
								
									|- Open InventoryWindow.
									/varset miscTimer 3s
									
									:openInventory
									/if (!${Window[InventoryWindow].Open}) /nomodkey /keypress i
									/delay 5 ${Window[InventoryWindow].Open}
									/if (!${Window[InventoryWindow].Open} && ${miscTimer}) /goto :openInventory
						
									|- Search packslots.
									/varset e 0
									/for x 1 to 8  
									
										|- If the item in the packslot is the summoned item, pick it up.
										/if (!${Me.Inventory[pack${x}].Container} && ${summonedItem.Equal[${Me.Inventory[pack${x}]}]}) {
											/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons -|- Picking up [${summonedItem}] form pack${x}
											/itemnotify pack${x} leftmouseup
										} else {
										
											|- If the item in the packslot is a container, search it.
											/for e 1 to ${Me.Inventory[pack${x}].Container}
												/if (${summonedItem.Equal[${Me.Inventory[pack${x}].Item[${e}]}]}) {
													/if (!${Window[pack${x}].Open}) /itemnotify pack${x} rightmouseup
													/delay 1s ${Window[pack${x}].Open}
													/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons -|- Picking up [${summonedItem}] form pack${x} slot${e}
													/itemnotify in pack${x} ${e} leftmouseup
												}
											/next e
										}
									/next x
									
									/if (${retryTimer}) {
										/goto :OpenTrade_Loop
									} else {
										/if (${Cursor.Name.Equal[${summonedItem}]}) /destroy 
									}
								}
							}
							
							/next i
						}
					}
				}
			}
		}
	}

/if (${outerDebug} || ${outerDebug_MAG}) /echo <== EVENT_petWeapons -|
/RETURN

#EVENT distributePetWeapons "#1# tells you, 'Hand out Pet Weapons'"
#EVENT distributePetWeapons "<#1#> Hand out Pet Weapons"
#EVENT distributePetWeapons "#1# tells you, 'Distribute Pet Weapons'"
#EVENT distributePetWeapons "<#1#> Distribute Pet Weapons"
SUB EVENT_distributePetWeapons(line, ChatSender)
/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_distributePetWeapons ==>

	/if (${petItems.Size}) {
	
		|- Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|-    |-----------Valid User----------|
		/if (!${Bool[${Macro.Return.Arg[2,_]}]}) {

			/call iniToVar "${iniGeneral},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
			
			|- If the user is not valid, check allow_BuffRequests
			/if (${Macro.Return}) {
		
				|- If allow_BuffRequests=TRUE, check restricked PCs
				/if (!${Select[${ChatSender},${Ini[${Macro_Ini},Buff Requests,Restricted PCs]}]}) /goto :allowAccess
			}
		} else {
		
			:allowAccess
			
			/if (!${SpawnCount[pc ${ChatSender}]}) {
				/if (${outerDebug} || ${outerDebug_MAG}) /echo |- EVENT_petWeapons -|- [${ChatSender}] is not in ${Zone}.
			} else {
				
				/if (${Spawn[pc ${ChatSender}].Distance} > 200) {
					/tell ${ChatSender} You're too far away.
				} else {
				
					/if (!${Spawn[pc ${ChatSender}].LineOfSight}) {
						/tell ${ChatSender} I can't see you.
					} else {
					
					
						/if (${ChatSender.NotEqual[${Me}]}) /tell ${ChatSender} Handing out pet weapons...
					
						/declare originalLoc string local ${Me.Loc.Replace[ ,]}
						/declare startingLoc string local ${Spawn[pc ${ChatSender}].Loc.Replace[ ,]}
						
						/declare nearbyPetOwners string local
						/declare petOwner string local
						/declare petID int local
						
						/declare i int local
						
					
						|- Move to the starting point
						/call MoveTo -Loc|${startingLoc}
					
						|- Make a list of all the pets nearby
						/for i 1 to ${SpawnCount[radius 100]}
						
							/if (${NearestSpawn[${i}].Type.Equal[PC]} && ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID} && ${Spawn[id ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID}].LineOfSight}) /varset nearbyPetOwners ${nearbyPetOwners}${NearestSpawn[${i}]},
				
						/next i
					
						|- Pass out weapons to each person on the nearbyPetOwners list
						
						/varset i 1
						
						:next_PetOwner
						
						|- set petOwner and petID, then send the owner a msg.
						/varset petOwner ${nearbyPetOwners.Arg[${i},,]}
					
						/call EVENT_petWeapons "${Line}" "${petOwner}"

						/varcalc i ${i} + 1
						/if (${Bool[${nearbyPetOwners.Arg[${i},,]}]}) /goto :next_PetOwner
						
						|- Move to the starting point
						/call MoveTo -Loc|${originalLoc}
					}
				}
			}
		}
	}

/if (${outerDebug} || ${outerDebug_MAG}) /echo <== EVENT_distributePetWeapons -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_PetWeapons
/if (${outerDebug} || ${outerDebug_MAG}) /echo |- check_petWeapons ==>

	/if (${autoWeapons} && ${Zone.ID}) {
		
		/if (${BuffCheck} && !${Me.Moving} && !${outerCombatTimer} && !${outerActiveTimer} && !${outerRebuffTimer}) {
		
			/declare i int local
			
			/for i 1 to ${NetBots.Counts}
				
				/if (${NetBots[${NetBots.Client[${i}]}].Zone} && ${NetBots[${NetBots.Client[${i}]}].PetID}) {
					
					/if (${Spawn[id ${NetBots[${NetBots.Client[${i}]}].PetID}].CleanName.Equal[${Ini[${iniTHF},${MacroQuest.Server}-${Me.Class.ShortName}-${Me.CleanName},Auto-Pet Weapons-${NetBots.Client[${i}]}]}]}) {
						/if (${outerDebug} || ${outerDebug_MAG}) /echo |- check_petWeapons -|- I have already given weapons to [${Spawn[id ${NetBots[${NetBots.Client[${i}]}].PetID}].CleanName}].
					} else {

						|- /chat ${channelNumber} Handing out pet weapons to [${Spawn[id ${NetBots[${NetBots.Client[${i}]}].PetID}].CleanName}(${NetBots.Client[${i}]})], please wait a few moments...
						/call EVENT_petWeapons PLACEHOLDER ${NetBots.Client[${i}]}
						/call writeToIni "${iniTHF},${MacroQuest.Server}-${Me.Class.ShortName}-${Me.CleanName},Auto-Pet Weapons-${NetBots.Client[${i}]}" "${Spawn[id ${NetBots[${NetBots.Client[${i}]}].PetID}].CleanName}" 1
					}
				}

			/next i
		}
	}

/if (${outerDebug} || ${outerDebug_MAG}) /echo <== check_petWeapons -|
/RETURN



|**
#EVENT CoH_Group "#*# tells the group, 'CoH the group"
#EVENT CoH_Group "#*# tells the group, 'CoH group"
SUB EVENT_CoH_Group
/if (${outerDebug} || ${outerDebug_MAG}) /echo |- CoH_Group ==>

	/if (${Defined[cohSpell]}) {

	}

/if (${outerDebug} || ${outerDebug_MAG}) /echo |- CoH_Group ==>
/RETURN
**|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MAG_Setup
	/if (${outerDebug} || ${outerDebug_MAG}) /echo |- MAG_Setup ==>
	
	/call iniToVar "${iniToon},Magician,Auto-Pet Weapons (On/Off)" autoWeapons bool outer
	/call iniToArray "${iniToon},Magician,Summoned Pet Item#" petItems

	/if (${outerDebug} || ${outerDebug_MAG}) /echo <== MAG_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MAG_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MAG_MacroSettings
	/if (${outerDebug} || ${outerDebug_MAG}) /echo |- MAG_MacroSettings ==>

	/call writeToIni "${iniTHF},Magician,Functions" "check_CombatBuffs/check_petHeal/check_Burns/check_Debuffs/check_Rain/check_Nukes/check_Buffs/check_Pets/check_PetWeapons" FALSE

	/if (${outerDebug} || ${outerDebug_MAG}) /echo <== MAG_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MAG_CharacterSettings
	/if (${outerDebug} || ${outerDebug_MAG}) /echo |- MAG_CharacterSettings ==>

	/call writeToIni "${iniToon},Magician,Auto-Pet Weapons (On/Off)"
	/call writeToIni "${iniToon},Magician,Summoned Pet Item#1"

	/if (${outerDebug} || ${outerDebug_MAG}) /echo <== MAG_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MAG_Aliases
	/if (${outerDebug} || ${outerDebug_MAG}) /echo |- MAG_Aliases ==>

	/squelch /alias /dpw /bc Distribute Pet Weapons
	/squelch /alias /PetWeapons /bc Pet Weapons

	/if (${outerDebug} || ${outerDebug_MAG}) /echo <== MAG_Aliases -|
/RETURN