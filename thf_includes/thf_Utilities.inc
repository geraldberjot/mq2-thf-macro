|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- thf_Utilities.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- This is thf's 'toolbox' which contains scripts used throughout thf's various functions.
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Validates Ini entries from supplied Ini keys, and creates variables of the specified type and scope.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Returns validated Ini entries if scope is local, or creates variables of specified type and scope.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to create a variable from.
|- @param VarToMake: The name of the variable you want to declare.
|- @param VarType: The type of variable to be declared.
|- @param VarScope: The scope of the variable to be declared.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call iniToVar IniKey VariableToMake VariableType VariableScope
|-	Ex:
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-		To create an outer variable,
|-			/call iniToVar Bot_Killians.ini,AssistStick,StickDistance int outer
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-		To pass a local variable,
|-			/call iniToVar Bot_Killians.Ini,Basics,Use_Potion/Pct UNDEFINED string local
|-			/declare PotionName ${Macro.Return.Arg[1,/]}
|-			/declare PotionPct ${Macro.Return.Arg[2,/]}
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB iniToVar(Ini_Key, VarToMake, VarType, VarScope)
	/if (${outerDebug}) /echo |- iniToVar [${Ini_Key}] ==>

	|- Check for the specified ini key.
	/if (!${Ini[${Ini_Key}].Length}) {
		/if (${outerDebug}) /echo [${Ini_Key}] not found.
	} else {
	
		|- Validate bool.
		/if (${VarType.Equal[bool]}) {
			/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
			/if (!${Defined[new_VarName]}) /declare new_VarName string outer
			/declare ${VarToMake} bool ${VarScope} ${If[${Select[${Ini[${Ini_Key}]},TRUE,ON,YES,1]},TRUE,FALSE]}
			/if (${outerDebug}) /echo |- iniToVar -|- Defined: ${VarToMake} [${${VarToMake}}]
		|- Validate int.
		} else /if (${VarType.Equal[Int]}) {
			/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
			/declare ${VarToMake} int ${VarScope} ${Ini[${Ini_Key}]}
			/if (${outerDebug}) /echo |- iniToVar -|- Defined: ${VarToMake} [${${VarToMake}}]
		|- Validate float.
		} else /if (${VarType.Equal[Float]}) {
			/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
			/declare ${VarToMake} float ${VarScope} ${Ini[${Ini_Key}]}
			/if (${outerDebug}) /echo |- iniToVar -|- Defined: ${VarToMake} [${${VarToMake}}]
		|- Create string or timer.
		} else {
			/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
			/declare ${VarToMake} ${VarType} ${VarScope} ${Ini[${Ini_Key}]}
			/if (${outerDebug}) /echo |- iniToVar -|- Defined: ${VarToMake} [${${VarToMake}}]
		}
	}

	/if (${outerDebug}) /echo <== iniToVar -|
/RETURN ${${VarToMake}}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Counts ini entries and creates a corresponding array								-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to create an array from			-|
|- @param ArrayName: The name of the array you want to declare						-|
|- @param ArrayScope: The scope of the array to be declared							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call iniToArray IniKey ArrayName VariableScope							-|
|-	Ex: /call iniToArray "Settings.ini,General,Debug Mode" Debug outer		-|
|-		/call iniToVar Bot_Killians.ini,Buffs,SelfBuffs MySelfBuffs local			-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB iniToArray(Ini_Key, ArrayName, ArrayScope)
	/if (${outerDebug}) /echo |- iniToArray [${Ini_Key}] ==>

	|- Check for the specified ini key.
	/if (!${Ini[${Ini_Key}1].Length}) {
		/if (${outerDebug}) /echo [${Ini_Key}] not found.
	} else {
		|- Count ini entries
		/declare i int local 1
		/declare count int local

		:CountingLoop
		/if (${Ini[${Ini_Key}${i}].Length}) {
			/varset count ${i}
			/varcalc i ${i} + 1
			/goto :CountingLoop
		}
		
		|- Declare the array and copy ini entries
		/if (${Defined[${ArrayName}]}) /deletevar ${ArrayName}
		/declare ${ArrayName}[${count}] string ${If[${Defined[ArrayScope]},${ArrayScope},outer]}
		
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}[${i}] ${Ini[${Ini_Key}${i}]}
		/next i
	}

	/if (${outerDebug}) {
		/if (${${ArrayName}.Size}) {
			/echo Declared: ${ArrayName}[${${ArrayName}.Size}]
			/for i 1 to ${${ArrayName}.Size}
				/echo ${ArrayName}[${i}] - ${${ArrayName}[${i}]}
			/next i
		}
		/echo <== iniToArray -|
	}
/RETURN ${ArrayName}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Split a ini key with the specified separator and return the specified array
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_Key : the ini value to split
|- @param ArrayName : The name of the array you want to declare
|- @param ArrayScope : The scope of the array to be declared
|- @param separator : The char to split with
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call splitFromIni IniKey ArrayName VariableScope
|-	Ex: /call splitFromIni "Settings.ini,General,Debug Mode" Debug outer
|-		/call splitFromIni Bot_Killians.ini,Buffs,SelfBuffs MySelfBuffs local
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB splitFromIni(String Ini_Key, String ArrayName, String ArrayScope, String separator)
	/if (${outerDebug}) /echo |- splitFromIni [(${Ini_Key})'s value with ${separator}] ==>

	/declare value String local
	/varset value ${Ini[${Ini_Key}]}
	|- Check for the specified ini key.
	/if (!${value.Length}) {
		/if (${outerDebug}) /echo [${Ini_Key}] not found !
	} else {
			/call split "${value}" ${ArrayName} ${ArrayScope} ${separator}
		}
	}
	/if (${outerDebug}) /echo <== splitFromIni -|
/RETURN ${ArrayName}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Split a string with the specified separator and return the specified array
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_Key : The string to split
|- @param ArrayName : The name of the array you want to declare
|- @param ArrayScope : The scope of the array to be declared
|- @param separator : The char to split with
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call iniToArray IniKey ArrayName VariableScope
|-	Ex: /call iniToArray "Settings.ini,General,Debug Mode" Debug outer
|-		/call iniToVar Bot_Killians.ini,Buffs,SelfBuffs MySelfBuffs local
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB split(String value, String ArrayName, String ArrayScope, String separator)
	/if (${outerDebug}) /echo |- split [(${value})'s value with '${separator}'] ==>

	|- Check for the specified ini key.
	/if (!${value.Length}) {
		/if (${outerDebug}) /echo Value is empty !
	} else {
		|- Declare the array and copy elements
		/if (${Defined[${ArrayName}]}) /deletevar ${ArrayName}

		|- Check if the separator is in the string.
		/if (!${value.Find[${separator}]}) {
			/if (${outerDebug}) /echo [${separator}] not found in ${value}.
			/declare ${ArrayName}[1] string ${If[${Defined[ArrayScope]},${ArrayScope},outer]}
			/varset ${ArrayName}[1] ${value}
		} else {
			|- Count separator
			/declare i int local 1
			/declare count int local
			/varcalc count ${value.Count[${separator}]}+1
			
			/declare ${ArrayName}[${count}] string ${If[${Defined[ArrayScope]},${ArrayScope},outer]}
			/for i 1 to ${${ArrayName}.Size}
				/varset ${ArrayName}[${i}] ${value.Token[${i},${separator}]}
			/next i
		}

		/if (${outerDebug}) {
			/echo Declared: ${ArrayName}[${${ArrayName}.Size}]
			/if (${${ArrayName}.Size} > 1) {
				/for i 1 to ${${ArrayName}.Size}
					/echo ${ArrayName}[${i}] - ${${ArrayName}[${i}]}
				/next i
			} else {
				/echo ${ArrayName}[1] - ${${ArrayName}[1]}
			}
			/echo <== split -|
		}
	}
/RETURN ${ArrayName}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Creates ini entries from supplied ini keys.														-|
|- Allows the user to toggle whether or not to overwrite an existing entry							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to write to										-|
|- @param WhatToWrite: Data to write to the given Ini_Key											-|
|- @param OverWrite(bool): Allow overwrite															-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call writeToIni "Ini_Key" "WhatToWrite" "OverWrite"										-|
|-	Ex:																								-|
|-		Blank entry 				- 	/call writeToIni "Loot.ini,B,Bone Chips"					-|
|-		Entry with setting 			- 	/call writeToIni "Loot.ini,B,Bone Chips" "Keep"				-|
|-		Overwrite existing entry	- 	/call writeToIni "Loot.ini,B,Bone Chips" "Destroy" TRUE		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB writeToIni(Ini_Key, WhatToWrite, bool OverWrite)
	/if (${outerDebug}) {
		/echo |- writeToIni ==>
		/echo Ini_Key = [${Ini_Key}] - Data to write = [${WhatToWrite}] - OverWrite = [${Bool[${OverWrite}]}]	
	}

	|- By switching ':'s to '*'s in arguemnt 3, to avoid issues when reading variables from the inis.
	/if (${Ini_Key.Arg[3,,].Find[:]}) /varset Ini_Key ${Ini_Key.Arg[1,,]},${Ini_Key.Arg[2,,]},${Ini_Key.Arg[3,,].Replace[:,;]}
	
	|- If the Ini_Key already exists, check overwrite	
	/if (${Ini[${Ini_Key}].Length}) {
		/if (${OverWrite}) /goto :OverWrite
	} else {
		:OverWrite
		/if (${Defined[WhatToWrite]}) {
			/ini "${Ini_Key.Arg[1,,]}" "${Ini_Key.Arg[2,,]}" "${Ini_Key.Arg[3,,]}" "${WhatToWrite}"
		
		|- Else, create the entry only.
		} else {
			/ini "${Ini_Key.Arg[1,,]}" "${Ini_Key.Arg[2,,]}" "${Ini_Key.Arg[3,,]}"
		}
	}

	/if (${outerDebug}) /echo <== writeToIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Creates new arrays, or stacks onto existing arrays.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Syntax: /call BuildArray ArrayName Data
|- Ex: /call BuildArray DebuffTargets ${Target.ID}
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param ArrayName: Name of the array to build.
|- @param Data: Data you wish to add to the array.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB BuildArray(ArrayName, Data, varScope)
	/if (${outerDebug}) /echo |- BuildArray Adding [${ArrayName}-${Data}-${varScope}] ==>

	/if (!${Defined[varScope]}) /declare varScope string local outer
	|- If the array is not defined, declare it.
	/if (!${Defined[${ArrayName}]}) {
		/declare ${ArrayName}[1] string ${varScope}
		/varset ${ArrayName}[1] ${Data}
		
	|- Else, if the array is already defined.
	} else {
		|- Create a copy of the existing array.
		/declare ${ArrayName}Copy[${${ArrayName}.Size}] string local
		/declare i int local
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}Copy[${i}] ${${ArrayName}[${i}]}
		/next i
		|- Delete the existing array.
		/deletevar ${ArrayName}
		
		|- Build a resized array.
		/declare ${ArrayName}[${Math.Calc[${${ArrayName}Copy.Size}+1].Int}] string ${varScope}
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}[${i}] ${${ArrayName}Copy[${i}]}
		/next i
	
		|- Insert new data into the array
		/varset ${ArrayName}[${${ArrayName}.Size}] ${Data}
	}

	/if (${outerDebug}) /echo <== BuildArray -|
/RETURN ${ArrayName}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Removes elements from an existing array.			-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param ArrayName: Name of the array to search.	-|
|- @param Data: Data you wish to remove.			-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB RemoveArrayElement(ArrayName, Data)
	/if (${outerDebug}) {
		/echo |- RemoveArrayElement ==>
		/echo |- RemoveArrayElement -|- Removing [${Data}] from [${ArrayName}]
	}

	/declare remainingElements string local
	/declare newArraySize int local
	/declare i int local

	|- Count array elements that do not match 'Data'.
	/for i 1 to ${${ArrayName}.Size}	
		/if (${${ArrayName}[${i}].NotEqual[${Data}]}) {
			/varset remainingElements ${remainingElements}${${ArrayName}[${i}]},
			/varcalc newArraySize ${newArraySize} + 1
		}
	/next i
	
	|- If there are  elements remaining in the array, create a copy of the new array
	/if (${newArraySize}) {
		/declare newArray[${newArraySize}] string local
		/for i 1 to ${newArraySize}
			/varset newArray[${i}] ${remainingElements.Arg[${i},,]}
		/next i
	}
	
	|- Delete the existing array.
	/deletevar ${ArrayName}
	
	|- If there is a new array to make
	/if (${Defined[newArray]}) {
		/declare ${ArrayName}[${newArray.Size}] string outer
		/for i 1 to ${newArray.Size}
			/varset ${ArrayName}[${i}] ${newArray[${i}]}
		/next i
	}
	
	/if (${outerDebug}) /echo <== RemoveArrayElement -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Read the timer in the ${iniTimers} file. Create a new line if it doesn't exist
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB readTimer(String castName)
	/declare itemTime int local
	/declare itemID int local
	/varset itemID ${FindItem[=${castName}].ID}
	/varset itemTime ${Ini[${iniToon},Timers,${itemID}]}
	/echo from iniToon = ${itemTime} (${castName})
	/if (!${Bool[${itemTime}]}) {
		/varset itemTime ${Ini[${iniTimers},${castName.Left[1].Upper},${itemID}]}
		/echo from iniTimers = ${itemTime} (${castName})
		/if (!${Bool[${itemTime}]}) {
			/ini "${iniToon}" "Timers" "${itemID}" "Need a value here !"
			/ini "${iniTimers}" "${castName.Left[1].Upper}" "${itemID}" "Need a value here !"
			/varset itemTime 0
		} else {
			/ini "${iniToon}" "Timers" "${itemID}" "${itemTime}"
		}
	}
/RETURN ${itemTime}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Reliably targets specified Target.IDs.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param TargetID: The ID of the spawn you wish to target.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB trueTarget(TargetID)
	/if (${outerDebug}) /echo |- trueTarget${If[${TargetID}, [${TargetID}],]} ==>

	|- Check if the target is in zone.
	/if (${SpawnCount[id ${TargetID}]}) {
		/declare retryTimer timer local 30
		
		:Target_Loop
		/squelch /target id ${TargetID}
		/delay 2 ${Target.ID} == ${TargetID}
		
		|- Check that I have the correct target ID.
		/if (${Target.ID} != ${TargetID} && ${SpawnCount[id ${TargetID}]}) {
			/if (${retryTimer}) {
				/goto :Target_Loop
			} else {
				/echo Failed to target [${Spawn[id ${TargetID}].CleanName} - ${TargetID}]
			}
		}
	}

	/if (${outerDebug}) /echo <== trueTarget -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Creates timers, at the specified duration, and adds their names to timerArray.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param timerName: Name of the timer you want to declare.							-|
|- @param timerSetting: Setting of the timer you're declaring.						-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createTimer(String timerName, String timerSetting)
	/if (${outerDebug}) /echo |- createTimer ==>

	|- Delete old timer.
	/if (${Defined[${timerName}]}) /deletevar ${timerName}

	|- Declare new timer.
	/declare ${timerName} timer global ${timerSetting}
	
	|- Add the timer's name to the timerArray.
	/call BuildArray timerArray ${timerName} global

	/if (${outerDebug}) {
		/echo created ${timerName} [${${timerName}}]
		/echo <== createTimer -|
	}
/RETURN

SUB createItemTimerAndAddToArray(String itemName, String arrayName)
	/if (!${Defined[itemID]}) /declare itemID int local
	/varset itemID ${FindItem[=${itemName}].ID}
	/if (${itemID}) {
		/call readTimer "${itemName}"
		|- readTimer gives item timer in seconds, need a conversion for createTimer
		/call createTimer "itemTimer_"${itemID} "${Macro.Return}s"
		/call BuildArray ${arrayName} ${itemID} outer
	} else {
		/echo Can't find ${itemName} !
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Deletes spent timers made by 'SUB createTimer'.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_macroTimers
	/if (${outerDebug}) /echo |- check_macroTimers ==>

	/declare i int local
	/declare SpentTimerCounter int local

	|- Check each entry of the timer array, count spent timers.	
	/for i 1 to ${timerArray.Size}
		/if (${outerDebug}) /echo Checking timer [${timerArray[${i}]} -- ${${timerArray[${i}]}}]
		/if (!${${timerArray[${i}]}}) {
			/varcalc SpentTimerCounter ${SpentTimerCounter} + 1
			/if (${Defined[${timerArray[${i}]}]}) {
				/if (${outerDebug}) /echo Deleting timer: ${timerArray[${i}]}
				/deletevar ${timerArray[${i}]}
			}
		}
	/next i
	
	|- If all the timers listed on timerArray are spent, delete timerArray.
	/if (${SpentTimerCounter}==${timerArray.Size}) {
		/if (${outerDebug}) /echo All timers have been cleared.
		/deletevar timerArray
	}
	
	/if (${outerDebug}) /echo <== check_macroTimers -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Auto inventories items from your cursor. 											-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Alerts the user, and ends the macro, if there is no free inventory space available.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ClearCursor
	/if (${outerDebug}) /echo |- ClearCursor ==>

	/declare i int local 1
	/declare dumpSlot string local
	
	|-If I have an item on my cursor, check what type of item it is.
	:auto_inv

	|-If the item is not a container.
	/if (!${Cursor.Container}) {
	
		|-If I have free inventory space, autoinventory the item.
		/if (${Me.FreeInventory}) {
			/autoinventory
			/delay 1
			
		|-Else, if I'm out of free inventory space, alert the user, and end the macro.
		} else {
			/chat ${channelNumber} I have run out of inventory space and I have an item on my cursor!!!
			/for i 1 to 5
				/beep
			/next i
			/endMacro
		}
		
	|-If the item is a container.
	} else {
		|-Check for a free pack slot.
		/for i 1 to 8
			/if (${InvSlot[pack${i}].Item.Container}) {
				/next i
			
			|-If I have no free pack slots, alert the user, and end the macro.
			/if (${InvSlot[pack8].Item.Container}) {
				/chat ${channelNumber} I have run out of inventory space and I have a bag on my cursor!!!
				/for i 1 to 7
					/beep
				/next i
				/endMacro
			}
		|-Else, if I have a free pack slot, drop the backpack.
		} else /if (!${InvSlot[pack${i}].Item.Container}) {
			/nomodkey /itemnotify pack${i} leftmouseup
		}
	}
	
	|-If I still have something on my cursor, try again to clear it.
	/if (${Cursor.ID}) /goto :auto_inv

	/if (${outerDebug}) /echo <== ClearCursor -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Triangulates distance between to points using the Pythagoras theorem.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param loc#1: Location 'A'												-|
|- @param loc#2: Location 'B'												-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Triangulate_Distance(string loc#1, string loc#2)
/RETURN ${Math.Sqrt[${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}*${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}+${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}*${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}]}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- MoveTo
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to utilize MQ2MoveUtils /moveto function, to move to a given destination.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param1 destination: Destinations can be provided as an ID, or loc, using the globalFollowing syntax:
|-
|- Move to a Spawn ID use, -SpawnID|#
|-		Ex.  /call MoveTo -SpawnID|${Target.ID}
|-
|- Move to a Location use, -Loc|Y,X
|-		Ex.  /call MoveTo -Loc|${Target.Y},${Target.X}
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MoveTo(destinationInfo, int distCheck)
	/if (${outerDebug}) /echo |- MoveTo ==>

	/declare X_Loc float local
	/declare Y_Loc float local
	/if (!${Defined[distCheck]}) /declare distCheck int local 10

	|-Set MoveTo variables per destinationType.
	/if (${destinationInfo.Find[-SpawnID]}) {
		/call trueTarget ${destinationInfo.Arg[2,|]}
		/varset Y_Loc ${Target.Y}
		/varset X_Loc ${Target.X}
	} else /if (${destinationInfo.Find[-Loc]}) {
		/varset Y_Loc ${destinationInfo.Arg[2,|].Arg[1,,]}
		/varset X_Loc ${destinationInfo.Arg[2,|].Arg[2,,]}
	} else {
		/echo [${destinationInfo}] does not specify a valid destinationType.  Use -SpawnID|#, or, -Loc|Y,X to note your intended destinationType.
	}

	|-Check if target is too far.
	/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
	/if (${Int[${Macro.Return}]} > ${MaxResponseDist}) {
		/echo [${Spawn[id ${destination}].CleanName}] is too far away.
	} else {
		|-Check if target is already close enough
		/if (${Int[${Macro.Return}]} < 10) {
			/if (${outerDebug}) /echo I am already at the destination.
		} else {
			|- Declare a timer based on how far you have to move (1s for every 50 units + 3).
			/declare retryTimer timer local ${Math.Calc[${Macro.Return} / 50 + 3].Int}s
			
			|- Engage MQ2MoveUtils.
			:retry_MoveToLoc
			/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /interrupt
			/squelch /moveto loc ${Y_Loc} ${X_Loc} dist ${If[${Defined[distCheck]},${distCheck},10]}
			/delay 10 ${Me.Moving} || ${MoveTo.Moving}
			
			:movingTo_Loop
			
			|- Debug Echo
			/if (${outerDebug} && !${outerSpamTimer_MTLoop}) {
				/echo |- MoveToSpawn -|- :MoveTo_Loop
				/call createTimer outerSpamTimer_MTLoop 50
			}

			/call check_BackgroundEvents
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
			
			/if (${MoveTo.Moving}) {
				/if (${Macro.Return} > ${distCheck} && ${retryTimer}) {
					/goto :movingTo_Loop
				} else {
					/squelch /stick off
					/if (${outerDebug} && !${outerSpamTimer_MTFAIL}) {
						/echo I have failed to move to the intended target.
						/call createTimer outerSpamTimer_MTFAIL 30
					}
				}
			} else {
				/if (${Macro.Return} > ${Math.Calc[${distCheck} + ${Math.Calc[${distCheck} / 4].Int}].Int}) {
					/if (${retryTimer}) {
						/goto :retry_MoveToLoc
					} else {
						/if (${outerDebug} && !${outerSpamTimer_MTFAIL}) {
							/echo I have failed to move to the intended target.
							/call createTimer outerSpamTimer_MTFAIL 30
						}
					}
				} else {
					/squelch /stick off
					/if (${outerDebug}) /echo I have arrived at my intended destination.
					/delay 30 !${Me.Moving}
				}
			}
		}
	}

	/if (${outerDebug}) /echo <== MoveTo -|
/RETURN

SUB argueString(String WhatToFind, String givenData)
	/if (${outerDebug}) /echo |- argueString ==>

	/declare i int local 1
	/declare foundData string local NULL
	
	:nextArg
	/if (${givenData.Find[/]}) {
		/if (${givenData.Arg[${i},/].Find[${WhatToFind}]}) {
			/varset foundData ${givenData.Arg[${i},/].Arg[2,|]}
			/if (${outerDebug}) /echo |- argueString -|- Found [${WhatToFind}] - [${foundData}]
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i},/].Length}) /goto :nextArg
		}
	} else {
		/if (${givenData.Arg[${i}, ].Find[${WhatToFind}]}) {
			/varset foundData ${givenData.Arg[${i}, ].Arg[2,|]}
			/if (${outerDebug}) /echo |- argueString -|- Found [${WhatToFind}] - [${foundData}]
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i}, ].Length}) /goto :nextArg
		}
	}

	/if (${outerDebug}) /echo <== argueString -|
/RETURN ${foundData}

SUB check_selectedBots(selectedBots, ChatSender)
	/declare botSelected bool local FALSE
	/if (${selectedBots.Find[ ]}) /varset selectedBots ${selectedBots.Replace[ ,,]}
	/if (${Select[priests,${selectedBots}]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${Select[healers,${selectedBots}]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${Select[ranged,${selectedBots}]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${Select[casters,${selectedBots}]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${Select[melee,${selectedBots}]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${Select[me,${selectedBots}]} && ${ChatSender.Equal[${Me.CleanName}]} || ${Select[${Me.Class},${selectedBots}]} || ${Select[${Me.Class.ShortName},${selectedBots}]} || ${Select[${Me.CleanName},${selectedBots}]} || ${Select[all,${selectedBots}]} || ${Select[group,${selectedBots}]} && ${Me.CleanName.Equal[${ChatSender}]} || ${Select[group,${selectedBots}]} && ${Group.Member[${ChatSender}]} || ${Select[zone,${selectedBots}]} && ${SpawnCount[pc ${ChatSender}]}) /varset botSelected TRUE
/RETURN ${botSelected}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check if a toon need an item in the list
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB thf_LootManager(int targetID, String lootList)
	/declare i int local
	/declare j int local
	/declare iniItem string local
	/split "${lootList}" itemsList outer ","

	/for i 1 to ${NetBots.Counts}
		/for j 1 to ${itemsList.Size}
			|- Réussir à chopper la Class.ShortName et le CleanName du netbot
			/varset iniItem ${Ini["thf_inis\${MacroQuest.Server}-${NetBots[${i}].Class.ShortName}-${NetBots[${i}].Name}.ini",Loot,${itemsList[${j}]}]}
			/if (${Int[${iniItem}] > 0) {
				/bct ${NetBots[${i}].Name} needLoot ${iniItem} on ${targetID}
			}
		/next j
	/next i
/RETURN

#EVENT macroHelp "[MQ2] Help"
#EVENT macroHelp "<#*#> Help"
#EVENT macroHelp "[MQ2] Help #1#"
#EVENT macroHelp "<#*#> Help #1#"
SUB EVENT_macroHelp(line, chapter)
/if (${outerDebug}) /echo |- EVENT_macroHelp ==>

	/declare help_Ini string local e3 Includes\e3 Help.ini
	/declare i int local 1
	/declare e int local 1
	
	/if (!${Defined[chapter]}) {
		
		/echo Welcome to e3's in game help function.  Please choose a topic you'd like to know more about.

		/echo ----------------------------------------
		:next_Chapter
		/if (${Ini[${help_Ini}].Arg[${i},|].Length}) {
			
			/if (${Ini[${help_Ini}].Arg[${i},|].NotEqual[macroResponses]}) /echo [${Ini[${help_Ini}].Arg[${i},|]}]
			/varcalc i ${i} + 1
			/goto :next_Chapter
		}
	} else {
	
		|- if the entry is not found
		/if (!${Ini[${help_Ini},${chapter}].Length}) {
			/echo Could not find an entry for [${chapter}].
		} else {
		
			:next_Topic
			/if (${Ini[${help_Ini},${chapter}].Arg[${i},|].Length}) {
				
				/echo ----------------------------------------
				/echo    ( ${Ini[${help_Ini},${chapter}].Arg[${i},|]} )
				/if (${Ini[${help_Ini},${chapter},${Ini[${help_Ini},${chapter}].Arg[${i},|]}].Length}) {
	
					/varset e 1
					:next_helpEcho
					/if (${Ini[${help_Ini},${chapter},${Ini[${help_Ini},${chapter}].Arg[${i},|]}].Arg[${e},^].Length}) {
						/echo ${Ini[${help_Ini},${chapter},${Ini[${help_Ini},${chapter}].Arg[${i},|]}].Arg[${e},^]}
						
						/varcalc e ${e} + 1
						/goto :next_helpEcho
					}
				}

				/varcalc i ${i} + 1
				/goto :next_Topic
			}
			/echo ----------------------------------------
		}
	}
	
/if (${outerDebug}) /echo <== EVENT_macroHelp -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Build an ini file, of a given name, that contains alphabetized sections.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_File: Name of the ini file you want to create.				-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Build_Alphabetized_Ini(Ini_File)
/if (${outerDebug}) /echo |- Build_Alphabetized_Ini ==>

	/ini "${Ini_File}" A "A is for"
	/ini "${Ini_File}" B "B is for"
	/ini "${Ini_File}" C "C is for"
	/ini "${Ini_File}" D "D is for"
	/ini "${Ini_File}" E "E is for"
	/ini "${Ini_File}" F "F is for"
	/ini "${Ini_File}" G "G is for"
	/ini "${Ini_File}" H "H is for"
	/ini "${Ini_File}" I "I is for"
	/ini "${Ini_File}" J "J is for"
	/ini "${Ini_File}" K "K is for"
	/ini "${Ini_File}" L "L is for"
	/ini "${Ini_File}" M "M is for"
	/ini "${Ini_File}" N "N is for"
	/ini "${Ini_File}" O "O is for"
	/ini "${Ini_File}" P "P is for"
	/ini "${Ini_File}" Q "Q is for"
	/ini "${Ini_File}" R "R is for"
	/ini "${Ini_File}" S "S is for"
	/ini "${Ini_File}" T "T is for"
	/ini "${Ini_File}" U "U is for"
	/ini "${Ini_File}" V "V is for"
	/ini "${Ini_File}" W "W is for"
	/ini "${Ini_File}" X "X is for"
	/ini "${Ini_File}" Y "Y is for"
	/ini "${Ini_File}" Z "Z is for"
  
/if (${outerDebug}) /echo |- Build_Alphabetized_Ini ==>
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Build an ini file, of a given name, that contains alphabetized sections.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param Ini_File: Name of the ini file you want to create.				-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Build_Class_Ini(Ini_File)
/if (${outerDebug}) /echo |- Build_Class_Ini ==>

	/ini "${Ini_File}" Bard "Bard is for"
	/ini "${Ini_File}" Beastlord "Beastlord is for"
	/ini "${Ini_File}" Berserker "Berserker is for"
	/ini "${Ini_File}" Cleric "Cleric is for"
	/ini "${Ini_File}" Druid "Druid is for"
	/ini "${Ini_File}" Enchanter "Enchanter is for"
	/ini "${Ini_File}" Magician "Magician is for"
	/ini "${Ini_File}" Monk "Monk is for"
	/ini "${Ini_File}" Necromancer "Necromancer is for"
	/ini "${Ini_File}" Paladin "Paladin is for"
	/ini "${Ini_File}" Ranger "Ranger is for"
	/ini "${Ini_File}" Rogue "Rogue is for"
	/ini "${Ini_File}" Shadowknight "Shadowknight is for"
	/ini "${Ini_File}" Shaman "Shaman is for"
	/ini "${Ini_File}" Warrior "Warrior is for"
	/ini "${Ini_File}" Wizard "Wizard is for"
  
/if (${outerDebug}) /echo |- Build_Class_Ini ==>
/RETURN

SUB checkToons()
	/declare i int local 0
	/declare count int local 0

	/if (!${Defined[toonsInZone]}) /declare toonsInZone int outer 0

	/for i 0 to ${NetBots.Counts}
		/if (${NetBots[${NetBots.Client.Arg[${i}]}].InZone}) {
			/varcalc count ${count}+1
		}
	/next i
	
	/varset toonsInZone ${count}
/RETURN

|**

e3 event template


e3 .inc template


|---------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------|
|- _Setup														-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------|
|- Imports and validates ini settings from Character Ini,[].	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------|
SUB _Setup
/if (${outerDebug}) /echo |- _Setup ==>

/if (${outerDebug}) /echo <== _Setup -|
/RETURN


 
SUB _BackgroundEvents

/RETURN



SUB _MacroSettings
/if (${outerDebug}) /echo |- _MacroSettings ==>

/if (${outerDebug}) /echo <== _MacroSettings -|
/RETURN



SUB _CharacterSettings
/if (${outerDebug}) /echo |- _CharacterSettings ==>

/if (${outerDebug}) /echo <== _CharacterSettings -|
/RETURN



SUB _Aliases
/if (${outerDebug}) /echo |- _Aliases ==>

/if (${outerDebug}) /echo <== _Aliases -|
/RETURN

**|



|** deprecated SCRIPTS (Keep for reference)---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- MoveToSpawn																					-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Tries for up to 10 seconds to move to a given spawn ID, using MQ2MoveUtils /moveto function.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Bots must have line of sight on their MoveTo target.											-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MoveToSpawn(MoveToID)
	/if (${outerDebug}) /echo |- MoveToSpawn ==>

	|- Check LoS on the MoveToID.
	/if (!${Spawn[id ${MoveToID}].LineOfSight}) {
		/echo I cannot see ${Spawn[id ${MoveToID}].CleanName}
	} else {
		
		|- Declare a 10 second timer.
		/declare retryTimer timer local 100
	
		|- Target the MoveToID
		/call trueTarget ${MoveToID}
		
		/if (${Target.ID} == ${MoveToID}) {
			
			:MoveTo_Loop
			/if (${outerDebug}) /echo |- MoveToSpawn -|- :MoveTo_Loop
		
			|- Engage MQ2MoveUtils.
			/moveto ID ${Target.ID} dist 7
			
			|- Wait to start moving.
			/delay 10 ${Me.Moving}
			
			|- Wait to stop moving.
			/delay 50 !${Me.Moving}
			
			|- Check target range.
			/if (${Target.Distance} > 20) {
			
				|- If there is time to try again.
				/if (${retryTimer}) {
					/goto :MoveTo_Loop
				
				|- Alert user, and return.
				} else {
					/echo ERROR: @MoveToSpawn - Failed to move to ${Spawn[id ${MoveToID}].CleanName}
					/chat ${channelNumber} ERROR: @MoveToSpawn - Failed to move to ${Spawn[id ${MoveToID}].CleanName}
				}
					
			}
		}
	}

/if (${outerDebug}) /echo <== MoveToSpawn -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Runs to a given location.							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param y_loc: Y intercept of your next destination.	-|
|- @param x_loc: X intercept of your next destination.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MoveToLoc(float y_loc, float x_loc)
/if (${outerDebug}) {
	/echo |- MoveToLoc ==>
	/echo Moving to location [${y_loc},${x_loc}]
}
	/if (!${LineOfSight[${Me.Y},${Me.X}:${y_loc},${x_loc}]}) {
		/chat ${channelNumber} I do not have line of sight on my destination.
	} else {
		
		/declare retryTimer timer local 150
		/declare startingLoc string local ${Me.Loc.Replace[ ,]}
		
	|---------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------Engage MQ2MoveUtils to move to the specified location.
		/squelch /moveto Loc ${y_loc} ${x_loc} dist 2
		/delay 10 ${Me.Moving}
		
	|---------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------Wait to stop moving.
		:wait4Stop
		/if (${outerDebug}) /echo |- MoveToLoc -|- :wait4Stop
		
	|---------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------Check if movement events were called.
		/call check_BackgroundEvents
		
	|---------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------If I'm still moving, wait to stop.
		/if (${Me.Moving}) {
			/if (${retryTimer}) {
				/goto :wait4Stop
			} else {
				/squelch /stick off
			}
		}

	|---------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------Calulate distance to desired location.
		/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${y_loc},${x_loc}
		/if (${Macro.Return} > 15) /chat ${channelNumber} I did not arrive at the desired location.
	}
	
/if (${outerDebug}) /echo <== MoveToLoc -|
/RETURN




---------------------------------------------------------------------------------------------------------------------------------------------->
- This method of validation was abandoned due to excessive overhead on startup, and being a downright pain in my ass...

SUB rewrite_ValidationKey(int newValue, int CurrentArg)
/if (${outerDebug}) /echo |- rewrite_ValidationKey ==>

	/if (!${Ini[${iniTHF},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length}) {
		/if (${outerDebug}) /echo |- rewrite_ValidationKey -|- Creating key: [${newValue}].
		/call writeToIni "${iniTHF},ValidationKey-${MacroQuest.Server},${Me.CleanName}" "${newValue}" 1	
	} else {
	
		/declare i int local
		/declare old_Key string local ${Ini[${iniTHF},ValidationKey-${MacroQuest.Server},${Me.CleanName}]}
		/declare new_Key string local
		
		/for i 1 to ${CurrentArg}
		
			/if (${i} < ${CurrentArg}) {
				/varset new_Key ${new_Key}${Int[${old_Key.Arg[${i},-]}]}-
			} else {
				/varset new_Key ${new_Key}${newValue}-
			}
		
		/next i
		
		/varcalc i ${CurrentArg} + 1
		
		:check_NextArg
		/if (${Ini[${iniTHF},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${i},-].Length}) {
			/varset new_Key ${new_Key}${Int[${old_Key.Arg[${i},-]}]}- 
			/varcalc i ${i} + 1
			/goto :check_NextArg
		}
		/if (${outerDebug}) /echo |- rewrite_ValidationKey -|- Rewriting key: [${new_Key}] 
		/call writeToIni "${iniTHF},ValidationKey-${MacroQuest.Server},${Me.CleanName}" "${new_Key}" 1
	}

/if (${outerDebug}) /echo <== rewrite_ValidationKey -|
/RETURN

---------------------------------------------------------------------------------------------------------------------------------------------->
