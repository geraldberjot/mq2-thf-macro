
Sub loadCustomVars
	/declare stickCommand string outer ${Ini[${iniGlobal},Global,stickCommand]}
/return

Sub joinChannel
	/join Team
	/doevents
	:channelLoop
	/if (${channel} == 0) {
		/doevents
		/goto :channelLoop
	}
/return

Sub isHolyShit
	/if (${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/return TRUE
	}
/return FALSE

Sub isDownShit
	/if (!${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/return TRUE
	}
/return FALSE

Sub instantBuff
	/declare i int local
	/declare myInstantList string local ${Ini[${iniSetup},InstantBuff]}
	/declare myInstantListCount int local ${myInstantList.Count[|]}
	/varcalc myInstantListCount ${myInstantListCount}-1
	/declare myInstantLine string local
	/declare myInstant string local
	/declare mySlot string local
	/declare myInstantSpell string local

	/if (${myInstantListCount} > 0) {
		/for i 1 to ${myInstantListCount}
			/varset mySlot ${myInstantList.Token[${i},|]}
			/varset myInstantLine ${Ini[${iniSetup},InstantBuff,${mySlot}]}
			| en 1, l'item
			/varset myInstant ${myInstantLine.Token[1,/]}
			/varset myInstantSpell ${FindItem[=${myInstant}].Spell}
			/if (${Spell[${myInstantSpell}].Stacks} && !${Me.Buff[${myInstantSpell}].ID}) {
				/casting "${myInstant}" ${mySlot}
			}
		/next i
	}
/return

Sub checkSpellBuff
	/declare i int local
	/declare targetID int local
	/declare mySpellList string local ${Ini[${iniSetup},SpellBuff]}
	/declare mySpellListCount int local
	/varcalc mySpellListCount ${mySpellList.Count[|]}-1
	/declare mySpellLine string local
	/declare mySpell string local
	/declare myGem string local
	/declare targetName string local

	/if (${mySpellListCount} > 0) {
		/for i 1 to ${mySpellListCount}
			/varset myGem ${mySpellList.Token[${i},|]}
			/varset mySpellLine ${Ini[${iniSetup},SpellBuff,${myGem}]}
			/varset mySpell ${mySpellLine.Token[1,/]}
			/varset targetName ${mySpellLine.Token[2,/]}
			| Si on a une target pour le spell, c'est que ce n'est pas un group spell
			/if (${targetName.Length}) {
				/varset targetID ${Spawn[pc ${targetName} los radius ${Spell[${mySpell}].MyRange}].ID}
				/if (${Me.CurrentMana} > ${Spell[${mySpell}].Mana} && ${Me.SpellReady[${mySpell}]} && ${targetID} && !${Me.Moving}) {
					/call checkTargetNeedBuff ${targetName} ${Spell[${mySpell}].ID}
					/if (${Macro.Return}) {
						/if (${targetID}) {
							/chat ${channel} casting "${mySpell}" on [${targetName}]
							/casting "${mySpell}" ${myGem} -maxtries|3 -targetid|${Spawn[pc ${targetName}].ID}
							/delay 3s
						}
					}
				}
			} else {
				/if (${Spell[${mySpell}].Mana} > ${Me.CurrentMana} && ${Spell[${mySpell}].Stacks} && !${Me.Moving}) {
					| Verifier la presence de tous les membres du groupe avant de caster le bazar
					/call checkGroupNeedBuff ${Spell[${mySpell}].ID}
					/if (${Macro.Return}) {
						/chat ${channel} casting "${mySpell}"
						/casting "${mySpell}" ${myGem} -maxtries|3
					}
				}
			}
		/next i
	}
/return

Sub checkTargetNeedBuff(string targetName, int spellID)
	/declare i int local
	/declare targetBuffs string local
	/declare targetBuff	string local
	
	/varset targetBuffs ${NetBots[${targetName}].Buff}
	/if (${targetBuffs.Find[${spellID}]}) {
		/return FALSE
	}
	/for i 1 to ${targetBuffs.Count[ ]} - 1
		/varset targetBuff ${targetBuffs.Token[${i}, ]}
		/if (${targetBuff.Length}) {
			/if (!${Spell[${spellID}].WillStack[${Spell[${targetBuff}]}]}) {
				/chat ${channel} '${Spell[${spellID}].Name}' won't stack with '${Spell[${targetBuff}]}' !
				/return FALSE
			}
		}
	/next i
/return TRUE

Sub checkGroupNeedBuff(int spellID)

/return FALSE

Sub checkDiscipline
	/declare i int local
	/declare myDiscList string local ${Ini[${iniSetup},Discipline]}
	/declare myDiscListCount int local
	/varcalc myDiscListCount ${myDiscList.Count[|]}-1
	/declare myDisc string local
	/declare myNumber string local
	
	/if (${myDiscListCount} > 0) {
		/for i 1 to ${myDiscListCount}
			/varset myNumber ${myDiscList.Token[${i},|]}
			/varset myDisc ${Ini[${iniSetup},Discipline,${myNumber}]}
			/if (${Me.CombatAbilityReady[${myDisc}]} && ${Target.PctHPs}<99) {
				/disc ${myDisc}
			}
		/next i
	}
/return

Sub checkHolyShitBuff
	/declare i int local
	/declare j int local
	/declare myHSBList string local ${Ini[${iniSetup},HolyShitBuff]}
	/declare myHSBListCount int local ${myHSBList.Count[|]}
	/varcalc myHSBListCount ${myHSBListCount}-1
	/declare myHSBLine string local
	/declare myHSB string local | HolyShitBuff
	/declare myHSS string local | HolyShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local
	/declare NewString string local

	/if (${myHSBListCount} > 0) {
		/for i 1 to ${myHSBListCount}
			/varset mySlot ${myHSBList.Token[${i},|]}
			/varset myHSBLine ${Ini[${iniSetup},HolyShitBuff,${mySlot}]}
			| en 1, l'item
			/varset myHSB ${myHSBLine.Token[1,/]}
			| en 2, le timer
			/varset recastTime ${myHSBLine.Token[2,/]}
			| en 3, la target
			/varset targetName ${myHSBLine.Token[3,/]}
			/varset itemCleanName ${myHSB.Replace[ ,].Replace[',].Replace[.,]}
			/if (${itemCleanName.Find[,]}) {
				/varset NewString
				/for j 1 to ${itemCleanName.Length}
					/if (${itemCleanName.Mid[${j},1].NotEqual[,]}) {
						/varset NewString ${NewString}${itemCleanName.Mid[${j},1]}
					}
				/next j
				/varset itemCleanName ${NewString}
			}
			/if (!${Defined[${itemCleanName}Timer]}) /declare ${itemCleanName}Timer timer outer 0
			/varset myHSS ${FindItem[=${myHSB}].Spell}
			/if (${Spell[${myHSS}].Stacks} && !${Me.Buff[${myHSS}].ID} && ${${itemCleanName}Timer} <= 0 && !${Me.Moving}) {
				/echo Casting "${myHSB}" ${mySlot}
				/casting "${myHSB}" ${mySlot}
				/varset ${itemCleanName}Timer ${recastTime}
			} else {
				/if (${${itemCleanName}Timer} <= 0 && !${Me.Moving} && ${itemCleanName.Lower.Equal[artoriastheabysswalker]}) {
					/echo Casting "${myHSB}" ${mySlot}
					/casting "${myHSB}" ${mySlot}
					/varset ${itemCleanName}Timer ${recastTime}
				}
			}
		/next i
	}
/return

Sub checkDownShitBuff
	/declare i int local
	/declare myDSBList string local ${Ini[${iniSetup},DownShitBuff]}
	/declare myDSBListCount int local ${myDSBList.Count[|]}
	/varcalc myDSBListCount ${myDSBListCount}-1
	/declare myDSBLine string local
	/declare myDSB string local | DownShitBuff
	/declare myDSS string local | DownShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local

	/if (${myDSBListCount} > 0) {
		/for i 1 to ${myDSBListCount}
			/varset mySlot ${myDSBList.Token[${i},|]}
			/varset myDSBLine ${Ini[${iniSetup},DownShitBuff,${mySlot}]}
			| en 1, l'item
			/varset myDSB ${myDSBLine.Token[1,/]}
			| en 2, le timer
			/varset recastTime ${myDSBLine.Token[2,/]}
			| en 3, la target
			/varset targetName ${myDSBLine.Token[3,/]}
			/varset itemCleanName ${myDSB.Replace[ ,].Replace[',].Replace[.,]}
			/if (!${Defined[${itemCleanName}Timer]}) /declare ${itemCleanName}Timer timer outer 0
			/varset myDSS ${FindItem[=${myDSB}].Spell}
			/if (${Spell[${myDSS}].Stacks} && !${Me.Buff[${myDSS}].ID} && ${${itemCleanName}Timer} <= 0 && !${Me.Moving}) {
				/casting "${myDSB}" ${mySlot} -targetid|${Spawn[pc ${targetName}].ID}
				/varset ${itemCleanName}Timer ${recastTime}
				/delay 3s
			}
		/next i
	}
/return

Sub EVENT_Dead
	|/call Wait4Rez
	/ssm max
/return

Sub EVENT_Camp
	/echo Camping...
	/end
/return

Sub EVENT_Hail(String Line, String Cible)
	/declare toTheZone string local
	/varset toTheZone ${Ini[${iniHail},General,${Cible.Lower}]}
	/if (${toTheZone.Length} && ${toTheZone.NotEqual[NULL]}) {
		/say ${toTheZone}
	} else {
		/echo ${Line}
	}
	/return

Sub EVENT_Task(String Line, String Tache)
	/tell ${myMain} ${Line}
/return

Sub EVENT_Assign(String Line, String Tache)
	/tell ${myMain} ${Line}
/return

Sub checkPlugIn
	/if (!${Plugin[MQ2Cast].Name.Length}) {
		/echo MQ2Cast is a REQUIRED PLUGIN
		/echo Please Load MQ2Cast before running this macro
		/end
	}
/return

Sub EVENT_Tell(Line, string From, string Msg)
	/if (${From.Find[${myMain}]}) {
		/if (${Msg.Equal[loot]}) {
			/call LootMobs
		}
		/if (${Msg.Lower.Find[find].NotEqual[NULL]}) {
			
		}
	} else {
		/if (${Me.CleanName.NotEqual[${myMain}]}) {
			/chat ${channel} ${Line}
		}
	}
/return

Sub EVENT_Group(Line, string From)
	/if (${From.Find[${myMain}]}) {
		/if (!${Me.Grouped}) {
			/invite
		}
	}
/return

Sub EVENT_DraekonVoice(string Line, string targetName)
	/moveto id ${Spawn[npc ${targetName}].ID}
/return


Sub EVENT_DraekonVoiceEnd(string Line, string targetName)
	/moveto id ${Spawn[npc Draekon].ID}
/return


Sub EVENT_DraekonNoGrass
	/if (${Me.Class.Name.Equal[Warrior]} || ${Me.Class.Name.Equal[Paladin]} || ${Me.Class.Name.Equal[Shadowknight]}) /return

	/declare EndX float local			-1609.88
	/declare EndY float local			308.23

	/call InternalMoveTo ${EndY} ${EndX}
/return


Sub EVENT_DraekonGrass
	/moveto id ${Spawn[npc Draekon].ID}
/return


Sub EVENT_DraekonBelly
	/moveto id ${Spawn[npc Draekon].ID}
/return

Sub InternalMoveTo(int y, int x)

	/declare currentX int local 0
	/declare currentY int local 0
	/declare currentZ int local 0
	/moveto loc ${y} ${x}
:waitUntilMovingFinished
	/if (${Math.Distance[${y},${x},${Me.Z}]} > 15 || ${Math.Distance[${currentY},${currentX},${currentZ}]} > 5) {
		/varset currentX ${Me.X}
		/varset currentY ${Me.Y}
		/varset currentZ ${Me.Z}
		/delay 1
		/doevents
		/goto :waitUntilMovingFinished
	}

/return

Sub isOneOfMyToons(String from)
	/if (${NetBots.Client.Find[${from}]}) {
		/return TRUE
	}
/return FALSE

Sub cleanToonName(String toonName)
	/if (${NetBots.Client.Find[${toonName}]}) {
		/return ${toonName}
	}
	/if (${NetBots.Client.Find[${toonName.Right[-2].Left[-1]}]}) {
		/return ${toonName.Right[-2].Left[-1]}
	}
/return ${toonName}

Sub sticking(String to)
    /if (${Spawn[${to}].ID} == ${Me.ID}) {
        /return
    }
	/call stopCurrentAction FALSE
    /target ${to}
    /face fast
    /delay 1s
    /stick ${stickCommand}
    /chat ${channel} Sticking to ${to}
/return

| Allow toon to gate to the Forest
Sub gate(String from, String who)
	/echo gate
    /call amIconcerned ${from} ${who}
    /if (${Macro.Return}) {
        /call stopCurrentAction TRUE
        /declare CharmOfQueenAdrianne string local ${FindItem[Charm of Queen Adrianne].Name}
        /if (${CharmOfQueenAdrianne.Length} > 0 && !${CharmOfQueenAdrianne.Equal[NULL]}) {
            /casting "${CharmOfQueenAdrianne}" charm
            /chat ${channel} Gating to the Forest !
        }
    }
/return

| stick off, attack off, interrupt, twist off (if specify)
Sub stopCurrentAction(bool evenBard)
	/echo stopCurrentAction ${evenBard}
    /if (${evenBard}) {
        /twist off
    }
	/target clear
    /stick off
    /attack off
    /interrupt
    /delay 1
/return

Sub amIMelee
	/if (${Me.Class.ShortName.Equal[WAR]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[ROG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BSK]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[MNK]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BRD]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[PAL]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[SHD]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[RNG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BST]) {
		/return TRUE
	}
/return FALSE

Sub amINuker
	/if (${Me.Class.ShortName.Equal[WIZ]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[MAG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[ENC]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[NEC]) {
		/return TRUE
	}
/return FALSE

Sub amIconcerned(String from, String who)
	/echo amIconcerned ${from} ${who}
	/if (${who.Equal[all]}) {
		/return TRUE
	}
	/if (${who.Equal[group]}) {
		/if (${Me.Grouped} && ${Group.Member[${from}]}) {
			/return TRUE
		}
	}
	/if (${who.Equal[caster]}) {
		| Can cast spells, including Bard 
		/if (${Me.Class.CanCast}) {
			/return TRUE
		}
	}
	/if (${who.Equal[melee]}) {
		| Monk/Rogue/Bard/Ranger/Warrior/Paladin/ShadowKnight?
		/call amIMelee
		/return ${Macro.Return}
	}
	/if (${who.Equal[priest]}) {
		| Cleric/Druid/Shaman? 
		/if (${Me.Class.HealerType}) {
			/return TRUE
		}
	}
	/if (${who.Equal[nuker]}) {
		| Wizard/Magician/Necromancer/Enchanter?
		/call amINuker
		/return ${Macro.Return}
	}
	/if (${who.Equal[${Me.Class.Name}]} || ${who.Equal[${Me.Class.ShortName}]}) {
		/return TRUE
	}
	/if (${who.Equal[${Me.CleanName}]}) {
		/return TRUE
	}
/return FALSE

Sub makeGroup
	/if (${Me.CleanName.Equal[conradd]}) {
		/invite tiarra
		/delay 1
		/invite tifania
		/delay 1
		/invite brocelia
		/delay 1
		/invite gaya
		/delay 1
		/invite houdi
		/return
	}
	/if (${Me.CleanName.Equal[vivyana]}) {
		/invite arsen
		/delay 1
		/invite aiguisee
		/delay 1
		/invite fandril
		/delay 1
		/invite shoalin
		/delay 1
		/invite korale
		/return
	}
	/if (${Me.CleanName.Equal[namea]}) {
		/invite margona
		/delay 1
		/invite mirlen
		/delay 1
		/invite elmunster
		/delay 1
		/invite malefica
		/delay 1
		/invite aede
		/return
	}
/return

Sub LPS
	| ----------------------------------------------------------------------
	| Declare loops / second variables
	| ----------------------------------------------------------------------
	/if (!${Defined[looptest]}) 	/declare looptest 	int outer 0
	/if (!${Defined[loops]}) 		/declare loops 		int outer 0
	/if (!${Defined[looptime]}) 	/declare looptime 	int outer ${Time.Second}
	/if (!${Defined[loopsecmax]}) 	/declare loopsecmax int outer 0
	/if (!${Defined[loopsecmin]}) 	/declare loopsecmin int outer 10000
	/if (!${Defined[loopcount]}) 	/declare loopcount 	int outer 0

	/if (${Time.Second}>=${Math.Calc[${looptime}+1]}) {
		/if (${looptest}>${loopsecmax}) /varset loopsecmax ${looptest}
		/if (${looptest}<${loopsecmin} && ${looptest}) /varset loopsecmin ${looptest}
		/varset loops ${looptest}
		/varset looptest 0
		/varset looptime ${Time.Second}
	} else /if (${Time.Second}==0 && ${looptime}!=0) {
		/echo LPS:${looptest} loop/sec min:${loopsecmin} max:${loopsecmax}
		/varset looptest 0
		/varset looptime 0
		/varset loopsecmax 0
		/varset loopsecmin 10000
	}
/return

| Allow toon to the guild hall
Sub goToGuildHall(String from, String who)
	/call gate ${from} ${who}
    /call amIconcerned ${from} ${who}
    /if (${Macro.Return}) {
        /call stopCurrentAction TRUE
        /declare CharmOfQueenAdrianne string local ${FindItem[Charm of Queen Adrianne].Name}
        /if (${CharmOfQueenAdrianne.Length} > 0 && !${CharmOfQueenAdrianne.Equal[NULL]}) {
            /casting "${CharmOfQueenAdrianne}" charm
            /chat ${channel} Gating to the Forest !
        }
    }
/return

Sub test()
	| Fermer toutes les fenetres ouvertes d'abord !
	/notify InventoryWindow InvSlot13 rightmouseheld
	/delay 1
	/keypress Enter
	/shiftkey /keypress . chat
	/nomodkey /keypress c chat
	/nomodkey /keypress h chat
	/nomodkey /keypress q chat
	/nomodkey /keypress t chat
	/nomodkey /keypress space chat
	/shiftkey /keypress ${channel} chat
	/keypress Space chat
	/delay 1
	/notify ItemDisplayWindow IconButton leftmouseup
	/delay 1
	/keypress Enter chat
	/delay 1
	/notify ItemDisplayWindow 0 close 
/return

Sub endMacro
	/deletevar * global
	/disband
	/raiddisband
	/end
/return

SUB halloween
	/if (!${Defined[Halloween]})		/declare Halloween 			boolean outer true
	/if (!${Defined[HalloweenZones]})	/declare HalloweenZones[7] 	int outer NULL
	
	/declare NormalZoneNpcID 				int local 0
	/declare HalloweenMerchantID 			int local 0
	/declare HalloweenRaidTokenNpcID 		int local 0
	/declare MistmooreTrickyAdvisorID 		int local 0
	/declare MistmooreTrickiestAdvisorID 	int local 0
	/declare LesserFaydarkFirenzellaID 		int local 0
	/declare LesserFaydarkShadyID	 		int local 0
	/declare LesserFaydarkTrickyID 			int local 0
	/declare LesserFaydarkGhoulieID 		int local 0
	/declare LesserFaydarkFrighteningID 	int local 0
	
	
	/declare i int local
	
	| On gate pour initialiser les actions
	/if (${HalloweenZones[1].Equal[NULL]}) {
		/varset HalloweenZones[1] 0
		/call gate ${Me.CleanName} ${Me.CleanName}
	}
	| On commence le manège
	/if (${HalloweenZones[1] == 0}) {
		| On est dans THF, il faut récupérer les costumes
		/call MoveTo -Loc|Y,X | On se met au milieu de la cave
		/call MoveTo -Loc|Y,X | On sort de la cave, milieu de THF
		/call MoveTo -Loc|Y,X | On se place dans la zone du buffer
		/varset HalloweenMerchantID ${Spawn[].ID} | le nom du marchand de costume
		/call MoveTo -SpawnID|${HalloweenMerchantID}
		| On achète les costumes au marchand
			| On ouvre la fenêtre de trade
			/target id ${HalloweenMerchantID}
			/stick 5
			/delay 30
			/nomodkey /click right target
			/delay 10
			| On achète
			/call Buy "Fairy Costume Kit" 5
			/call Buy "Gargoyle Costume Kit" 1
			/call Buy "Scarecrow Costume Kit" 2
		| On va chercher la quête
		/call MoveTo -Loc|Y,X | On sort de la cave, milieu de THF
		/varset HalloweenRaidTokenNpcID ${Spawn[Fitzina].ID}
		/call MoveTo -SpawnID|${HalloweenRaidTokenNpcID} | l'ID de Fitzina
		/casting "Fairy Costume Kit" item -maxtries|3
		/delay 1s
		/target id ${HalloweenRaidTokenNpcID}
		/delay 1s
		/hail
		/call MoveTo -Loc|Y,X | On sort de la cave, milieu de THF
		/varset NormalZoneNpcID ${Spawn[].ID} | le nom de la fée des "normal zones"
		/call MoveTo -SpawnID|${NormalZoneNpcID}
		/target ID ${NormalZoneNpcID}
		/delay 1s
		/varset HalloweenZones[1] ${Zone.ID}
		/varset HalloweenZones[2] 0
		/say mistmoore
	}
	| In mistmoore
	/if (${HalloweenZones[2] == 0}) {
		/varset MistmooreTrickyAdvisorID ${Spawn[a tricky advisor].ID}
		/call MoveTo -SpawnID|${MistmooreTrickyAdvisorID}
		/casting "Gargoyle Costume Kit" item -maxtries|3
		/delay 1s
		/target ID ${MistmooreTrickyAdvisorID}
		/delay 1s
		/for i 1 to 12
			/hail
			/delay 1
		/next i
		/varset MistmooreTrickiestAdvisorID ${Spawn[a trickiest advisor].ID}
		/call MoveTo -SpawnID|${MistmooreTrickiestAdvisorID}
		/delay 1
		/target ID ${MistmooreTrickiestAdvisorID}
		/delay 1
		/hail
		/delay 1
		/call MoveTo -SpawnID|${MistmooreTrickyAdvisorID}
		/call MoveTo -SpawnID|${Spawn[].ID} | la nana à l'entrée de mistmoore
		/varset HalloweenZones[2] ${Zone.ID}
		/varset HalloweenZones[3] 0
		/call MoveTo -Loc|Y,X | la zone de Lesser Faydark
	}
	| In Lesser Faydark
	/if (${HalloweenZones[3] == 0}) {
		/call MoveTo -Loc|Y,X | on sort de Mistmoore
		/call MoveTo -Loc|Y,X | on s'approche du druid ring
		/varset LesserFaydarkFirenzellaID ${Spawn[Firenzella].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFirenzellaID}
		/target ID ${LesserFaydarkFirenzellaID}
		/casting "Scarecrow Costume Kit" item -maxtries|3
		/hail
		/varset LesserFaydarkShadyID ${Spawn[shady farmer].ID}
		
		/call MoveTo -SpawnID|${LesserFaydarkShadyID}
		/target ID ${LesserFaydarkShadyID}
		/hail
		/autoinventory
		
	}
/RETURN

|----------------------------------------------------------------------------|
|- Triangulates distance between to points using the Pythagoras theorem.	-|
|----------------------------------------------------------------------------|
|- @param loc#1: Location 'A'												-|
|- @param loc#2: Location 'B'												-|
|----------------------------------------------------------------------------|
SUB Triangulate_Distance(string loc#1, string loc#2)
/RETURN ${Math.Sqrt[${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}*${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}+${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}*${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}]}

|----------------------------------------------------------------------------------------------------|
|- MoveTo																							-|
|----------------------------------------------------------------------------------------------------|
|- Attempts to utilize MQ2MoveUtils /moveto function, to move to a given destination.				-|
|----------------------------------------------------------------------------------------------------|
|- @param1 destination: Destinations can be provided as an ID, or loc, using the following syntax:	-|
|-																									-|
|- Move to a Spawn ID use, -SpawnID|#																-|
|-		Ex.  /call MoveTo -SpawnID|${Target.ID}														-|
|-																									-|
|- Move to a Location use, -Loc|Y,X																	-|
|-		Ex.  /call MoveTo -Loc|${Target.Y},${Target.X}												-|
|----------------------------------------------------------------------------------------------------|
SUB MoveTo(destinationInfo, int distCheck)
	/if (${Debug}) /echo |- MoveTo ==>

	/declare X_Loc float local
	/declare Y_Loc float local
	/if (!${Defined[distCheck]}) /declare distCheck int local 10

	|-Set MoveTo variables per destinationType.
	/if (${destinationInfo.Find[-SpawnID]}) {
		/call TrueTarget ${destinationInfo.Arg[2,|]}
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
			/if (${Debug}) /echo I am already at the destination.
		} else {
			| Declare a timer based on how far you have to move (1s for every 50 units + 3).
			/declare retryTimer timer local ${Math.Calc[${Macro.Return} / 50 + 3].Int}s
			
			| Engage MQ2MoveUtils.
			:retry_MoveToLoc
			/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /interrupt
			/squelch /moveto loc ${Y_Loc} ${X_Loc} dist ${If[${Defined[distCheck]},${distCheck},10]}
			/delay 10 ${Me.Moving} || ${MoveTo.Moving}
			
			:movingTo_Loop
			
			| Debug Echo
			/if (${Debug} && !${spamTimer_MTLoop}) {
				/echo |- MoveToSpawn -| :MoveTo_Loop
				/call createTimer spamTimer_MTLoop 50
			}

			|/call Background_Events
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
			
			/if (${MoveTo.Moving}) {
				/if (${Macro.Return} > ${distCheck} && ${retryTimer}) {
					/goto :movingTo_Loop
				} else {
					/squelch /stick off
					/if (${Debug} && !${spamTimer_MTFAIL}) {
						/echo I have failed to move to the intended target.
						/call createTimer spamTimer_MTFAIL 30
					}
				}
			} else {
				/if (${Macro.Return} > ${Math.Calc[${distCheck} + ${Math.Calc[${distCheck} / 4].Int}].Int}) {
					/if (${retryTimer}) {
						/goto :retry_MoveToLoc
					} else {
						/if (${Debug} && !${spamTimer_MTFAIL}) {
							/echo I have failed to move to the intended target.
							/call createTimer spamTimer_MTFAIL 30
						}
					}
				} else {
					/squelch /stick off
					/if (${Debug}) /echo I have arrived at my intended destination.
					/delay 30 !${Me.Moving}
				}
			}
		}
	}

	/if (${Debug}) /echo <== MoveTo -|
/RETURN

|------------------------------------------------------------|
|- Reliably targets specified Target.IDs.					-|
|------------------------------------------------------------|
|- @param TargetID: The ID of the spawn you wish to target.	-|
|------------------------------------------------------------|
SUB TrueTarget(TargetID)
	/if (${Debug}) /echo |- TrueTarget${If[${TargetID}, [${TargetID}],]} ==>

	| Check if the target is in zone.
	/if (${SpawnCount[id ${TargetID}]}) {
		| Check server for range restrictions.
		/if (${MacroQuest.Server.Equal[PEQTGC]} && ${SpawnCount[id ${TargetID} radius 200]} || ${MacroQuest.Server.NotEqual[PEQTGC]}) {
		
			/declare retryTimer timer local 30
			
			:Target_Loop
			/squelch /target id ${TargetID}
			/delay 2 ${Target.ID} == ${TargetID}
			
			| Check that I have the correct target ID.
			/if (${Target.ID} != ${TargetID} && ${SpawnCount[id ${TargetID}]}) {
				/if (${retryTimer}) {
					/goto :Target_Loop
				} else {
					/echo Failed to target [${Spawn[id ${TargetID}].CleanName} - ${TargetID}]
				}
			}
		}
	}

	/if (${Debug}) /echo <== TrueTarget -|
/RETURN

