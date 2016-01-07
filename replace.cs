//+ main.cs
//+
//+ Remade with added functions to facilitate server manager
//+
//++++++++++++++++++++++++++++++++
//+ DO NOT EDIT CODE BELOW UNLESS YOU KNOW WHAT YOU ARE DOING!
//+ YOU CAN BREAK BLOCKLAND AND THE SERVER MANAGER!
//++++++++++++++++++++++++++++++++

function initCommon()
{
	setRandomSeed();
	exec("./client/canvas.cs");
	exec("./client/audio.cs");
}

function initBaseClient()
{
	exec("./client/message.cs");
	exec("./client/mission.cs");
	exec("./client/missionDownload.cs");
	exec("./client/actionMap.cs");
}

function initBaseServer()
{
	exec("./server/mainServer.cs");
}

function onDatablockLimitExceeded()
{
	$datablockExceededCount += 1;
}

function onDatablocksDeleted()
{
	$datablockExceededCount = 0;
}

echo("--------- Executing Custom Code ---------");

//Stuff to open listening server
$asm::com::serverPort = 5123;
$asm::com::serverIP = "127.0.0.1";
$asm::com::serverNumber = 0;

function initASMCom()
{
	new TCPObject(asmCom);
	asmCom.listen($asm::com::serverPort);
}

function asmCom::onConnectRequest(%this,%address,%socket)
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
	%output = "";
	for(%i=0;%i<%count;%i++)
	{
		%client = ClientGroup.getObject(%i);
		%output = %output @ %client.getPlayerName() @ "\t" @ %client.getBLID() @ "\t" @ %client.getPing();
		if(%i != %count-1)
			%output = %output @ "|";
	}
	$asm::com::server[$asm::com::serverNumber].send(%output @ "\n");
	$sendClientList = schedule(5000,0,sendClientList);
}

initASMCom();

echo("--------- End Executing Custom Code ---------");

//++++++++++++++++++++++++++++++++
//+ END OF CODE THAT CAN BREAK THINGS
//++++++++++++++++++++++++++++++++