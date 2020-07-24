//++++++++++++++++++++++++++++++++
//+ DO NOT EDIT CODE BELOW UNLESS YOU KNOW WHAT YOU ARE DOING!
//+ YOU CAN BREAK THE SERVER MANAGER!
//++++++++++++++++++++++++++++++++

echo("--------- Executing Custom Code ---------");

//Stuff to open listening server
$asm::com::serverPort = -1;
$asm::com::serverIP = "127.0.0.1";
$asm::com::serverNumber = 0;
$asm::com::server = 0;

//Only initialize after server is started!
function initASMCom()
{
	new TCPObject(asmCom);
	$asm::com::serverPort = $Server::Port + 1000;
	echo("ASM PORT:" SPC $asm::com::serverPort);
	asmCom.listen($asm::com::serverPort);
	sendClientList();
}

function asmCom::onConnectRequest(%this, %address, %socket)
{
	%ip = getSubStr(%address, 0, strPos(%address, ":"));
	if(%ip !$= $asm::com::serverIP)
		return;
	%server = new TCPObject(asmComInput, %socket)
	{
		address = %address;
	};
	if($asm::com::server != 0)
		$asm::com::server.delete();
	$asm::com::server = %server;
}

function asmComInput::onLine(%this,%line)
{
	echo(%line);
	eval(%line);
}

function sendClientList()
{
	cancel($sendClientList);
	
	if(!$asm::com::server)
	{
		$sendClientList = schedule(5000,0,sendClientList);
		return;
	}
	
	%count = ClientGroup.getCount();
		
	for(%i=0;%i<%count;%i++)
	{
		%client = ClientGroup.getObject(%i);
		if(%client.isSuperAdmin)
			%output = "S";
		else if(%client.isAdmin)
			%output = "A";
		else
			%output = " ";
		%output = %output @ "\t" @ %client.getPlayerName() @ "\t" @ %client.getBLID() @ "\t" @ %client.getPing();
		%output = %output @ "\t" @ %client.clanPrefix @ "\t" @ %client.clanSuffix @ "\t" @ %client.score @ "\t" @ %client.getRawIP();
		if(%i != %count-1)
			%output = %output @ "|";
	}
	
	$asm::com::server.send("PLAYERS" SPC %output @ "\n");
	$sendClientList = schedule(5000,0,sendClientList);
}

package BlocklandASM
{
	function serverCmdMessageSent(%client, %message)
	{
		parent::serverCmdMessageSent(%client, %message);
		%output = %client.getSimpleName() @ ":" SPC %message;
		$asm::com::server.send("MESSAGE" SPC %output @ "\n");
	}
};
schedule(10, 0, activatePackage, BlocklandASM);

function fastPacketFixLoop(%bool)
{
   cancel($FastPacketFixSchedule);
   $Pref::Net::FastPackets = %bool;

   if(%bool)
      $FastPacketFixSchedule = schedule(8000, 0, fastPacketFixLoop, !%bool);
   else
      $FastPacketFixSchedule = schedule(600, 0, fastPacketFixLoop, !%bool);
}

fastPacketFixLoop(true);

schedule(10,0,initASMCom);

echo("--------- End Executing Custom Code ---------");

//++++++++++++++++++++++++++++++++
//+ END OF CODE THAT CAN BREAK THINGS
//++++++++++++++++++++++++++++++++
