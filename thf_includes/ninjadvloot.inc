| Ninjadvloot.inc v3.1 05/11/2013 Redguides.com 
| Author A_Druid_00 12/19/2005
| Based on original looting code of toomanynames
| The following updates were all individually posted and consolidated by Maskoi into the current version.
| Updated: Moeymoejoe added strip out comma in item names when accessing ini file 08/02/2006
| Updated: mystikule added sell option to ini file 01/07/2008
| Updated: drkrain for House of Thule 10/23/2010
| Updated: Maskoi with Sell Routines 11/27/2010
| Updated: Maskoi with /item keep/sell/destroy ini commands 07/21/2011
| Updated: Maskoi with Foraged item handling 08/31/2011
| Updated: Maskoi with Buying item handling 07/31/2012
| Updated: Maskoi with Assign different ini file for item handling 01/01/2013
|
| Selling Contributions by
|   drkrain - TLInv item check
|   dewey2461 - Add sold items to ini file Event
|
| Requirements: MQ2Moveutils
| Usage: add the following to your macro for looting
|   #include Ninjadvloot.inc
|   /call setupAdvLootVars
|   /call LootMobs
|
| Assign differnent ini files. Default is Loot.ini
|   /call setupAdvLootVars "MyToonLoot.ini"
|   /call setupAdvLootVars "LootMesa.ini"
|
| Commands
| Usage. Grab the item and put it on your cursor then use one of the following commands
| 	/lootdestroy - Mark item on cursor to Destroy in Loot.ini file
| 	/lootignore - Mark item on cursor to Ignore in Loot.ini file
| 	/lootkeep - Mark item on cursor to Keep in Loot.ini file
| 	/lootsell - Mark item on cursor to Sell in Loot.ini file
| 	/sellstuff - Target a vendor and this command will sell any item in your inventory marked sell. 
| 	/depositstuff - Target your guildbanker and this command will deposit any item in your inventory marked keep into the guild bank.

#EVENT CantLoot         "#*#may not loot this corpse#*#"
#EVENT Sell             "#*#You receive#*# for the #1#(s)#*#"
#EVENT EditIniItem      "[MQ2] LootIniItem #1# #2#"
#EVENT SellStuff        "[MQ2] NinjadvLoot selling items to vendor"
#EVENT Broke            "#*#you cannot afford#*#" 
#EVENT Broke            "#*#you can't afford#*#" 
#EVENT Forage           "Your forage mastery has enabled you to find something else!"
#EVENT Forage           "You have scrounged up #*#"
#EVENT InventoryFull    "#*#Your inventory appears full!#*#"

SUB setupAdvLootVars(string ChangeIniFile)

    /declare CantLootID                         int                 outer 
    /declare CorpseRotTimer                     timer               outer
	/declare AletClearTimer                     timer               outer
    /declare NinjadVersion                      string              outer   3.0
    /declare InventoryFull                      int                 outer   0
    /declare NinjaIniFile                       string              outer   Loot.ini
    /if (${ChangeIniFile.Length}) {
        /varset NinjaIniFile ${ChangeIniFile}
        /echo Ninja Advance Loot in now writing all info to the ${ChangeIniFile} file.
    }
    /declare NALVersion                         string              outer   ${Ini[${NinjaIniFile},Settings,Version]}
    /if (!${Ini[${NinjaIniFile},Settings,Version].Length}) {
        /call CreateIni
        /ini "${NinjaIniFile}" "Settings" "Version" "0.0"
    }
    /if (${NALVersion.NotEqual[${NinjadVersion}]}) {
        /echo NinjAdvLoot version mismatch detected writing aliases.
        /noparse /squelch /alias /lootignore    /echo LootIniItem Ignore ${Cursor.Name}
        /noparse /squelch /alias /lootsell      /echo LootIniItem Sell ${Cursor.Name}
        /noparse /squelch /alias /lootkeep      /echo LootIniItem Keep ${Cursor.Name}
        /noparse /squelch /alias /lootdestroy   /echo LootIniItem Destroy ${Cursor.Name}
        /squelch /alias /sellstuff              /echo NinjadvLoot selling items to vendor
        /squelch /alias /depositstuff           /echo NinjadvLoot depositing items to guildbank
        /ini "${NinjaIniFile}" "Settings" "Version" "${NinjadVersion}"
    }

    | Loot ini settings
    /call ALLoadVar Settings OpenAllBags        1                   int
    /call ALLoadVar Settings AddNewSales        1                   int
    /call ALLoadVar Settings LootForage         1                   int
    /call ALLoadVar Settings LootMobs           TRUE                bool
    /call ALLoadVar Settings CorpseRadius       100                 int
    /call ALLoadVar Settings MobsTooClose       100                 int
    /call ALLoadVar Settings CorpseRotTime      440s                string
    /call ALLoadVar Settings ReportLoot         FALSE               bool
    /call ALLoadVar Settings LootForageSpam     1                   int
    | Alert list 9 used to define an ignore list like campfires untargetable etc.
    /if (!${Defined[${AlertList}]}) /declare AlertList int outer 9

/RETURN

SUB ALLoadVar(IniSection,IniVar,IniValue,VarType)
    /if (!${Defined[${IniVar}]} && ${Defined[VarType]}) /declare ${IniVar} ${VarType} outer
    /declare IniString string local ${Ini[${NinjaIniFile},${IniSection},${IniVar},NOTFOUND]}
    /varset ${IniVar} ${IniString}
    /if (${IniString.Equal["NOTFOUND"]}) {
        /if (${IniString.Equal["NOTFOUND"]}) /varset ${IniVar} ${IniValue}
        /ini "${NinjaIniFile}" "${IniSection}" "${IniVar}" "${${IniVar}}"
    } 
/RETURN

| ****************  Loot Mobs Section ******************** |

SUB EVENT_CantLoot
   /varset CantLootID ${Target.ID}
/RETURN

SUB LootMobs
	/if (!${AletClearTimer}) {
		/call AlertClear
		/varset AletClearTimer 5m
		/squelch /alert add ${AlertList} untargetable
		/squelch /alert add ${AlertList} FSP
	}
	/declare DeadCount int local ${SpawnCount[corpse radius ${CorpseRadius} zradius 50 noalert 25]}
    /if (!${LootMobs}) {
		/chat ${channel} Can't loot mobs !
		/RETURN
	}
	/if (${SpawnCount[npc radius ${MobsTooClose} zradius 50 noalert ${AlertList}]}) {
		/chat ${channel} Mobs near me, can't loot !
		/RETURN
	}
	/if (!${DeadCount}) {
		/chat ${channel} No mobs' corpses to loot !
		/RETURN
	}
	/if (${Me.Combat}) {
		/chat ${channel} In combat, can't loot !
		/RETURN
	}
	/if (!${Me.FreeInventory}) {
		/chat ${channel} Inventory FULL !
		/RETURN
	}
    /if (!${CorpseRotTimer}) /squelch /alert clear 25
    /declare i int local
    /declare CorpseList string local 
    /declare CorpseToLoot int local
    /declare ZDist float local ${Math.Distance[${Target.Z}:${Me.Z}]}
	/chat ${channel} "Number of corpse = ${DeadCount}"
    /for i 1 to ${DeadCount}
        /varset CorpseList ${CorpseList}${NearestSpawn[${i},corpse radius ${CorpseRadius} zradius 50 noalert 25].ID}|
    /next i
    /for i 1 to ${DeadCount}
    /varset CorpseToLoot ${Spawn[${CorpseList.Arg[${i},|]}].ID} 
        /if (${Spawn[${CorpseToLoot}].Deity.ID} || !${Spawn[${CorpseToLoot}].ID} || !${Spawn[${CorpseToLoot}].LineOfSight}) /goto :SkipCorpse
            /if (${Target.ID}!=${CorpseToLoot}) /target id ${CorpseToLoot}
            /if (!${Me.Standing}) /stand
            /delay 2s ${Target.ID}==${CorpseToLoot} && ${Me.Standing}
            /if (${Target.Distance}>5) {
                /moveto mdist 10
                /moveto loc ${Target.Y} ${Target.X} 
                /delay 250 ${MoveTo.Stopped}
                /call ZCheck ${ZDist} 8
                /face fast
            }
            /if (${Target.Distance}<10) /call LootCorpse
        :SkipCorpse
    /next i
	/chat ${channel} "Looting done !"
/RETURN

SUB LootCorpse
    /declare i              int    local
    /declare j          	int    local
	/declare Cible			string local
	/declare LootList       string local
    /declare FirstLetter    string local
    /call CheckCursor
    /loot
    /delay 3s ${Corpse.Open}
    /doevents CantLoot
    /if (${Target.ID}==${CantLootID} && ${Spawn[${CantLootID}].ID}) /squelch /alert add 25 id ${CantLootID}
    /delay 3s ${Corpse.Items} || ${Target.ID}==${CantLootID}
    /if (${Corpse.Open} && ${Corpse.Items}) {
        /declare loottotal int  local
        :LootLag
        /varset loottotal ${Corpse.Items}
        /delay 1s ${loottotal}!=${Corpse.Items}
        /if (${loottotal}!=${Corpse.Items}) /goto :LootLag
        /for i 1 to ${loottotal}
            /varset FirstLetter ${Corpse.Item[${i}].Name.Left[1]}
            /if (${Corpse.Item[${i}].ID} && !${Select[${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"]},Ignore,Keep,Destroy,Sell]}) {
                /if (${Corpse.Item[${i}].NoDrop})  /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" Ignore
                /if (!${Corpse.Item[${i}].NoDrop}) /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" Keep
            }    
            /if (${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].NotEqual[Destroy]} && !${Me.FreeInventory} && (!${FindItemCount[=${Corpse.Item[${i}].Name}]} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && !${Corpse.Item[${i}].FreeStack})) || (${Corpse.Item[${i}].Lore} && ${FindItem[${Corpse.Item[${i}]}].ID}) || ${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Ignore]}) /varset LootList ${LootList}${Corpse.Item[${i}]},
            /if ((${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Keep]} || ${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Sell]}) && (!${Corpse.Item[${i}].Lore} || !${FindItem[${Corpse.Item[${i}]}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack}))) /call LootItem ${i} Keep Right
            /if (${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"].Equal[Destroy]}) /call LootItem ${i} Destroy left
        /next i
        /if (${Corpse.Items}) {
            /if (${ReportLoot} && ${LootList.Length}) {
                /keypress Enter 
                /shiftkey /keypress . chat
				/nomodkey /keypress c chat
				/nomodkey /keypress h chat
				/nomodkey /keypress q chat
				/nomodkey /keypress t chat
				/nomodkey /keypress space chat
                /shiftkey /keypress ${channel} chat
                /keypress Space chat
                /notify LootWND LW_BroadcastButton leftmouseup 
                /delay 1s
                /call Type " left on corpse "
				/if (${Target.ID}) {
					/shiftkey /keypress i chat
					/shiftkey /keypress d chat
					/keypress = chat
					/varset Cible ${Target.ID}
					/for j 1 to ${Cible.Length}
						/shiftkey /keypress ${Cible.Mid[${j}, 1]} chat
					/next j
				}
                /keypress Enter chat
            }
            /if (${Target.ID}) /squelch /alert add 25 id ${Target.ID}
            /varset CorpseRotTimer ${CorpseRotTime}
        }
    }
    :clickdoneffs
        /nomodkey /notify LootWnd LW_DoneButton leftmouseup
        /delay 1s !${Window[LootWnd].Open}
    /if (${Window[LootWnd].Open}) /goto :clickdoneffs
/RETURN

SUB LootItem(int i,DoWhat,WhichButton)
    /declare CorpseItemID int local ${Corpse.Item[${i}].ID}
    /nomodkey /shift /itemnotify  ${Corpse.Item[${i}].InvSlot} ${WhichButton}mouseup
    /delay 5s ${Window[ConfirmationDialogBox].Open} || !${Corpse.Item[${i}].NoDrop}
    /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
    /delay 5s ${Cursor.ID} || ${WhichButton.NotEqual[left]}
    /if (${DoWhat.Equal[Destroy]} && ${Cursor.ID}==${CorpseItemID}) /destroy
    /delay 3s !${Corpse.Item[${i}].ID}
    /if (${Cursor.ID}) /call CheckCursor
/RETURN

SUB CheckCursor
    :ALauto_inv
	/if (${Cursor.ID}) {
		/if (${Cursor.NoDrop} && !${Me.FreeInventory}) /RETURN
		/timed 5 /autoinventory
		/delay 2s !${Cursor.ID}
		/goto :ALauto_inv
    }
/RETURN

| **************** Foraged Items Section ******************** |
SUB EVENT_Forage
    /if (!${LootForage}) /RETURN
    /declare ForagedItem string local
    :MoreToAutoInventory
        /if (${Cursor.ID}) {
            /varset ForagedItem ${Cursor.Name}
            /if (!${Select[${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"]},Ignore,Keep,Destroy,Sell]}) {
                /if (${Cursor.ID}) /ini "${NinjaIniFile}" "${Cursor.Name.Left[1]}" "${Cursor.Name}" Keep
                /if (${LootForageSpam}) /echo New foraged item >> ${Cursor.Name} << added to ini file. 
            }
            /if (${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Destroy]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Ignore]}) {
                /if (${Cursor.Name.Equal[${ForagedItem}]})     /destroy
                /delay 5
                /if (${LootForageSpam}) /echo NINJADVLOOT --  Destroyed foraged ${ForagedItem}.
            } else /if ((${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Keep]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Sell]}) && (!${Cursor.Lore} || !${FindItem[${Cursor.Name}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Cursor.Name}]} && ${Cursor.Stackable} && ${Cursor.FreeStack}))) {
                /if (${LootForageSpam}) /echo NINJADVLOOT ++ Kept foraged ${ForagedItem}.
                /autoinventory
                /delay 5
            }
        }
    /if (${Cursor.ID}) /goto :MoreToAutoInventory
/RETURN

| **************** Search Loot Section ******************** |
SUB DoLootStuff(LootAction)
    /declare i int local
    /declare j int local
    /declare k int local
    /declare LootItem string
    /if (${OpenAllBags}) {
        /echo Opening all bags
        /keypress OPEN_INV_BAGS
    }
   /call NPC "${Target.CleanName}"
   | Top level inv has its own loop since ${FindItem[=${ItemToSell}].InvSlot} looks at top lev inv slots first in SUB SellToVendor for the item to sell.
   | Loop through top level inventory slots for sellable items
   /for k 1 to 10
        | Check if top level inv slot is not a container/bag
        /if (${InvSlot[pack${k}].Item.Container}==0) {
            | If top level inv slot is empty check for an item marked sell
            /if (${InvSlot[pack${k}].Item.ID}) {
                /varset LootItem ${InvSlot[pack${k}].Item}
                /if (${LootAction.Equal[sell]})  {
                /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
                }
                /if (${LootAction.Equal[deposit]})  {
                    /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
                }
            }
        }
    /next k      
    | Loop through top level inventory slots for containers
    /for i 1 to 10
        | Check if top level inv slot has a bag
        /if (${InvSlot[pack${i}].Item.Container}==0) /next i
        | Set j 1 to number of bag slots and loop through slots looking for items marked sell
        /for j 1 to ${InvSlot[pack${i}].Item.Container}
            /if (${InvSlot[pack${i}].Item.Item[${j}].ID}) {
            /varset LootItem ${InvSlot[pack${i}].Item.Item[${j}].Name}
            /if (${LootAction.Equal[sell]})  {
                /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Sell]}) /call SellToVendor "${LootItem}"
                }
                /if (${LootAction.Equal[deposit]})  {
                    /if (${Ini[${NinjaIniFile},"${LootItem.Left[1]}","${LootItem}"].Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
                }
            }
        /next j
    /next i

    /if (${OpenAllBags}) {
        /echo Closing all bags
        /keypress CLOSE_INV_BAGS
    }
    | Prevent spam from Sell event after selling items in SUB SellStuff
    /doevents flush Sell
    /if (${Window[MerchantWnd].Open}) {
        /echo Closing merchant window
        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
    }
    /if (${Window[GuildBankWnd].Open}) {
        /echo Closing guildbank window
        /keypress esc
    }
/RETURN
   
SUB NPC(NPCName)
    /target npc ${NPCName}
    /delay 1s
    /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.CleanName.Find[familiar]}) {
        /echo Please target a vendor or guildbanker
        /RETURN
    }
    /echo Doing business with ${NPCName}
    /target id ${Spawn[${NPCName}].ID}
    /delay 3s ${Target.ID}==${Spawn[${NPCName}].ID}
    /if (${Target.Distance}>15) {
        /moveto id  ${Spawn[${NPCName}].ID}
        /delay 250 ${MoveTo.Stopped}
    }
    /face nolook
    /delay 1s
    /echo Opening merchant/guildbank window
    /nomodkey /click right target
    /echo Waiting 5s for merchant/guildbank window to populate
    /delay 5s
/RETURN

| **************** Sell Loot Section ******************** |
SUB EVENT_Sell(string soldstr, string ItemName)
    /if ( ${AddNewSales} ) {
        /echo Setting ${ItemName} to sell
        /ini "${NinjaIniFile}" "${ItemName.Left[1]}" "${ItemName}" Sell
    }
/RETURN

SUB SellToVendor(ItemToSell)
    /if (${Window[MerchantWnd].Open}) {
    :sell
        /echo Selling ${ItemToSell}
        /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup
        /delay 5
        /nomodkey /shiftkey /notify merchantwnd MW_Sell_Button leftmouseup
        /delay 2s
    /if (${FindItem[${itemToSell}].InvSlot}) /goto :sell
    }
/RETURN

| **************** Deposit Loot Section ******************** 
SUB DepositToGuildBank(ItemToDeposit)
    /if (${Window[GuildBankWnd].Open}) {
        :deposit
            /if (${Window[GuildBankWnd].Child[GBANK_DepositCountLabel].Text.Arg[2,:]}==0) {
                /echo Guildbank Deposited Items is full.
                /RETURN
            }
            /if (${FindItem[=${ItemToDeposit}].InvSlot}<=22) /RETURN
            /echo Depositing ${ItemToDeposit}
            /nomodkey /shiftkey /itemnotify ${FindItem[=${ItemToDeposit}].InvSlot} leftmouseup
            /delay 5
            /nomodkey /notify GuildBankWnd GBANK_DepositButton leftmouseup
            /delay 2s
        /if (${FindItem[${itemToSell}].InvSlot}) /goto :deposit
    }
/RETURN

| **************** Item set ini file event ******************** 
    SUB EVENT_EditIniItem(ItemString,ItemAction,ItemName)
        /if (${Cursor.ID}) {
            /echo -- Setting ${ItemName} to ${ItemAction}
            /ini "${NinjaIniFile}" ${ItemName.Left[1]} "${ItemName}" ${ItemAction}
        } else {
            /echo No item on cursor.
        }
    /RETURN

| **************** Sell Stuff Event ******************** 
    SUB EVENT_SellStuff
        /call DoLootStuff sell
    /RETURN
    
| **************** Deposit Stuff Event *******************
    SUB EVENT_DepositStuff
        /call DoLootStuff deposit
    /RETURN
    
| **************** Broke Event ******************** 
SUB EVENT_Broke 
    /echo You are out of money! 
    /endMacro 
/RETURN

| **************** Type Stuff  ******************** 
| Lifted from commonsubs.inc by Armysoldier    
SUB Type(String InStr)
    /declare char string local 
    /declare loopctr int local 
    /for loopctr 1 to ${InStr.Length} 
        /varset char ${InStr.Mid[${loopctr},1]} 
        /if (!${char.Length}) { 
            /nomodkey /keypress space chat 
        } else { 
            /nomodkey /keypress ${char} chat 
        } 
    /next loopctr 
/RETURN 

| **************** Buy Stuff ********************  
SUB Buy(string ItemToBuy, int amount)
    /declare i int local
    | /echo Buying ${ItemToBuy}!
    /declare QTY int local
    /declare ListItem int local
    /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
    /delay 1s
    /if (${FindItemCount[${ItemToBuy}]}>= ${amount}) {
        /echo I have enough ${ItemToBuy}.
        /RETURN
    }   
    /varset ListItem ${Window[MerchantWnd].Child[ItemList].List[=${ItemToBuy},2]}
    /if (!${ListItem}) {
        /echo couldn't find ${ItemToBuy}
        /RETURN
    } else {
        /notify MerchantWnd ItemList listselect ${ListItem}
        /delay 1s       
    } 
    /echo Buying ${ItemToBuy} Till I get ${amount} 
    :Loop
    /doevents
    /if (${InventoryFull}) /RETURN
    /if (${QTY}>1) {
        /if (${QTY}>19) {
            /Shiftkey /notify merchantwnd MW_Buy_Button leftmouseup
            /delay 2s ${FindItemCount[${ItemToBuy}]}>=${QTY}
            /echo ${FindItemCount[${ItemToBuy}]} ${ItemToBuy} in inventory
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]} 
            /delay 3
            /if (${QTY}<=0) /RETURN
            /goto :Loop
        }
        /if (${QTY}>0 && ${QTY}<20) {
            /for i 1 to ${QTY} 
                /ctrlkey /notify merchantwnd MW_Buy_Button leftmouseup
                /delay 2s ${FindItemCount[${ItemToBuy}]}>=${Math.Calc[${FindItemCount[${ItemToBuy}]}+${i}]}
                /echo Buying ${ItemToBuy} ${i} of ${QTY}
                /if (${i}>=${QTY}) {
                    /echo ${FindItemCount[${ItemToBuy}]} ${ItemToBuy} in inventory
                    /RETURN
                }
            /next i
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
            
        }
    }
/RETURN
    
SUB ZCheck(float Zcur, float Zdiff)
    | Move down if Z distance more than Zcheck due to levitation
    /if (${Zcur}>=${Zdiff}) {
        /keypress end hold
        /delay 10
        /keypress end
    }
/RETURN

SUB EVENT_InventoryFull
    /echo Your Inventory is full. Turning off looting functions.
    /varset LootMobs FALSE
    /varset InventoryFull 1
/RETURN

SUB CreateIni
    /if (${Ini[${NinjaIniFile}].Length}) /RETURN
    /declare a local Settings|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z
    /echo Creating ini file ${NinjaIniFile}
    /declare i int local 1
    /for i 1 to 27
        /ini ${NinjaIniFile} "${a.Arg[${i},|]}" "|===================================" "==================================|"
    /next i
/RETURN

SUB AlertClear
	/squelch /alert clear 25
	/squelch /alert clear ${AlertList}
 /RETURN