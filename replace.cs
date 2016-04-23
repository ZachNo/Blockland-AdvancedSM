//++++++++++++++++++++++++++++++++
//+ DO NOT EDIT CODE BELOW UNLESS YOU KNOW WHAT YOU ARE DOING!
//+ YOU CAN BREAK THE SERVER MANAGER!
//++++++++++++++++++++++++++++++++

echo("--------- Executing Custom Code ---------");

//Stuff to open listening server
$asm::com::serverPort = 5123;
$asm::com::serverIP = "127.0.0.1";
$asm::com::serverNumber = 0;

function initASMCom()
{
	new TCPObject(asmCom);
	asmCom.listen($asm::com::serverPort);
	sendClientList();
}

function asmCom::onConnectRequest(%this, %address, %socket)
{
	%server = new TCPObject(asmComInput,%socket)
	{
		address = %address;
	};
	$asm::com::serverNumber++;
	$asm::com::server[$asm::com::serverNumber] = %server;
}

function asmComInput::onLine(%this,%line)
{
	eval(%line);
}

function sendClientList()
{
	cancel($sendClientList);
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
	$asm::com::server[$asm::com::serverNumber].send("PLAYERS" SPC %output @ "\n");
	$sendClientList = schedule(5000,0,sendClientList);
}

initASMCom();

echo("--------- End Executing Custom Code ---------");

//++++++++++++++++++++++++++++++++
//+ END OF CODE THAT CAN BREAK THINGS
//++++++++++++++++++++++++++++++++