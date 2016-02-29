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