/* 
 * site related parts
 */
var xmlInfo;
var waitingStr;
var msg404;

var lastTimeOut = 0;

/*
 * Menu related parts
 */
var xmlMenu;
var lastSelectedMenu;
var lastSelectedSubMenu;

function create_xml()
{
	var xmlFile = null;
	// trial and error... browser hell
	if (window.ActiveXObject)
		xmlFile = new ActiveXObject("Microsoft.XMLDOM");
	else	if (document.implementation && document.implementation.createDocument)
			xmlFile = document.implementation.createDocument("","doc",null);
		else
			xmlFile = null;
			
	return xmlFile;
}

function window_load( MenuXMLFileName, InfoXMLFileName )
{
	try
	{
		// draw main content
		xmlInfo = create_xml();
		xmlInfo.async = false;
		xmlInfo.validateOnParse = false;
		xmlInfo.load( InfoXMLFileName );
		drawMainSite( xmlInfo );
		
		// draw the menus
		xmlMenu = create_xml();
		xmlMenu.async = false;
		xmlMenu.validateOnParse = false;
		xmlMenu.load( MenuXMLFileName );
		drawMenus( xmlMenu.getElementsByTagName("menu") );
	}
	catch (e)
	{
		// opera is still undeupported. 		
		alert( "Error in jsportal.js line: "  + e.lineNumber + " \n\n"
		  + e.code +" : "+ e.message );
	}
	
}

function drawMainSite( info )
{
	var siteName   = info.getElementsByTagName("name");
	var siteURL    = info.getElementsByTagName("url");
	var webMaster  = info.getElementsByTagName("webmaster");	
	var links1     = info.getElementsByTagName("links1")[0].getElementsByTagName("link");
	var links2     = info.getElementsByTagName("links2")[0].getElementsByTagName("link");
	waitingStr = info.getElementsByTagName("messages")[0].getElementsByTagName("loading")[0].firstChild.data;
	msg404     = info.getElementsByTagName("messages")[0].getElementsByTagName("msg404")[0].firstChild.data;
	
	/* site title */
	document.title = siteName[0].firstChild.data;
	document.getElementById("idSiteTitle").innerHTML = 	document.title;
		
	/* upper left+right links */
	try{drawLinks( links1, "idLinks1" );} catch(e){};
	try{drawLinks( links2, "idLinks2" );} catch(e){};

	/* footer */
	document.getElementById("idWebmasterEMAIL").innerHTML = 	
		"<a href='mailto:" + 
		webMaster[0].getElementsByTagName("email")[0].firstChild.data + "'>" +
		webMaster[0].getElementsByTagName("name")[0].firstChild.data +
		"</a>";

	try // he, it's ok if the webmaster does not have a homepage, right..?
	{
		var homePage = webMaster[0].getElementsByTagName("webpage")[0].firstChild.data;
		if (homePage!=null)
			document.getElementById("idHomePage").innerHTML = 
				"<a href='" + homePage + "'>" + homePage + "</a>" 
	}
	catch(e)
	{
	}
}

function drawLinks( links, where )
{	
	var tmpHTML = "";
	for (n=0; n<links.length; n++)
	{
		var u, thisLinkName, thisLinkURL;
		var newWindow = false;
		
		u = links[n].attributes;
		thisLinkName = u.getNamedItem("n").value;
		thisLinkURL  = u.getNamedItem("h").value;
		
		// "w" property is optional
		try{newWindow = u.getNamedItem("w").value == "1";}
		catch(e){};
			
		if (n>0) tmpHTML += " | ";
		
		tmpHTML+= "<a href='" + thisLinkURL +"'"
		if (newWindow)
			tmpHTML+= " target='_blank'";
		tmpHTML+= ">" + thisLinkName +"</a>";
	}	
	
	document.getElementById(where).innerHTML = tmpHTML;
}

function drawMenus( menus )
{
	var menuHTML, xmlDoc, menus;

	menuHTML = "";
	for (n=0; n<menus.length; n++)
	{
		var u, thisMenuName, thisMenuTitle, thisMenuLink;
		var hidden = false;
		u = menus[n].attributes;
		thisMenuName  = u.getNamedItem("n").value;
		thisMenuTitle = u.getNamedItem("t").value;

		// "hidden" property is optional
		try{hidden = u.getNamedItem("hidden").value == "1";}
		catch(e){};
			
		menuHTML+=  "<a class='clsMainMenuNormal'" +
			(hidden ? " style='display: none;'" : "" ) +
			" id='id" + thisMenuName + "' href='javascript:clickMenu( \"" + thisMenuName + "\" )'>" +
			"<span> " + thisMenuTitle + "</span></a>";
	}
	
	document.getElementById("idMainMenu").innerHTML = menuHTML;
	clickMenu( menus[0].attributes.getNamedItem("n").value );
}

function clickMenu( menuItem )
{
	var menuDiv = document.getElementById( "id" + menuItem );
	var subMenu = null;
	var menus;
	
	// select corresponding item 
	if (lastSelectedMenu != null)
	{
		lastSelectedMenu.className = "clsMainMenuNormal";
	}
	
	lastSelectedMenu = menuDiv;
	menuDiv.className = "clsMainMenuSelected";
	
	// show subMenus
	menus = xmlMenu.getElementsByTagName("menu");
	for (n=0; n<menus.length; n++)
	{
		if (menus[n].attributes.getNamedItem("n").value == menuItem)
			subMenu = menus[n];
	}
	
	// check...
	if (subMenu == null)
		return;

	menus = subMenu.getElementsByTagName("submenu");

	// draw sub menus
	menuHTML = "";
	for (n=0; n<menus.length; n++)
	{
		var menuText  = menus[n].attributes.getNamedItem("t").value;
		var menuURL   = menus[n].attributes.getNamedItem("u").value;
		
		if (menuURL!="")
			menuHTML += "<a class='clsSubMenuNormal' href='" + menuURL + 
				"' onclick='clickSubMenu(this)' " +
				"><span>" + menuText + "</span></a>";
		else
			menuHTML += "<p class='clsSubMenuTitle'>" + menuText + "</p>";
	}
	document.getElementById("idSubMenu").innerHTML = menuHTML;
	
	// just in case the connection is bad, display a message
	document.getElementById("idMainContent").innerHTML =
		"<div class='clsWaitingBanner'>"+ waitingStr +"</div>";
	
	// redirect main frame
	var u = document.getElementById("idHiddenContent");
	var v = document.getElementById("idMainContent");
	
	u.src = subMenu.attributes.getNamedItem("u").value;
	if (lastTimeOut!=0)
		clearTimeout(lastTimeOut);
	lastTimeOut = setTimeout("checkContent()",7000);
}

function checkContent()
{	
	text1 = document.getElementById("idMainContent").innerHTML;
	
	if (text1.indexOf(waitingStr)!=-1)
		document.getElementById("idMainContent").innerHTML = 
			"<center><div class='clsError'>"+ msg404 +"</div></center>";			
}

function clickSubMenu( subMenuItem )
{
	// select corresponding item
	//lastSelectedMenu = subMenuItem;
	//subMenuItem.className = "thisPage";
}
