

var menus    = new Array();
var isShow   = new Boolean(); 
isShow=false;
var isdown   = new Boolean();
isdown=false;
var srcel; 
var fromel;  
var toel; 
var usestatus=false;
var thestatus="";
var popTimer = 0;   //弹出时间
menuunitwidth=60;    //主菜单项宽度
menuitemwidth=150;   //子菜单宽度
menuheight=24;        //主菜单高度
menuwidth='100%';    //主菜单宽度
menuadjust=0;        //子菜单微调
menualign='center'; //主菜单对齐方式 
leftchar='(无)';    //主菜单左侧修饰
rightchar='(无)';   //主菜单右侧修饰
fonts='font-family: 宋体; font-size: 9pt; color: MenuText; '; //菜单字体风格
cursor='default';   //光标形状
var swipeSteps = 4; //菜单延迟次数
var swipemsec = 10; //菜单延迟时间
var swipeArray = new Array();
 function ccddcd(){
alert()
}
function swipe(el, dir, steps, msec) 
{
	if (steps == null) steps = swipeSteps;
 	if (msec == null) msec = swipemsec;
 	if (el.swipeIndex == null)  el.swipeIndex = swipeArray.length;
 	if (el.swipeTimer != null)  window.clearTimeout(el.swipeTimer);
	swipeArray[el.swipeIndex] = el;
 	el.style.clip = "rect(-99999, 99999, 99999, -99999)";
 	if (el.swipeCounter == null) 
	{
  		el.orgLeft  = el.offsetLeft;
  		el.orgTop  = el.offsetTop;
  		el.orgWidth = el.offsetWidth;
  		el.orgHeight  = el.offsetHeight;
	}
 	else if (el.swipeCounter == 0) 
	{
  		el.orgLeft  = el.offsetLeft;
		el.orgTop  = el.offsetTop;
  		el.orgWidth = el.offsetWidth;
  		el.orgHeight  = el.offsetHeight;
	}
 	el.style.left = el.orgLeft;
 	el.style.top  = el.orgTop;
 	el.swipeCounter = steps;
 	el.style.clip = "rect(0,0,0,0)";
 	window.setTimeout("repeat(" + dir + "," + el.swipeIndex + "," + steps + "," + msec + ")", msec);
}

function repeat(dir, index, steps, msec) 
{
 	el = swipeArray[index];
 	var left   = el.orgLeft;
 	var top    = el.orgTop;
 	var width  = el.orgWidth;
 	var height = el.orgHeight;
 	if (el.swipeCounter == 0) 
	{
  		el.style.clip = "rect(-99999, 99999, 99999, -99999)";
  		return;
 	}
 	else    
 	{
  		el.swipeCounter--;
  		el.style.visibility = "visible";
  		switch (dir) 
		{
	   		case 2: el.style.clip = "rect(" + height*el.swipeCounter/steps + "," + width + "," + height + "," + 0 + ")";
	    		el.style.top  = top - height*el.swipeCounter/steps;
	    		break;
	   		case 8: el.style.clip = "rect(" + 0 + "," + width + "," + height*(steps-el.swipeCounter)/steps + "," + 0 + ")";
	    		el.style.top  = top + height*el.swipeCounter/steps;
	    		break;
	   		case 6: el.style.clip = "rect(" + 0 + "," + width + "," + height + "," + width*(el.swipeCounter)/steps + ")";
	    		el.style.left  = left - width*el.swipeCounter/steps;
	    		break;
	   		case 4:el.style.clip = "rect(" + 0 + "," + width*(swipeSteps - el.swipeCounter)/steps + "," + height + "," + 0 + ")";
	    		el.style.left  = left + width*el.swipeCounter/steps;
	    		break;
		}
  		el.swipeTimer = window.setTimeout("repeat(" + dir + "," + index + "," + steps + "," + msec + ")", msec);
 	}
}
	
function hideSwipe(el) 
{
	window.clearTimeout(el.swipeTimer);
	el.style.visibility = "hidden";
	el.style.clip = "rect(-99999, 99999, 99999, -99999)";
	el.swipeCounter = 0;
}

function popOut() 
{
	popTimer = setTimeout('allhide()', 200);
}

function getReal(el, type, value) 
{
	 temp = el;
	 while ((temp != null) && (temp.tagName != "BODY")) 
 	 {
 		if (eval("temp." + type) == value) 
 		{
   			el = temp;
   			return el;
  		}
  		temp = temp.parentElement;
 	}
 	return el;
}

function MenuRegister(menu) 
{
  	menus[menus.length] = menu
  	return (menus.length - 1)
}

function MenuItem(caption,command,target,isline,statustxt)
{
 	this.caption=caption;
 	this.command=command;
 	this.target=target;
 	this.isline=isline;
 	this.statustxt=statustxt;
}

function Menu(caption,command,target)
{
 	this.items = new Array();
 	this.caption=caption;
 	this.command=command;
 	this.target=target;
 	this.id=MenuRegister(this);
}
function goto(gg)
{
sbpm_right.location.href=gg
}
function MenuAddItem(item)
{
  	this.items[this.items.length] = item
  	item.parent = this.id;
  	this.children=true;
}

Menu.prototype.addItem = MenuAddItem;

function toout(src)      //鼠标移出后的风格
{
	src.className="3dtable_up1";
}

function toin(src)                 //鼠标移入后的风格
{
	src.className="3dtable_down1";
}

function nochange(src)             //恢复原值
{
	src.className="3dtable1";
}

function allhide()                 //隐藏
{
 	for(var nummenu=0;nummenu<menus.length;nummenu++)
    	{
    		var themenu=document.all['menu'+nummenu]
    		var themenudiv=document.all['menudiv'+nummenu]
    		nochange(themenu);
    		menuhide(themenudiv);
    	}
}

function menuhide(menuid)           //菜单HIDE
{
	hideSwipe(menuid);
	isShow=false;
}

function menushow(menuid,pid)     //菜单SHOW
{
	menuid.style.left=posflag.offsetLeft*1+pid.offsetLeft*1+menuadjust*1;
	menuid.style.top=menutable.offsetTop+menutable.offsetHeight;
	swipe(menuid,2,4);
	isShow=true;
}

function menu_over(menuid,x)   //菜单选中
{
	toEl = getReal(window.event.toElement, "className", "3dtable1");
	fromEl = getReal(window.event.fromElement, "className", "3dtable1");
	if (toEl == fromEl) return;
	srcel = window.event.srcElement;
	if(menus[x].command=="")
	{
		if(!isShow){toout(srcel);}
		else
		{
  			allhide();
  			toin(srcel);
  			menushow(menuid,eval("menu"+x));
	     	}
	}
	else
	{
  		isShow = false;
  		allhide();
  		toout(srcel);
	}
	clearTimeout(popTimer);
}

function menu_out(menuid)   
{
	toEl = getReal(window.event.toElement, "className", "3dtable1");
	fromEl = getReal(window.event.fromElement, "className", "3dtable1");
	if (toEl == fromEl) return; 
	srcel = window.event.srcElement;
	if (isShow)
	{
		toin(srcel);
	}
	else	
	{
		nochange(srcel);
	}
	popOut()
}

function menu_down(menuid,x)
{
	srcel = getReal(window.event.srcElement, "className", "3dtable1");
	if(menus[x].command=="")
	{
		if(isShow)
		{
  			menuhide(menuid);
  			toout(srcel);
  		}
  		else
  		{
	  		toin(srcel);
			menushow(menuid,eval("menu"+x));
  			isdown=true;
  		}
	}
	else
	{
	  	toin(srcel);
  		isdown=true;
	}

}

function menu_up()   //菜单非按下
{
  isdown=false;
}

function menuitem_over(menuid)
{
	srcel = window.event.srcElement;
	if(isdown)
	{
 		srcel.className="highlight_container";
	}
	else
	{
 		srcel.className="highlight_container"; 		
	}
	if(thestatus!="")
	{
		usestatus=true;
	 	window.status=thestatus;
	}
	clearTimeout(popTimer);
}

function menuitem_out(menuid)
{
	srcel = getReal(window.event.srcElement, "className", "3dtable1");
	nochange(srcel);
	if(usestatus)window.status="";
	popOut()
}

function menuitem_down(menuid)
{
	isdown=true;
}

function menuitem_up(menuid)
{
	isdown=false;
}

function exec2(x)
{
	var cmd;
	if(menus[x].target=="blank")
	{
  		cmd = "window.open('"+menus[x].command+"')";
	}
	else
	{
	  	cmd = menus[x].target+".location='"+menus[x].command+"'";
	}
	eval(cmd);
}

function exec(x,i)
{
	var cmd;
	if(menus[x].items[i].target=="blank")
	{
  		cmd = "window.open('"+menus[x].items[i].command+"')";
	}
	else	
	{
 // 		cmd = menus[x].items[i].target+".location='"+menus[x].items[i].command+"'";
	cmd = menus[x].items[i].command
	}
	eval(cmd);
}

function body_click()
{
	if (isShow)
	{
 		srcel = getReal(window.event.srcElement, "className", "3dtable1");
	 	for(var x=0;x<=menus.length;x++)
 		{
  			if(srcel.id=="menu"+x)
  			return;
 		}
	 	allhide();
	}
}

document.onclick=body_click;
var MenuBodyRef;

function write()
{
	var wb=1;
	for(var i in document.all)
	{
		if (document.all[i].tagName == 'BODY')
		{
			MenuBodyRef = document.all[i]
                  	var stringx='<div id="posflag" style="position:absolute;"></div>\n<table id=menutable border=0 cellpadding=0 cellspacing=0 width='+menuwidth+' height='+menuheight+
                     	' onselectstart="event.returnValue=false"'+
                     	' style="cursor:'+cursor+';'+fonts+'; padding:0px" class=3dtable_up1><tr>'
                     	if(leftchar!="(无)")stringx+='<td width=16 align="center">'+leftchar+'</td>'      
                     	for(var x=0;x<menus.length;x++)
                     	{
                      		var thismenu=menus[x];
                        	stringx += "<td class=3dtable1 id=menu"+x+" "+
                      		" width="+menuunitwidth+"px onmouseover=menu_over(menudiv"+x+
                      		","+x+") onmouseout=menu_out(menudiv"+x+
                      		") onmousedown=menu_down(menudiv"+x+","+x+")";
                         	if(thismenu.command!="")
                         	{
                                	stringx += " onmouseup=exec2("+x+");menu_up();";
                        	}
                         	else
                         	{
                               		stringx += " onmouseup=menu_up()";
                            	}
                      		stringx += " align="+menualign+">"+thismenu.caption+"</td>"
                     	}
                     	if(rightchar!="(无)")	stringx+='<td width=16 align="center">'+rightchar+'</td>'
                     	stringx+="<td width=*>&nbsp;</td></tr></table>\n";
                                          
                     	for(var x=0;x<menus.length;x++)
                     	{
                      		thismenu=menus[x];
                      		if(x<0)
                      		{
                        		stringx+='<div id="menudiv'+x+'" style="visiable:none"></div>';
                        	}
                        	else
                        	{
                        		stringx+='<div id="menudiv'+x+'" style="position:absolute; z-index:'+(x+100)+';width:'+menuitemwidth+'px;overflow: visible;visibility: hidden;">\n'+
                      			'<table class=menu_space width="100%" border="0" height="100%" align="center" cellpadding="0" cellspacing="0" '+
                      			'style="'+fonts+'" >\n'    //子菜单的高度padding:2px
                      			for(var i=0;i<thismenu.items.length;i++)
                      			{
                       				var thismenuitem=thismenu.items[i];
                       				var menu_frame_class;
                       				if(i==0 && thismenu.items.length==1) menu_frame_class="3dtable_up1";
               					if(i==0 && thismenu.items.length!=1) menu_frame_class="3dtable_nobottom1";
               					if(i==thismenu.items.length-1 && thismenu.items.length!=1) menu_frame_class="3dtable_notop1";
               					if(i!=0 && i!=thismenu.items.length-1 && thismenu.items.length!=1) menu_frame_class="3dtable_nov1";
                       				if(!thismenuitem.isline)
                       				{
	
	if(x==0){
		stringx += "<tr><td  width=100% height=150px ><iframe width=100% height=125 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==1){
		stringx += "<tr><td  width=100% height=178px ><iframe width=100% height=178 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==2){
		stringx += "<tr><td  width=100% height=225px ><iframe width=100% height=225 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==3){
		stringx += "<tr><td  width=100% height=75px ><iframe width=100% height=75 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==4){
		stringx += "<tr><td  width=100% height=312px ><iframe width=100% height=312 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==5){
		stringx += "<tr><td  width=100% height=287px ><iframe width=100% height=287 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==6){
		stringx += "<tr><td  width=100% height=237px ><iframe width=100% height=237 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==7){
		stringx += "<tr><td  width=100% height=100px ><iframe width=100% height=100 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==8){
		stringx += "<tr><td  width=100% height=179px ><iframe width=100% height=179 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	if(x==9){
		stringx += "<tr><td  width=100% height=100px ><iframe width=100% height=100 frameborder=0 scrolling=no src='/layermenu"+x+"-"+i+".htm'></iframe></td></tr>\n"; 
	}
	
	}
	else
	{
        stringx+='<tr><td  height="1"></td></tr>\n';
	}
                      			}
                      		}
                      		stringx+='</table></div>\n'
                	}
                
                     	document.write(stringx);
      			break
    		}
	}
}

function write_menu()
{
	write();
}


pmenu0=new Menu('设置(F)','','self');
pmenu0.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu1=new Menu('察看(V)','','self');
pmenu1.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu2=new Menu('统计(C)','','self');
pmenu2.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu3=new Menu('图表(T)','','self');
pmenu3.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu4=new Menu('内容监控','','self');
pmenu4.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu5=new Menu('邮件拦截','','self');
pmenu5.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu6=new Menu('访问控制','','self');
pmenu6.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu7=new Menu('带宽管理','','self');
pmenu7.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu8=new Menu('系统(S)','','self');
pmenu8.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

pmenu9=new Menu('帮助(H)','','self');
pmenu9.addItem(new MenuItem('察看','goto("new.htm")','sbpm_right',false,'文件1'));

